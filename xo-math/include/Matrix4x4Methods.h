// The MIT License (MIT)
//
// Copyright (c) 2016 Jared Thomson
//
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT 
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR 
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef XOMATH_INTERNAL
static_assert(false, "Don't include Matrix4x4Methods.h directly. Include xo-math.h, which fully implements this type.");
#else // XOMATH_INTERNAL

#if defined(_XO_ASSERT_MSG)
_XOMATH_INTERNAL_MACRO_WARNING
#else
#   define _XO_ASSERT_MSG(msg) "xo-math Matrix4x4" msg
#endif

XOMATH_BEGIN_XO_NS();

Matrix4x4::Matrix4x4() {
}

Matrix4x4::Matrix4x4(float m)
#if XO_SSE
{ 
    r[0].m = r[1].m = r[2].m = r[3].m = _mm_set_ps1(m);
}
#else
{
	r[0] = Vector4(m);
	r[1] = Vector4(m);
	r[2] = Vector4(m);
	r[3] = Vector4(m);
}
#endif

Matrix4x4::Matrix4x4(float a0, float b0, float c0, float d0, float a1, float b1, float c1, float d1, float a2, float b2, float c2, float d2, float a3, float b3, float c3, float d3)
{
    r[0] = Vector4(a0, b0, c0, d0);
    r[1] = Vector4(a1, b1, c1, d1);
    r[2] = Vector4(a2, b2, c2, d2);
    r[3] = Vector4(a3, b3, c3, d3);
}

// TODO: couldn't this be faster with just a single copy?
Matrix4x4::Matrix4x4(const Matrix4x4& m)
{
	r[0] = m.r[0];
	r[1] = m.r[1];
	r[2] = m.r[2];
	r[3] = m.r[3];
}

Matrix4x4::Matrix4x4(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3)
{
	r[0] = r0;
	r[1] = r1;
	r[2] = r2;
	r[3] = r3;
}

Matrix4x4::Matrix4x4(const Vector3& r0, const Vector3& r1, const Vector3& r2)
{
	r[0] = Vector4(r0);
	r[1] = Vector4(r1);
	r[2] = Vector4(r2);
	r[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4::Matrix4x4(const class Quaternion& q) {
    Vector4* v4 = (Vector4*)&q;
    Vector4 q2 = *v4 + *v4;

    Vector4 qq2 = (*v4) * q2;
    Vector4 wq2 = q2 * q.w;

    float xy2 = q.x * q2.y;
    float xz2 = q.x * q2.z;
    float yz2 = q.y * q2.z;

    r[0] = Vector4( 1.0f - qq2.y - qq2.z,  xy2 + wq2.z,            xz2 - wq2.y,          0.0f);
    r[1] = Vector4( xy2 - wq2.z,           1.0f - qq2.x - qq2.z,   yz2 + wq2.x,          0.0f);
    r[2] = Vector4( xz2 + wq2.y,           yz2 - wq2.x,            1.0f - qq2.x - qq2.y, 0.0f);
    r[3] = Vector4::UnitW;
}

const Matrix4x4& Matrix4x4::MakeTranspose() {
#if XO_SSE
    _MM_TRANSPOSE4_PS(r[0].m, r[1].m, r[2].m, r[3].m);
#else
    float t;
#   define _XO_TRANSPOSE_SWAP(i,j) t = r[i][j]; r[i][j] = r[j][i]; r[j][i] = t;
    // The number of swaps should be:
    //      (((n*n)-n)/2) = 6
    //      (((4*4)-4)/2) = 6
    //      (((all elements)-identity)/(two elements processed per swap)) = 6
    _XO_TRANSPOSE_SWAP(0, 1);
    _XO_TRANSPOSE_SWAP(0, 2);
    _XO_TRANSPOSE_SWAP(0, 3);
    _XO_TRANSPOSE_SWAP(1, 2);
    _XO_TRANSPOSE_SWAP(1, 3);
    _XO_TRANSPOSE_SWAP(2, 3);
#   undef _XO_TRANSPOSE_SWAP
#endif
    return *this;
}

Matrix4x4 Matrix4x4::Transpose() const {
    auto m = *this;
    return m.MakeTranspose();
}

const Matrix4x4& Matrix4x4::Transform(Vector3& v) const {
    v = (*this) * Vector4(v);
    return *this;
}

const Matrix4x4& Matrix4x4::Transform(Vector4& v) const {
    v = (*this) * v;
    return *this;
}

void Matrix4x4::Scale(float xyz, Matrix4x4& m) {
    // Also valid but requires division or recip:
    // 1 0 0 0
    // 0 1 0 0
    // 0 0 1 0
    // 0 0 0 1/xyz
    m = Matrix4x4(
            xyz,  0.0f, 0.0f, 0.0f,
            0.0f, xyz,  0.0f, 0.0f,
            0.0f, 0.0f, xyz,  0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
}
 
void Matrix4x4::Scale(float x, float y, float z, Matrix4x4& m) {
    m = Matrix4x4(
            x,    0.0f, 0.0f, 0.0f,
            0.0f, y,    0.0f, 0.0f,
            0.0f, 0.0f, z,    0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
}

void Matrix4x4::Scale(const Vector3& v, Matrix4x4& m) {
    m = Matrix4x4(
            v.x,  0.0f, 0.0f, 0.0f,
            0.0f, v.y,  0.0f, 0.0f,
            0.0f, 0.0f, v.z,  0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
}
 
void Matrix4x4::Translation(float x, float y, float z, Matrix4x4& m) {
   m = Matrix4x4(
            1.0f, 0.0f, 0.0f, x   ,
            0.0f, 1.0f, 0.0f, y   ,
            0.0f, 0.0f, 1.0f, z   ,
            0.0f, 0.0f, 0.0f, 1.0f
        );
}

void Matrix4x4::Translation(const Vector3& v, Matrix4x4& m) {
    m = Matrix4x4(
            1.0f, 0.0f, 0.0f, v.x ,
            0.0f, 1.0f, 0.0f, v.y ,
            0.0f, 0.0f, 1.0f, v.z ,
            0.0f, 0.0f, 0.0f, 1.0f
        );
}

void Matrix4x4::RotationXRadians(float radians, Matrix4x4& m) {
    float cosr = Cos(radians);
    float sinr = Sin(radians);
    m = Matrix4x4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cosr,-sinr, 0.0f,
            0.0f, sinr, cosr, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    
}
 
void Matrix4x4::RotationYRadians(float radians, Matrix4x4& m) {
    float cosr = Cos(radians);
    float sinr = Sin(radians);
    m = Matrix4x4(
            cosr, 0.0f,-sinr, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            sinr, 0.0f, cosr, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
}

void Matrix4x4::RotationZRadians(float radians, Matrix4x4& m) {
    float cosr = Cos(radians);
    float sinr = Sin(radians);
    m = Matrix4x4(
            cosr,-sinr, 0.0f, 0.0f,
            sinr, cosr, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
}

void Matrix4x4::RotationRadians(float x, float y, float z, Matrix4x4& m) {
    Matrix4x4 mx, mz;
    RotationXRadians(x, mx);
    RotationYRadians(y, m);
    RotationZRadians(z, mz);
    m *= mx * mz;
}

void Matrix4x4::RotationRadians(const Vector3& v, Matrix4x4& m) {
    Matrix4x4 mx, mz;
    RotationXRadians(v.x, mx);
    RotationYRadians(v.y, m);
    RotationZRadians(v.z, mz);
    m *= mx * mz;
}

void Matrix4x4::AxisAngleRadians(const Vector3& a, float radians, Matrix4x4& m) {
    float c = Cos(radians);
    float s = Sin(radians);
    float t = 1.0f - c;
    const float& x = a.x;
    const float& y = a.y;
    const float& z = a.z;
    m = Matrix4x4(
             t*x*x+c  ,    t*x*y-z*s,  t*x*z+y*s,  0.0f,
             t*x*y+z*s,    t*y*y+c  ,  t*y*z-x*s,  0.0f,
             t*x*z-y*s,    t*y*z+x*s,  t*z*z+c  ,  0.0f,
             0.0f,         0.0f,       0.0f,       1.0f
        );
}

void Matrix4x4::RotationXDegrees(float degrees, Matrix4x4& m) {
    RotationXRadians(degrees * Deg2Rad, m);
}

void Matrix4x4::RotationYDegrees(float degrees, Matrix4x4& m) {
    RotationYRadians(degrees * Deg2Rad, m);
}

void Matrix4x4::RotationZDegrees(float degrees, Matrix4x4& m) {
    RotationZRadians(degrees * Deg2Rad, m);
}

void Matrix4x4::RotationDegrees(float x, float y, float z, Matrix4x4& m) {
    Matrix4x4 m0, m1, m2;
    RotationXDegrees(x, m0);
    RotationYDegrees(y, m1);
    RotationZDegrees(z, m2);
    m = m0 * m1 * m2;
}

void Matrix4x4::RotationDegrees(const Vector3& v, Matrix4x4& m) {
    Matrix4x4 m0, m1, m2;
    RotationXDegrees(v.x, m0);
    RotationYDegrees(v.y, m1);
    RotationZDegrees(v.z, m2);
    m = m0 * m1 * m2;
}


void Matrix4x4::AxisAngleDegrees(const Vector3& a, float degrees, Matrix4x4& m) {
    AxisAngleRadians(a, degrees * Deg2Rad, m);
}

void Matrix4x4::OrthographicProjection(float w, float h, float n, float f, Matrix4x4& m) {
    XO_ASSERT(w != 0.0f, _XO_ASSERT_MSG("::OrthographicProjection Width (w) should not be zero."));
    XO_ASSERT(h != 0.0f, _XO_ASSERT_MSG("::OrthographicProjection Height (h) should not be zero."));
    m = Matrix4x4(
            1.0f/w,    0.0f,       0.0f,    0.0f,
            0.0f,      1.0f/h,     0.0f,    0.0f,
            0.0f,      0.0f,       f-n,     0.0f,
            0.0f,      0.0f,       n*(f-n), 1.0f
        );
}
 
void Matrix4x4::PerspectiveProjectionRadians(float fovx, float fovy, float n, float f, Matrix4x4& m) {
    XO_ASSERT(n != f, _XO_ASSERT_MSG("::PerspectiveProjectionRadians Near (n) and far (f) values should not be equal."));
    m = Matrix4x4(
            ATan(fovx/2.0f),   0.0f,               0.0f,               0.0f,
            0.0f,              ATan(fovy/2.0f),    0.0f,               0.0f,
            0.0f,              0.0f,               f/(f-n),            1.0f,
            0.0f,              0.0f,               -n*(f/-n),          1.0f
        );
}

void Matrix4x4::PerspectiveProjectionDegrees(float fovx, float fovy, float near, float far, Matrix4x4& outMatrix) {
    Matrix4x4::PerspectiveProjectionRadians(fovx * Deg2Rad, fovy * Deg2Rad, near, far, outMatrix);
}

void Matrix4x4::LookAtFromPosition(const Vector3& from, const Vector3& to, const Vector3& up, Matrix4x4& m) {
    Vector3 zAxis = (to - from).Normalized();
    Vector3 xAxis = Vector3::Cross(up, zAxis).Normalized();
    Vector3 yAxis = Vector3::Cross(zAxis, xAxis);
    m = Matrix4x4(
            xAxis.x,           yAxis.x,            zAxis.x,            0.0f,
            xAxis.y,           yAxis.y,            zAxis.y,            0.0f,
            xAxis.z,           yAxis.z,            zAxis.z,            0.0f,
            -xAxis.Dot(from),  -yAxis.Dot(from),   -zAxis.Dot(from),   1.0f
        );
}

void Matrix4x4::LookAtFromPosition(const Vector3& from, const Vector3& to, Matrix4x4& m) {
    LookAtFromPosition(from, to, Vector3::Up, m);
}

void Matrix4x4::LookAtFromDirection(const Vector3& direction, const Vector3& up, Matrix4x4& m) {
    Vector3 zAxis = direction.Normalized();
    Vector3 xAxis = Vector3::Cross(up, zAxis).Normalized();
    Vector3 yAxis = Vector3::Cross(zAxis, xAxis);
    m = Matrix4x4(
            xAxis.x,           yAxis.x,            zAxis.x,            0.0f,
            xAxis.y,           yAxis.y,            zAxis.y,            0.0f,
            xAxis.z,           yAxis.z,            zAxis.z,            0.0f,
            0.0f,              0.0f,               0.0f,               1.0f
        );
}

void Matrix4x4::LookAtFromDirection(const Vector3& direction, Matrix4x4& m) {
    LookAtFromDirection(direction, Vector3::Up, m);
}

Matrix4x4 Matrix4x4::Scale(float xyz) {
    Matrix4x4 m;
    Scale(xyz, m);
    return m;
}
Matrix4x4 Matrix4x4::Scale(float x, float y, float z) {
    Matrix4x4 m;
    Scale(x, y, z, m);
    return m;
}
Matrix4x4 Matrix4x4::Scale(const Vector3& v) {
    Matrix4x4 m;
    Scale(v, m);
    return m;
}

Matrix4x4 Matrix4x4::Translation(float x, float y, float z) {
    Matrix4x4 m;
    Translation(x, y, z, m);
    return m;
}
Matrix4x4 Matrix4x4::Translation(const Vector3& v) {
    Matrix4x4 m;
    Translation(v, m);
    return m;
}

Matrix4x4 Matrix4x4::RotationXRadians(float radians) {
    Matrix4x4 m;
    RotationXRadians(radians, m);
    return m;
}
Matrix4x4 Matrix4x4::RotationYRadians(float radians) {
    Matrix4x4 m;
    RotationYRadians(radians, m);
    return m;
}
Matrix4x4 Matrix4x4::RotationZRadians(float radians) {
    Matrix4x4 m;
    RotationZRadians(radians, m);
    return m;
}
Matrix4x4 Matrix4x4::RotationRadians(float x, float y, float z) {
    Matrix4x4 m;
    RotationRadians(x, y, z, m);
    return m;
}
Matrix4x4 Matrix4x4::RotationRadians(const Vector3& v) {
    Matrix4x4 m;
    RotationRadians(v, m);
    return m;
}
Matrix4x4 Matrix4x4::AxisAngleRadians(const Vector3& axis, float radians) {
    Matrix4x4 m;
    AxisAngleRadians(axis, radians, m);
    return m;
}

Matrix4x4 Matrix4x4::RotationXDegrees(float degrees) {
    Matrix4x4 m;
    RotationXDegrees(degrees, m);
    return m;
}
Matrix4x4 Matrix4x4::RotationYDegrees(float degrees) {
    Matrix4x4 m;
    RotationYDegrees(degrees, m);
    return m;
}
Matrix4x4 Matrix4x4::RotationZDegrees(float degrees) {
    Matrix4x4 m;
    RotationZDegrees(degrees, m);
    return m;
}
Matrix4x4 Matrix4x4::RotationDegrees(float x, float y, float z) {
    Matrix4x4 m;
    RotationDegrees(x, y, z, m);
    return m;
}
Matrix4x4 Matrix4x4::RotationDegrees(const Vector3& v) {
    Matrix4x4 m;
    RotationDegrees(v, m);
    return m;
}
Matrix4x4 Matrix4x4::AxisAngleDegrees(const Vector3& axis, float degrees) {
    Matrix4x4 m;
    AxisAngleDegrees(axis, degrees, m);
    return m;
}

Matrix4x4 Matrix4x4::OrthographicProjection(float w, float h, float n, float f) {
    Matrix4x4 m;
    OrthographicProjection(w, h, n, f, m);
    return m;
}

Matrix4x4 Matrix4x4::PerspectiveProjectionRadians(float fovx, float fovy, float n, float f) {
    Matrix4x4 m;
    PerspectiveProjectionRadians(fovx, fovy, n, f, m);
    return m;
}

Matrix4x4 Matrix4x4::PerspectiveProjectionDegrees(float fovx, float fovy, float n, float f) {
    Matrix4x4 m;
    PerspectiveProjectionDegrees(fovx, fovy, n, f, m);
    return m;
}

Matrix4x4 Matrix4x4::LookAtFromPosition(const Vector3& from, const Vector3& to, const Vector3& up) {
    Matrix4x4 m;
    LookAtFromPosition(from, to, up, m);
    return m;
}
Matrix4x4 Matrix4x4::LookAtFromPosition(const Vector3& from, const Vector3& to) {
    Matrix4x4 m;
    LookAtFromPosition(from, to, m);
    return m;
}
Matrix4x4 Matrix4x4::LookAtFromDirection(const Vector3& direction, const Vector3& up) {
    Matrix4x4 m;
    LookAtFromDirection(direction, up, m);
    return m;
}
Matrix4x4 Matrix4x4::LookAtFromDirection(const Vector3& direction) {
    Matrix4x4 m;
    LookAtFromDirection(direction, m);
    return m;
}

XOMATH_END_XO_NS();

#undef _XO_ASSERT_MSG

#endif