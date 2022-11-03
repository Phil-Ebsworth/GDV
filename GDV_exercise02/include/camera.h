#ifndef CAMERA_H
#define CAMERA_H

#include <nanogui/vector.h>

#include "camera_parameters.h"
#include "common.h"
#include "exercise02.h"
#include "ray.h"

/**
 * A simple pinhole camera model, that is used to generate rays.
 */
class Camera {
    using Orthographic = CameraParameters::Orthographic;

public:
    Camera(const CameraParameters& params)
        : origin{params.pos}, cameraFrame{params.target - params.pos, params.up},
          halfViewSpan{params.halfViewSpan()}, orthographic{params.orthographic},
          isPerspective{params.isPerspective}
    {
    }

    /**
     * @brief generateRay
     * @param pixel in normalized sceen coordinates [-1,1]^2, x is towards the right, y is up
     * @return camera ray
     */
    Ray generateRay(Point2D pixel) const
    {
        if (isPerspective) {
            Point2D nearPlanePixel = pixel * halfViewSpan;
            Vector3D direction = normalize(cameraFrame.dir + cameraFrame.right * nearPlanePixel.x
                                           - cameraFrame.up * nearPlanePixel.y);
            return Ray{origin, direction};
        }
        else
            return Exercise02::orthographic(pixel, origin, cameraFrame, orthographic);
    }

private:
    Point3D origin;
    CameraFrame cameraFrame;
    /// half of the width and height of the view frustum at depth 1
    Point2D halfViewSpan;
    Orthographic orthographic;
    bool isPerspective{true};
};
#endif // CAMERA_H
