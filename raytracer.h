#pragma once

#include <memory>

class RayTracerImpl;

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();
    void init(int width, int height);
    void start();
    void stop();
    unsigned char* lockFramebuffer();
    void unlockFramebuffer();
    double getFPS() const;

private:
    std::unique_ptr<RayTracerImpl> m_impl;
};
