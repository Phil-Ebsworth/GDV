#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "aabb.h"
#include "common.h"
#include "mesh.h"
#include "ray.h"
#include "scene.h"
#include "triangle.h"
#include <limits>

struct Intersection {
    Point3D intersection_point;
    float t{std::numeric_limits<float>::infinity()}; // intersection distance
    Vertex normal;

    operator bool() const { return std::isfinite(t); }
};

bool intersect(const AABB& aabb, const IntersectionRay& ray);
Intersection intersect(const Triangle& triangle, const IntersectionRay& ray);
Intersection intersect(const Mesh& mesh, const IntersectionRay& ray);
Intersection intersect(const Scene& scene, const IntersectionRay& ray);

#endif // !INTERSECTION_H
