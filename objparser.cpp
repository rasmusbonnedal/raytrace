#include "objparser.h"
#include <fstream>
#include <sstream>

BoundingBox::BoundingBox() : m_empty(true) {}

void BoundingBox::addVertex(const Vec3d& v) {
    if (m_empty) {
        m_min = m_max = v;
        m_empty = false;
    } else {
        m_min = vmin(m_min, v);
        m_max = vmax(m_max, v);
    }
}

const Vec3d& BoundingBox::getMin() const { return m_min; }

const Vec3d& BoundingBox::getMax() const { return m_max; }

Vec3d BoundingBox::getCenter() const {
    if (m_empty) {
        return Vec3d(0, 0, 0);
    } else {
        return m_max - m_min;
    }
}

void CPUObject::init(size_t vertices, size_t faces) {
    m_vertices.reserve(vertices);
    m_faces.reserve(faces);
}

void CPUObject::pushVertex(const Vec3d& v) {
    m_vertices.push_back(v);
    m_bb.addVertex(v);
}

void CPUObject::pushFace(const Vec3u& f) { m_faces.push_back(f); }

size_t CPUObject::getNumFaces() const { return m_faces.size(); }

size_t CPUObject::getNumVertices() const { return m_vertices.size(); }

const Vec3u* CPUObject::getFacePtr() const { return &m_faces[0]; }

const Vec3d* CPUObject::getVertexPtr() const { return &m_vertices[0]; }

const BoundingBox& CPUObject::getBoundingBox() const { return m_bb; }

bool parseObj(const std::string& filename, CPUObject& cpuobject) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        return false;
    }

    std::string line;
#if 0
    size_t faces = 0;
    size_t vertices = 0;
    while (!ifs.eof()) {
        std::getline(ifs, line);
        if (line.length() == 0) {
            continue;
        }
        switch (line[0]) {
            case 'f':
                faces++;
                break;
            case 'v':
                vertices++;
                break;
        }
    }
    cpuobject.init(vertices, faces);
    ifs.seekg(0);
#endif
    while (!ifs.eof()) {
        std::getline(ifs, line);
        if (line.length() == 0) {
            continue;
        }
        char c;
        std::istringstream iss(line);
        switch (line[0]) {
            case 'f': {
                Vec3u f;
                iss >> c >> f.x >> f.y >> f.z;
                cpuobject.pushFace(f - Vec3u(1, 1, 1));
                break;
            }
            case 'v': {
                Vec3d v;
                iss >> c >> v.x >> v.y >> v.z;
                v.y = -v.y;
                cpuobject.pushVertex(v);
                break;
            }
        }
    }
    return true;
}

#if 0
void createPlane(CPUObject& cpuobject) {
    const float width = 0.5f;

    int numVertex = 0;
    for (float x = -100.0f; x < 100.0f; x += 10.0f) {
        cpuobject.pushVertex(glm::vec3(x, 0, -100.0f));
        cpuobject.pushVertex(glm::vec3(x + width, 0, -100.0f));
        cpuobject.pushVertex(glm::vec3(x + width, 0, 100.0f));
        cpuobject.pushVertex(glm::vec3(x, 0, 100.0f));
        cpuobject.pushFace(glm::uvec3(0, 1, 2) + glm::uvec3(numVertex));
        cpuobject.pushFace(glm::uvec3(0, 2, 3) + glm::uvec3(numVertex));
        numVertex += 4;
        cpuobject.pushVertex(glm::vec3(-100.0f, 0, x));
        cpuobject.pushVertex(glm::vec3(100.0f, 0, x));
        cpuobject.pushVertex(glm::vec3(100.0f, 0, x + width));
        cpuobject.pushVertex(glm::vec3(-100.0f, 0, x + width));
        cpuobject.pushFace(glm::uvec3(0, 1, 2) + glm::uvec3(numVertex));
        cpuobject.pushFace(glm::uvec3(0, 2, 3) + glm::uvec3(numVertex));
        numVertex += 4;
    }
}
#endif