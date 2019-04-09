#include "raytracer.h"

#include "color.h"
#include "intersection.h"
#include "ray.h"
#include "objparser.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
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

    Ray generateRay(const Vec2d& screenCoordinate) const {
        Vec3d direction(
            (screenCoordinate.x / m_frameBufferSize.x - 0.5) * m_frameSize.x,
            (screenCoordinate.y / m_frameBufferSize.y - 0.5) * m_frameSize.y,
            -m_focalLength);
        direction.normalize();
        return Ray(Vec3d(0, 0, 1000), direction);
    }

private:
    Vec2d m_frameSize;
    double m_focalLength;
    Vec2i m_frameBufferSize;
};

class TriScene
{
public:
    TriScene(const std::string& filename) {
        if (!parseObj(filename, m_object)) {
            throw std::runtime_error("Could not load obj file: " + filename);
        }
    }

    bool intersect(const Ray& ray, Intersection& inters) const {
        double best_t = 1e30;
        bool objhit = intersectObject(ray, inters, best_t);
        bool planehit = intersectPlane(ray, inters, best_t);
        return objhit || planehit;
    }

    bool intersectShadow(const Ray& ray) const {
        return intersectObjectShadow(ray);
    }


private:
    static bool intersectPlane(const Ray& ray, Intersection& intersection, double max_t) {
        Vec3d center = Vec3d(0, -200, 0);
        Vec3d normal = Vec3d(0, 1, 0);
        double denom = normal.dot(ray.direction);
        if (abs(denom) > 1e-10) {
            double t = (center - ray.origin).dot(normal) / denom;
            if (t >= 0 && t < max_t) {
                intersection.normal = normal;
                intersection.intersection = ray.origin + ray.direction * t;
                return true;
            }
        }
        return false;
    }

    bool intersectObjectShadow(const Ray& ray) const {
        if (!intersectAABB(ray, m_object.getBoundingBox())) {
            return false;
        }
        const Vec3d* vertices = m_object.getVertexPtr();
        size_t nFaces = m_object.getNumFaces();
        for (size_t i = 0; i < nFaces; ++i) {
            double u, v, t;
            const Vec3u& face = m_object.getFacePtr()[i];
            if (intersectTri(ray, vertices[face.x], vertices[face.y], vertices[face.z], u, v, t) && t > 0) {
                return true;
            }
        }
        return false;
    }

    bool intersectObject(const Ray& ray, Intersection& inters, double& out_best_t) const {
        if (!intersectAABB(ray, m_object.getBoundingBox())) {
            return false;
        }

        double best_t = 1e30;
        int best_tri = -1;

        const Vec3d* vertices = m_object.getVertexPtr();
        size_t nFaces = m_object.getNumFaces();
        for (size_t i = 0; i < nFaces; ++i) {
            double u, v, t;
            const Vec3u& face = m_object.getFacePtr()[i];
            if (intersectTri(ray, vertices[face.x], vertices[face.y], vertices[face.z], u, v, t)) {
                if (t < best_t) {
                    best_t = t;
                    best_tri = (int)i;
                }
            }
        }

        if (best_tri >= 0) {
            inters.intersection = ray.origin + ray.direction * best_t;
            const Vec3u& face = m_object.getFacePtr()[best_tri];
            inters.normal = (vertices[face.z] - vertices[face.x]).cross(vertices[face.y] - vertices[face.x]);
            inters.normal.normalize();
            out_best_t = best_t;
            return true;
        }
        return false;
    }

    static bool intersectTri(const Ray& r, const Vec3d& v0, const Vec3d& v1, const Vec3d& v2, double& u, double& v, double& t) {
        static const double kEpsilon = 1e-10;
        Vec3d v0v1 = v1 - v0;
        Vec3d v0v2 = v2 - v0;
        Vec3d pvec = r.direction.cross(v0v2);
        double det = v0v1.dot(pvec);
        // ray and triangle are parallel if det is close to 0
        if (abs(det) < kEpsilon) return false;
        double invDet = 1 / det;

        Vec3d tvec = r.origin - v0;
        u = tvec.dot(pvec) * invDet;
        if (u < 0 || u > 1) return false;

        Vec3d qvec = tvec.cross(v0v1);
        v = r.direction.dot(qvec) * invDet;
        if (v < 0 || u + v > 1) return false;
        t = v0v2.dot(qvec) * invDet;
        return true;
    }

    static bool intersectAABB(const Ray& r, const BoundingBox& bb) {
        double tmin = (bb.getMin().x - r.origin.x) / r.direction.x;
        double tmax = (bb.getMax().x - r.origin.x) / r.direction.x;

        if (tmin > tmax) std::swap(tmin, tmax);

        double tymin = (bb.getMin().y - r.origin.y) / r.direction.y;
        double tymax = (bb.getMax().y - r.origin.y) / r.direction.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        double tzmin = (bb.getMin().z - r.origin.z) / r.direction.z;
        double tzmax = (bb.getMax().z - r.origin.z) / r.direction.z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        return true;
    }

    CPUObject m_object;
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
    void startAsync();
    void startThreads();

    void renderer();
    void renderTile(Vec2i tile, std::vector<unsigned char>& data) const;
    void trace(int x, int y, unsigned char* pixel);
    Color trace(const Ray& ray) const;
    Color shade(const Ray& ray, const Intersection& intersection) const;
    bool getNextTile(Vec2i& tile);
    void writeTile(const Vec2i& tile, const std::vector<unsigned char>& data);
    std::mutex m_mutex;
    std::vector<std::thread> m_renderthreads;
    std::vector<unsigned char> m_framebuffer;
    int m_width, m_height;
    Vec2i m_currentTile;
    int m_framesRendered;

    double m_fps;
    double m_startTime;

    Camera m_camera;
    TriScene m_scene;
    std::atomic_bool m_stopRendering;
    bool m_isRunning;
    const int tileSize = 16;
};

RayTracerImpl::RayTracerImpl() : m_isRunning(false), m_currentTile(-16, 0), m_scene("..\\budai.obj") {}

void RayTracerImpl::init(int width, int height)
{
    std::lock_guard<std::mutex> s(m_mutex);
    m_framebuffer.resize(width * height * 3);
    m_width = width;
    m_height = height;
    m_camera = Camera(Vec2d(0.036, 0.024), 0.050, Vec2i(m_width, m_height));
}

void RayTracerImpl::start()
{
    stop();
    m_stopRendering = false;

    //startThreads();
    startAsync();

    m_isRunning = true;
    m_startTime = glfwGetTime();
    m_framesRendered = 0;
}

void RayTracerImpl::startThreads() {
    for (unsigned i = 0; i < std::thread::hardware_concurrency(); ++i) {
        m_renderthreads.push_back(std::thread(&RayTracerImpl::renderer, this));
    }
}

void RayTracerImpl::startAsync() {
    using RenderResult = std::future<std::pair<std::vector<unsigned char>, Vec2i>>;
    m_renderthreads.push_back(std::thread([this]() {
        std::vector<RenderResult> apa;
        Vec2i tile;
        while (getNextTile(tile)) {
            RenderResult f = std::async([&]() {
                std::vector<unsigned char> data(tileSize * tileSize * 3);
                renderTile(tile, data);
                std::cout << "Rendered tile" << std::endl;
                return std::make_pair(data, tile);
            });
            std::cout << "Created future" << std::endl;
            std::async([x = std::move(f), this]() {
                auto r = x.get();
                writeTile(r.second, r.first);
                std::cout << "Wrote tile" << std::endl;
            });
            apa.push_back(std::move(f));
        }
        std::cout << "Bye!" << std::endl;
    }));

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

Color RayTracerImpl::shade(const Ray& ray, const Intersection& intersection) const {
    Vec3d lightPos = Vec3d(100, -100, 300);
    Vec3d ld = lightPos - intersection.intersection;
    ld.normalize();

    Ray shadowRay(intersection.intersection + ld * 0.02, ld);
    if (!m_scene.intersectShadow(shadowRay)) {
        double diffuse = ld.dot(intersection.normal);
        return Color(Vec3d(diffuse, diffuse, diffuse));
    }
    return Color(Vec3d(0, 0, 0));
}

Color RayTracerImpl::trace(const Ray& ray) const {
    Intersection inters;
    return m_scene.intersect(ray, inters) ? shade(ray, inters) : Vec3d(0.2, 0.2, 0.8);
}


bool RayTracerImpl::getNextTile(Vec2i& tile) {
    std::lock_guard<std::mutex> s(m_mutex);
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
            return false;
        }
    }
    tile = m_currentTile;
    return true;
}

void RayTracerImpl::writeTile(const Vec2i& tile, const std::vector<unsigned char>& data) {
    std::lock_guard<std::mutex> s(m_mutex);
    int esi = 0;
    int edi = (tile.y * m_width + tile.x) * 3;
    for (int y = 0; y < tileSize; ++y)
    {
        memcpy(&m_framebuffer[edi], &data[esi], tileSize * 3);
        esi += tileSize * 3;
        edi += m_width * 3;
    }
}

void RayTracerImpl::renderTile(Vec2i tile, std::vector<unsigned char>& data) const {
    int edi = 0;
    for (int ty = 0; ty < tileSize; ++ty)
    {
        int y = tile.y + ty;
        for (int tx = 0; tx < tileSize; ++tx)
        {
            int x = tile.x + tx;
            Color8 color8 = trace(m_camera.generateRay(Vec2d(x, y))).clamp();
            data[edi] = color8.r;
            data[edi + 1] = color8.g;
            data[edi + 2] = color8.b;
            edi += 3;
        }
    }
}

void RayTracerImpl::renderer()
{
    std::vector<unsigned char> data(tileSize * tileSize * 3);

    while (!m_stopRendering)
    {
        Vec2i tile;
        if (!getNextTile(tile)) {
            return;
        }
        renderTile(tile, data);
        writeTile(tile, data);
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
