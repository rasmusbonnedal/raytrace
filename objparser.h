#pragma once

#include <string>
#include <vector>

#include "vecmath.h"

class BoundingBox {
public:
    BoundingBox();
    void addVertex(const Vec3d& v);
    const Vec3d& getMin() const;
    const Vec3d& getMax() const;
    Vec3d getCenter() const;

private:
    bool m_empty;
    Vec3d m_min, m_max;
};

class CPUObject {
public:
    void init(size_t vertices, size_t faces);
    void pushVertex(const Vec3d& v);
    void pushFace(const Vec3u& f);
    size_t getNumFaces() const;
    size_t getNumVertices() const;
    const Vec3u* getFacePtr() const;
    const Vec3d* getVertexPtr() const;
    const BoundingBox& getBoundingBox() const;

private:
    std::vector<Vec3d> m_vertices;
    std::vector<Vec3u> m_faces;
    BoundingBox m_bb;
};

bool parseObj(const std::string& filename, CPUObject& cpuobject);
