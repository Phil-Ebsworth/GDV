#include "exercise02.h"
#include <cmath>

bool Exercise02::intersect(const AABB& aabb, const IntersectionRay& ray)
{
    // TODO: return whether the ray intersects the bounding box

    return false;
}

Intersection Exercise02::intersect(const Triangle& triangle, const IntersectionRay& ray)
{
    // TODO: return the intersection of the ray and the triangle (if any)
    // make sure to set the "intersection_point", distance "t", and "normal"

    return {};
}

Ray Exercise02::orthographic(const Point2D& pixel, const Point3D& origin,
                             const CameraFrame& cameraFrame,
                             const CameraParameters::Orthographic& orthographic)
{
    // TODO: return the camera ray for an orthographic camera
    // the given pixel values are in normalized screen coordinates [-1,1]^2

    return {};
}
