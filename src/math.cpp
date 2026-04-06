#include <iostream>
#include <cmath>

#include "math.h"

Vec3 mxv(const Mat4& mat, const Vec3& v)
{
    Vec3 result;

    result.x = mat.m[0][0] * v.x +
               mat.m[0][1] * v.y +
               mat.m[0][2] * v.z +
               mat.m[0][3] * 1.0f;

    result.y = mat.m[1][0] * v.x +
               mat.m[1][1] * v.y +
               mat.m[1][2] * v.z +
               mat.m[1][3] * 1.0f;

    result.z = mat.m[2][0] * v.x +
               mat.m[2][1] * v.y +
               mat.m[2][2] * v.z +
               mat.m[2][3] * 1.0f;

    return result; 
}

Mat4 mxm(const Mat4& a, const Mat4& b)
{
    Mat4 result;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            result.m[i][j] = 0.0f;

            for (int k = 0; k < 4; k++)
            {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }

    return result;
}

Mat4 trans_m(float tx, float ty, float tz)
{
    Mat4 mat;

    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;

    return mat;
}

Mat4 scale_m(float sx, float sy, float sz)
{
    Mat4 mat;
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    mat.m[2][2] = sz;

    return mat;
}

Mat4 rot_x(float theta)
{
    Mat4 m;
    float c = cos(theta);
    float s = sin(theta);

    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;

    return m;
}

Mat4 rot_y(float theta)
{
    Mat4 m;
    float c = cos(theta);
    float s = sin(theta);

    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;

    return m;
}

Mat4 rot_z(float theta)
{
    Mat4 m;
    float c = cos(theta);
    float s = sin(theta);

    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;

    return m;
}

Mat4 rot_axis(float theta, float ux, float uy, float uz)
{
    Mat4 m;

    float c = cos(theta);
    float s = sin(theta);
    float t = 1 - c;

    m.m[0][0] = t*ux*ux + c;
    m.m[0][1] = t*ux*uy - s*uz;
    m.m[0][2] = t*ux*uz + s*uy;

    m.m[1][0] = t*ux*uy + s*uz;
    m.m[1][1] = t*uy*uy + c;
    m.m[1][2] = t*uy*uz - s*ux;

    m.m[2][0] = t*ux*uz - s*uy;
    m.m[2][1] = t*uy*uz + s*ux;
    m.m[2][2] = t*uz*uz + c;

    return m;
}

int compute_outcode(float x, float y)
{
    int code = INSIDE;

    if (x < -1) code |= LEFT;
    else if (x > 1) code |= RIGHT;

    if (y < -1) code |= BOTTOM;
    else if (y > 1) code |= TOP;

    return code;
}