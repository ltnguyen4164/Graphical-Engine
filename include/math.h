#pragma once
#ifndef MATH_H
#define MATH_H

#define INSIDE 0
#define LEFT 1
#define RIGHT 2
#define BOTTOM 4
#define TOP 8

struct Vec3 {
    float x, y, z;
};

struct Mat4 {
    float m[4][4];

    Mat4()
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }
};

// matrix operations
Vec3 mxv(const Mat4& mat, const Vec3& v);
Mat4 mxm(const Mat4& a, const Mat4& b);

// transform constructors
Mat4 trans_m(float tx, float ty, float tz);
Mat4 scale_m(float sx, float sy, float sz);
Mat4 rot_x(float theta);
Mat4 rot_y(float theta);
Mat4 rot_z(float theta);
Mat4 rot_axis(float theta, float ux, float uy, float uz);

// clipping operations
int compute_outcode(float x, float y);

#endif