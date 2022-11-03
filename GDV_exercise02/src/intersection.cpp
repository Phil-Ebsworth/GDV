#include "intersection.h"

#include "exercise02.h"

bool intersect(const AABB& aabb, const IntersectionRay& ray)
{
    return Exercise02::intersect(aabb, ray);
}

Intersection intersect(const Triangle& triangle, const IntersectionRay& ray)
{
    return Exercise02::intersect(triangle, ray);
}

Intersection intersect(const Mesh& mesh, const IntersectionRay& ray)
{
    Intersection result;

    if (intersect(mesh.getBounds(), ray)) {
        for (const auto& face : mesh.getFaces()) {
            const Triangle triangle = mesh.getTriangleFromFace(face);

            Intersection its = intersect(triangle, ray);

            if (its.t < result.t)
                result = its;
        }
    }
    return result;
}

Intersection intersect(const Scene& scene, const IntersectionRay& ray)
{
    Intersection result;

    // brute force intersection tests
    for (const Mesh& mesh : scene.getMeshes()) {
        Intersection its = intersect(mesh, ray);

        if (its.t < result.t)
            result = its;
    }

    return result;
}
