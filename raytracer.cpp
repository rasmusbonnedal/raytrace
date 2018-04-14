#include "raytracer.h"

#include "color.h"
#include "intersection.h"
#include "ray.h"

#include <GLFW/glfw3.h>

#include <math.h>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

class Camera
{
public:
    Camera() {}

    Camera(Vec2d frameSize, double focalLength, Vec2i frameBufferSize)
        : m_frameSize(frameSize),
          m_focalLength(focalLength),
          m_frameBufferSize(frameBufferSize)
    {
    }

    Ray generateRay(const Vec2d& screenCoordinate)
    {
        Vec3d direction(
            (screenCoordinate.x / m_frameBufferSize.x - 0.5) * m_frameSize.x,
            (screenCoordinate.y / m_frameBufferSize.y - 0.5) * m_frameSize.y,
            m_focalLength);
        direction.normalize();
        return Ray(Vec3d(0, 0, 0), direction);
    }

private:
    Vec2d m_frameSize;
    double m_focalLength;
    Vec2i m_frameBufferSize;
};

class Scene
{
public:
    Scene() {}

    void addSphere(Vec3d center, double radius)
    {
        m_spheres.push_back(Sphere(center, radius * radius));
    }

    bool intersect(const Ray& ray, Intersection& inters)
    {
        double best_t = 1e30;
        Sphere* best_sphere = 0;

        for (auto& sphere : m_spheres)
        {
            double t;
            if (intersectSphere(ray, sphere, t) && t < best_t)
            {
                best_t = t;
                best_sphere = &sphere;
            }
        }
        if (best_sphere)
        {
            inters.intersection = ray.origin + ray.direction * best_t;
            inters.normal = inters.intersection - best_sphere->center;
            inters.normal.normalize();
        }
        return best_sphere != 0;
    }

private:
    struct Sphere
    {
        Sphere(const Vec3d& center, double radius2)
            : center(center), radius2(radius2)
        {
        }
        Vec3d center;
        double radius2;
    };

    bool intersectSphere(const Ray& ray, const Sphere& s, double& t)
    {
        Vec3d L = ray.origin - s.center;
        double b = 2 * ray.direction.dot(L);
        double c = L.dot(L) - s.radius2;
        double discr = b * b - 4 * c;
        double x0, x1;
        if (discr < 0)
        {
            return false;
        }
        else if (discr == 0)
        {
            x0 = x1 = -0.5 * b;
        }
        double sqrtDiscr = sqrt(discr);
        double q = (b > 0) ? -0.5 * (b + sqrtDiscr) : -0.5 * (b - sqrtDiscr);
        x0 = q;
        x1 = c / q;
        if (x0 > x1) std::swap(x0, x1);
        if (x0 < 0)
        {
            x0 = x1;
            if (x0 < 0) return false;
        }
        t = x0;
        return true;
    }

    std::vector<Sphere> m_spheres;
};

class RayTracerImpl
{
public:
    RayTracerImpl();
    void init(int width, int height);
    void start();
    void stop();
    unsigned char* lockFramebuffer();
    void unlockFramebuffer();
    double getFPS() const;

private:
    void renderer();
    void trace(int x, int y, unsigned char* pixel);
    std::mutex m_mutex;
    std::vector<std::thread> m_renderthreads;
    std::vector<unsigned char> m_framebuffer;
    int m_width, m_height;
    Vec2i m_currentTile;
    int m_framesRendered;

    double m_fps;
    double m_startTime;

    Camera m_camera;
    Scene m_scene;
    std::atomic_bool m_stopRendering;
    bool m_isRunning;
};

RayTracerImpl::RayTracerImpl() : m_isRunning(false), m_currentTile(0, 0) {}

void RayTracerImpl::init(int width, int height)
{
    std::lock_guard<std::mutex> s(m_mutex);
    m_framebuffer.resize(width * height * 3);
    m_width = width;
    m_height = height;
    m_camera = Camera(Vec2d(0.036, 0.024), 0.050, Vec2i(m_width, m_height));
    m_scene.addSphere(Vec3d(0, 0, 6), 1);
    m_scene.addSphere(Vec3d(0, -20, 6), 19);
}

void RayTracerImpl::start()
{
    stop();
    m_stopRendering = false;
    for (unsigned i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        m_renderthreads.push_back(std::thread(&RayTracerImpl::renderer, this));
    }
    m_isRunning = true;
    m_startTime = glfwGetTime();
    m_framesRendered = 0;
}

void RayTracerImpl::stop()
{
    if (m_isRunning)
    {
        m_stopRendering = true;
        for (auto& t : m_renderthreads)
        {
            t.join();
        }
        m_renderthreads.clear();
        m_isRunning = false;
    }
}

unsigned char* RayTracerImpl::lockFramebuffer()
{
    m_mutex.lock();
    return &m_framebuffer[0];
}

void RayTracerImpl::unlockFramebuffer() { m_mutex.unlock(); }

void RayTracerImpl::renderer()
{
    int gx = -1;
    int gy = -1;
    const int tileSize = 16;

    std::vector<unsigned char> tile(tileSize * tileSize * 3);

    while (!m_stopRendering)
    {
        if (gx >= 0)
        {
            int edi = 0;
            for (int ty = 0; ty < tileSize; ++ty)
            {
                int y = gy + ty;
                for (int tx = 0; tx < tileSize; ++tx)
                {
                    int x = gx + tx;
                    Ray ray = m_camera.generateRay(Vec2d(x, y));
                    Intersection inters;
                    Color color;
                    if (m_scene.intersect(ray, inters))
                    {
                        color = inters.normal * 0.5 + Vec3d(0.5, 0.5, 0.5);
                    }
                    else
                    {
                        color = Vec3d(0, 0, 0);
                    }
                    Color8 color8 = color.clamp();
                    tile[edi] = color8.r;
                    tile[edi + 1] = color8.g;
                    tile[edi + 2] = color8.b;
                    edi += 3;
                }
            }
        }
        {
            std::lock_guard<std::mutex> s(m_mutex);
            if (gx >= 0)
            {
                int esi = 0;
                int edi = (gy * m_width + gx) * 3;
                for (int y = 0; y < tileSize; ++y)
                {
                    memcpy(&m_framebuffer[edi], &tile[esi], tileSize * 3);
                    esi += tileSize * 3;
                    edi += m_width * 3;
                }
            }
            m_currentTile.x += tileSize;
            if (m_currentTile.x + tileSize > m_width)
            {
                m_currentTile.x = 0;
                m_currentTile.y += tileSize;
                if (m_currentTile.y + tileSize > m_height)
                {
                    m_currentTile.y = 0;
                    m_framesRendered++;
                    double t = glfwGetTime();
                    if (m_startTime + 1.0 < t)
                    {
                        m_fps = m_framesRendered / (t - m_startTime);
                        m_framesRendered = 0;
                        m_startTime = t;
                    }
                }
            }
            gx = m_currentTile.x;
            gy = m_currentTile.y;
        }
    }
}

double RayTracerImpl::getFPS() const { return m_fps; }

void RayTracerImpl::trace(int x, int y, unsigned char* pixel) {}

RayTracer::RayTracer() : m_impl(new RayTracerImpl) {}

RayTracer::~RayTracer() {}

void RayTracer::init(int width, int height) { m_impl->init(width, height); }

void RayTracer::start() { m_impl->start(); }

void RayTracer::stop() { m_impl->stop(); }

unsigned char* RayTracer::lockFramebuffer()
{
    return m_impl->lockFramebuffer();
}

void RayTracer::unlockFramebuffer() { m_impl->unlockFramebuffer(); }

double RayTracer::getFPS() const { return m_impl->getFPS(); }
