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

#define _XO_MATH_OBJ
#include "xo-math.h"

XOMATH_BEGIN_XO_NS();

#if defined(_XONOCONSTEXPR)
#   if defined(XO_SSE)
const float Quaternion::Epsilon = sse::SSEFloatEpsilon * 4.0f;
#   else
const float Quaternion::Epsilon = FloatEpsilon * 4.0f;
#   endif
#endif

const Quaternion Quaternion::Identity(0.0f, 0.0f, 0.0f, 1.0f);
const Quaternion Quaternion::Zero(0.0f, 0.0f, 0.0f, 0.0f);

namespace xo_internal
{
    _XOINL float QuaternionSquareSum(const Quaternion& q)
    {
#if defined(XO_SSE)
        __m128 square = _mm_mul_ps(q.xmm, q.xmm);
        square = _mm_hadd_ps(square, square);
        square = _mm_hadd_ps(square, square);
        return _mm_cvtss_f32(square);
#else
        return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
#endif
    }
}

Quaternion::Quaternion()
{
}

Quaternion::Quaternion(const Matrix4x4& mat)
{
    Vector3 xAxis(mat[0]);
    Vector3 yAxis(mat[1]);
    Vector3 zAxis(mat[2]);

    Vector3 scale(xAxis.Magnitude(), yAxis.Magnitude(), zAxis.Magnitude());

    // don't use close enough, skip the abs since we're all positive value.
    // todo: do we actually care about near-zero?
    if (scale.x <= FloatEpsilon || scale.y <= FloatEpsilon || scale.z <= FloatEpsilon)
    {
#if defined(XO_SSE)

#else
        w = 1.0f;
        x = y = z = 0.0f;
#endif
        return; // too close.
    }

#if defined(XO_SSE)
#   if defined(XO_NO_INVERSE_DIVISION)
    Vector3 recipScale = Vector3(_mm_div_ps(Vector4::One.xmm, scale.xmm));
#   else
    Vector3 recipScale = Vector3(_mm_rcp_ps(scale.xmm));
#   endif
#else
    Vector3 recipScale = Vector3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);
#endif
    xAxis *= recipScale.x;
    yAxis *= recipScale.y;
    zAxis *= recipScale.z;

    // Now calculate the rotation from the resulting matrix (axes).
    float trace = xAxis.x + yAxis.y + zAxis.z + 1.0f;

    if (trace > 1.0f)
    {
        float s = 0.5f / Sqrt(trace);
        _XO_ASSIGN_QUAT(
            0.25f / s,
            (yAxis.z - zAxis.y) * s,
            (xAxis.y - yAxis.x) * s,
            (zAxis.x - xAxis.z) * s);
    }
    else
    {
        if (xAxis.x > yAxis.y && xAxis.x > zAxis.z)
        {
            float s = 0.5f / Sqrt(1.0f + xAxis.x - yAxis.y - zAxis.z);
            _XO_ASSIGN_QUAT(
                (yAxis.z - zAxis.y) * s,
                0.25f / s,
                (zAxis.x + xAxis.z) * s,
                (yAxis.x + xAxis.y) * s);
        }
        else if (yAxis.y > zAxis.z)
        {
            float s = 0.5f / Sqrt(1.0f + yAxis.y - xAxis.x - zAxis.z);
            _XO_ASSIGN_QUAT(
                (zAxis.x - xAxis.z) * s,
                (yAxis.x + xAxis.y) * s,
                (zAxis.y + yAxis.z) * s,
                0.25f / s);
        }
        else
        {
            float s = 0.5f / Sqrt(1.0f + zAxis.z - xAxis.x - yAxis.y);
            _XO_ASSIGN_QUAT(
                (xAxis.y - yAxis.x) * s,
                (zAxis.x + xAxis.z) * s,
                0.25f / s,
                (zAxis.y + yAxis.z) * s);
        }
    }
}

Quaternion::Quaternion(float x, float y, float z, float w) :
#if defined(XO_SSE)
    xmm(_mm_set_ps(w, z, y, x))
#else
    x(x), y(y), z(z), w(w)
#endif
{
}

Quaternion Quaternion::Inverse() const
{
    return Quaternion(*this).MakeInverse();
}

Quaternion& Quaternion::MakeInverse()
{
    float magnitude = xo_internal::QuaternionSquareSum(*this);

    if (CloseEnough(magnitude, 1.0f, Epsilon))
    {
        return MakeConjugate();
    }
    if (CloseEnough(magnitude, 0.0f, Epsilon))
    {
        return *this;
    }

    MakeConjugate();
    (*(Vector4*)this) /= magnitude;
    return *this;
}

Quaternion Quaternion::Normalized() const
{
    return Quaternion(*this).Normalize();
}

Quaternion& Quaternion::Normalize()
{
    float magnitude = xo_internal::QuaternionSquareSum(*this);
    if (CloseEnough(magnitude, 1.0f, Epsilon))
    {
        return *this;
    }

    magnitude = Sqrt(magnitude);
    if (CloseEnough(magnitude, 0.0f, Epsilon))
    {
        return *this;
    }

    (*(Vector4*)this) /= magnitude;
    return *this;
}

Quaternion Quaternion::Conjugate() const
{
    return Quaternion(*this).MakeConjugate();
}

Quaternion& Quaternion::MakeConjugate()
{
    _XO_ASSIGN_QUAT(w, -x, -y, -z);
    return *this;
}

void Quaternion::GetAxisAngleRadians(Vector3& axis, float& radians) const
{
    Quaternion q = Normalized();

#if defined(XO_SSE)
    // todo: don't we need to normalize axis in sse too?
    axis.xmm = q.xmm;
#else
    axis.x = q.x;
    axis.y = q.y;
    axis.z = q.z;
    axis.Normalize();
#endif
    radians = (2.0f * ACos(q.w));
}

void Quaternion::RotationRadians(float x, float y, float z, Quaternion& outQuat)
{
    RotationRadians(Vector3(x, y, z), outQuat);
}

void Quaternion::RotationRadians(const Vector3& v, Quaternion& outQuat)
{
    Vector3 hv = v * 0.5f;
    _XOSIMDALIGN float s[3];
    _XOSIMDALIGN float c[3];
    SinCos_x3(hv.f, s, c);

    _XO_ASSIGN_QUAT_Q(outQuat,
                      c[0] * c[1] * c[2] + s[0] * s[1] * s[2],
                      s[0] * c[1] * c[2] - c[0] * s[1] * s[2],
                      c[0] * s[1] * c[2] + s[0] * c[1] * s[2],
                      c[0] * c[1] * s[2] - s[0] * s[1] * c[2]);
}

void Quaternion::AxisAngleRadians(const Vector3& axis, float radians, Quaternion& outQuat)
{
    float hr = radians * 0.5f;
    float sr = Sin(hr);

    Vector3 n = axis.Normalized();
    n *= sr;
    _XO_ASSIGN_QUAT_Q(outQuat, Cos(radians), n.x, n.y, n.z);
}

void Quaternion::LookAtFromPosition(const Vector3& from, const Vector3& to, const Vector3& up, Quaternion& outQuat)
{
    LookAtFromDirection(to - from, up, outQuat);
}

void Quaternion::LookAtFromPosition(const Vector3& from, const Vector3& to, Quaternion& outQuat)
{
    LookAtFromPosition(from, to, Vector3::Up, outQuat);
}

void Quaternion::LookAtFromDirection(const Vector3&, const Vector3&, Quaternion&)
{
    // Todo
}

void Quaternion::LookAtFromDirection(const Vector3& direction, Quaternion& outQuat)
{
    LookAtFromDirection(direction, Vector3::Up, outQuat);
}

void Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t, Quaternion& outQuat)
{
    //      The folowing copyright and licence applies to the contents of this Quaternion::Slerp method

    //      Copyright 2013 BlackBerry Inc.
    //      Licensed under the Apache License, Version 2.0 (the "License");
    //      you may not use this file except in compliance with the License.
    //      You may obtain a copy of the License at
    //      http://www.apache.org/licenses/LICENSE-2.0
    //      Unless required by applicable law or agreed to in writing, software
    //      distributed under the License is distributed on an "AS IS" BASIS,
    //      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    //      See the License for the specific language governing permissions and
    //      limitations under the License.

    //      Original file from GamePlay3D: http://gameplay3d.org

    //      Modified by Jared Thomson in 2016:
    //        - removed assertions
    //        - made compatable with xo-math

    // Fast slerp implementation by kwhatmough:
    // It contains no division operations, no trig, no inverse trig
    // and no sqrt. Not only does this code tolerate small constraint
    // errors in the input quaternions, it actually corrects for them.
    if (CloseEnough(t, 0.0f))
    {
        outQuat = a;
        return;
    }
    else if (CloseEnough(t, 1.0f))
    {
        outQuat = b;
        return;
    }
    else if (a == b)
    {
        outQuat = a;
    }
    else
    {
        float halfY, alpha, beta;
        float u, f1, f2a, f2b;
        float ratio1, ratio2;
        float halfSecHalfTheta, versHalfTheta;
        float sqNotU, sqU;

        Vector4 * va = (Vector4*)&a;
        Vector4 * vb = (Vector4*)&b;

        float cosTheta = ((*va) * (*vb)).Sum();

        // As usual in all slerp implementations, we fold theta.
        alpha = cosTheta >= 0 ? 1.0f : -1.0f;
        halfY = 1.0f + alpha * cosTheta;

        // Here we bisect the interval, so we need to fold t as well.
        f2b = t - 0.5f;
        u = f2b >= 0 ? f2b : -f2b;
        f2a = u - f2b;
        f2b += u;
        u += u;
        f1 = 1.0f - u;

        // One iteration of Newton to get 1-cos(theta / 2) to good accuracy.
        halfSecHalfTheta = 1.09f - (0.476537f - 0.0903321f * halfY) * halfY;
        halfSecHalfTheta *= 1.5f - halfY * halfSecHalfTheta * halfSecHalfTheta;
        versHalfTheta = 1.0f - halfY * halfSecHalfTheta;

        // Evaluate series expansions of the coefficients.
        sqNotU = f1 * f1;
        ratio2 = 0.0000440917108f * versHalfTheta;
        ratio1 = -0.00158730159f + (sqNotU - 16.0f) * ratio2;
        ratio1 = 0.0333333333f + ratio1 * (sqNotU - 9.0f) * versHalfTheta;
        ratio1 = -0.333333333f + ratio1 * (sqNotU - 4.0f) * versHalfTheta;
        ratio1 = 1.0f + ratio1 * (sqNotU - 1.0f) * versHalfTheta;

        sqU = u * u;
        ratio2 = -0.00158730159f + (sqU - 16.0f) * ratio2;
        ratio2 = 0.0333333333f + ratio2 * (sqU - 9.0f) * versHalfTheta;
        ratio2 = -0.333333333f + ratio2 * (sqU - 4.0f) * versHalfTheta;
        ratio2 = 1.0f + ratio2 * (sqU - 1.0f) * versHalfTheta;

        // Perform the bisection and resolve the folding done earlier.
        f1 *= ratio1 * halfSecHalfTheta;
        f2a *= ratio2;
        f2b *= ratio2;
        alpha *= f1 + f2a;
        beta = f1 + f2b;

        // Apply final coefficients to a and b as usual.
        float w = alpha * a.w + beta * b.w;
        float x = alpha * a.x + beta * b.x;
        float y = alpha * a.y + beta * b.y;
        float z = alpha * a.z + beta * b.z;

        // This final adjustment to the quaternion's length corrects for
        // any small constraint error in the inputs q1 and q2 But as you
        // can see, it comes at the cost of 9 additional multiplication
        // operations. If this error-correcting feature is not required,
        // the following code may be removed.
        f1 = 1.5f - 0.5f * (w * w + x * x + y * y + z * z);
        _XO_ASSIGN_QUAT_Q(outQuat, w * f1, x * f1, y * f1, z * f1);
    }
}

void Quaternion::Lerp(const Quaternion& a, const Quaternion& b, float t, Quaternion& outQuat)
{
    Vector4& vq = (Vector4&)outQuat;
    const Vector4& va = a;
    const Vector4& vb = b;
    // Todo: give the vectors the same static 'outQuat' type interface, then just use the Vector4::Lerp for this lerp
    vq = va + (vb - va) * t;
}

XOMATH_END_XO_NS();