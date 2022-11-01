#include "exercise01.h"
#include <cmath>

Vertex Exercise01::scale(Vertex v, float sx, float sy, float sz)
{
    // TODO: scale the input vector v in x, y, and, z
    v[1] = v[1] * sx;
    v[2] = v[2] * sy;
    v[3] = v[3] * sz;
    return v;
}

Vertex Exercise01::translate(Vertex v, float tx, float ty, float tz)
{
    // TODO: translate the input vector v in x, y, and, z
    v[1] = v[1] * sx;
    v[2] = v[2] * sy;
    v[3] = v[3] * sz;
    return v;
}

Vertex Exercise01::rotateX(Vertex v, float angle)
{
    // TODO: rotate the input vector around the x axis (angle is given in radians)
    v[1] = v[1];
    v[2] = v[2] * cos[angle] - v[3] * sin[angle];
    v[3] = v[2] * cos[angle] + v[3] * sin[angle];
    return v;
}

Vertex Exercise01::rotateY(Vertex v, float angle)
{
    // TODO: rotate the input vector around the y axis (angle is given in radians)
    v[1] = v[1] * cos(angle) + v[3] * sin(angle);
    v[2] = v[2];
    v[3] = v[1] * sin[-angle] + v[3] * cos[angle];
    return v;
}

Vertex Exercise01::rotateZ(Vertex v, float angle)
{
    // TODO: rotate the input vector around the z axis (angle is given in radians)
    v[1] = v[1] * cos[angle] + v[2] * sin[-angle];
    v[2] = v[1] * sin[angle] + v[2] * cos[angle];
    v[3] = v[3];
    return v;
}
