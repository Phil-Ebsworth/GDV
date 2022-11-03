#ifndef COMMON_H
#define COMMON_H

#if __cplusplus < 201703L
#error "C++17 or newer is required"
#endif
#ifdef __has_include
#if __has_include(<version>)
#include <version>
#endif
#endif

#if __cpp_lib_math_constants >= 201907L
#include <numbers>
#endif

#include <cstdint>

#include "point2d.h"
#include "point3d.h"

// define some type aliases (all of these have x,y,z coordinates)

using Vertex = Point3D;
using Vector3D = Point3D;
using Normal3D = Vector3D;
using TextureCoordinate = Point2D;

// forward declarations

class Mesh;
struct TriangleIndices;

class Integrator;
class Scene;
class Camera;
class Film;

#if __cpp_lib_math_constants >= 201907L
static constexpr float pi = std::numbers::pi_v<float>;
#else
static constexpr float pi = 3.14159265359f;
#endif
static constexpr float degToRad = pi / 180.0f;
static constexpr float epsilon = 1.0e-6f;

#endif // COMMON_H
