#ifndef XOMATH_INTERNAL
static_assert(false, "Don't include Vector2.h directly. Include GameMath.h, which fully implements this type.");
#else // XOMATH_INTERNAL

XOMATH_BEGIN_XO_NS
class _MM_ALIGN16 Vector2 {
public:
    constexpr static const float Epsilon = FloatEpsilon * 2.0f;

    // No initialization is done.
    _XOINL Vector2();
    
    _XOINL Vector2(float v);
    _XOINL Vector2(float x, float y);
    _XOINL Vector2(const Vector2& v);
    _XOINL Vector2(const class Vector3& v);
    _XOINL Vector2(const class Vector4& v);

    _XOINL const Vector2& Set(float x, float y);
    _XOINL const Vector2& Set(float v);
    _XOINL const Vector2& Set(const Vector2& v);

    _XOINL void Get(float& x, float& y) const;
    _XOINL void Get(float* f) const;

    _XOINL float& operator [](int i);
    _XOINL const float& operator [](int i) const;

    _XOINL Vector2 operator - () const;
    _XOINL Vector2 operator ~ () const;

    _XOINL const Vector2& operator += (const Vector2& v);
    _XOINL const Vector2& operator += (float v);
    _XOINL const Vector2& operator += (double v);
    _XOINL const Vector2& operator += (int v);
    _XOINL const Vector2& operator += (const class Vector3& v);
    _XOINL const Vector2& operator += (const class Vector4& v);

    _XOINL const Vector2& operator -= (const Vector2& v);
    _XOINL const Vector2& operator -= (float v);
    _XOINL const Vector2& operator -= (double v);
    _XOINL const Vector2& operator -= (int v);
    _XOINL const Vector2& operator -= (const class Vector3& v);
    _XOINL const Vector2& operator -= (const class Vector4& v);

    _XOINL const Vector2& operator *= (const Vector2& v);
    _XOINL const Vector2& operator *= (float v);
    _XOINL const Vector2& operator *= (double v);
    _XOINL const Vector2& operator *= (int v);
    _XOINL const Vector2& operator *= (const class Vector3& v);
    _XOINL const Vector2& operator *= (const class Vector4& v);

    _XOINL const Vector2& operator /= (const Vector2& v);
    _XOINL const Vector2& operator /= (float v);
    _XOINL const Vector2& operator /= (double v);
    _XOINL const Vector2& operator /= (int v);
    _XOINL const Vector2& operator /= (const class Vector3& v);
    _XOINL const Vector2& operator /= (const class Vector4& v);

    _XOINL Vector2 operator + (const Vector2& v) const;
    _XOINL Vector2 operator + (float v) const;
    _XOINL Vector2 operator + (double v) const;
    _XOINL Vector2 operator + (int v) const;
    _XOINL Vector2 operator + (const class Vector3& v) const;
    _XOINL Vector2 operator + (const class Vector4& v) const;

    _XOINL Vector2 operator - (const Vector2& v) const;
    _XOINL Vector2 operator - (float v) const;
    _XOINL Vector2 operator - (double v) const;
    _XOINL Vector2 operator - (int v) const;
    _XOINL Vector2 operator - (const class Vector3& v) const;
    _XOINL Vector2 operator - (const class Vector4& v) const;

    _XOINL Vector2 operator * (const Vector2& v) const;
    _XOINL Vector2 operator * (float v) const;
    _XOINL Vector2 operator * (double v) const;
    _XOINL Vector2 operator * (int v) const;
    _XOINL Vector2 operator * (const class Vector3& v) const;
    _XOINL Vector2 operator * (const class Vector4& v) const;

    _XOINL Vector2 operator / (const Vector2& v) const;
    _XOINL Vector2 operator / (float v) const;
    _XOINL Vector2 operator / (double v) const;
    _XOINL Vector2 operator / (int v) const;
    _XOINL Vector2 operator / (const class Vector3& v) const;
    _XOINL Vector2 operator / (const class Vector4& v) const;

    _XOINL bool operator < (const Vector2& v) const;
    _XOINL bool operator < (float v) const;
    _XOINL bool operator < (double v) const;
    _XOINL bool operator < (int v) const;
    _XOINL bool operator < (const class Vector3& v) const;
    _XOINL bool operator < (const class Vector4& v) const;

    _XOINL bool operator <= (const Vector2& v) const;
    _XOINL bool operator <= (float v) const;
    _XOINL bool operator <= (double v) const;
    _XOINL bool operator <= (int v) const;
    _XOINL bool operator <= (const class Vector3& v) const;
    _XOINL bool operator <= (const class Vector4& v) const;

    _XOINL bool operator > (const Vector2& v) const;
    _XOINL bool operator > (float v) const;
    _XOINL bool operator > (double v) const;
    _XOINL bool operator > (int v) const;
    _XOINL bool operator > (const class Vector3& v) const;
    _XOINL bool operator > (const class Vector4& v) const;

    _XOINL bool operator >= (const Vector2& v) const;
    _XOINL bool operator >= (float v) const;
    _XOINL bool operator >= (double v) const;
    _XOINL bool operator >= (int v) const;
    _XOINL bool operator >= (const class Vector3& v) const;
    _XOINL bool operator >= (const class Vector4& v) const;

    _XOINL bool operator == (const Vector2& v) const;
    _XOINL bool operator == (float v) const;
    _XOINL bool operator == (double v) const;
    _XOINL bool operator == (int v) const;
    _XOINL bool operator == (const class Vector3& v) const;
    _XOINL bool operator == (const class Vector4& v) const;

    _XOINL bool operator != (const Vector2& v) const;
    _XOINL bool operator != (float v) const;
    _XOINL bool operator != (double v) const;
    _XOINL bool operator != (int v) const;
    _XOINL bool operator != (const class Vector3& v) const;
    _XOINL bool operator != (const class Vector4& v) const;

    _XOINL float Sum() const;
    
    _XOINL float Magnitude() const;
    _XOINL float MagnitudeSquared() const;
    _XOINL const Vector2& Normalize();
    _XOINL Vector2 Normalized() const;
    _XOINL bool IsZero() const;
    _XOINL bool IsNormalized() const;

    _XOINL static float Dot(const Vector2& a, const Vector2& b);
    _XOINL static float Cross(const Vector2& a, const Vector2& b);
    _XOINL static float AngleRadians(const Vector2& a, const Vector2& b);
    _XOINL static float AngleDegrees(const Vector2& a, const Vector2& b);

    _XOINL static void Max(const Vector2& a, const Vector2& b, Vector2& outVec);
    _XOINL static void Min(const Vector2& a, const Vector2& b, Vector2& outVec);

    // input vector rotated 90 degrees
    _XOINL static void OrthogonalCCW(const Vector2& v, Vector2& outVec);

    // input vector rotated -90 degrees
    _XOINL static void OrthogonalCW(const Vector2& v, Vector2& outVec);
    _XOINL static void Lerp(const Vector2& a, const Vector2& b, float t, Vector2& outVec);

    _XOINL static Vector2 Max(const Vector2& a, const Vector2& b);
    _XOINL static Vector2 Min(const Vector2& a, const Vector2& b);
    _XOINL static Vector2 OrthogonalCCW(const Vector2& v);
    _XOINL static Vector2 OrthogonalCW(const Vector2& v);
    _XOINL static Vector2 Lerp(const Vector2& a, const Vector2& b, float t);

    _XOINL float Dot(const Vector2& v) const;
    _XOINL float Cross(const Vector2& v) const;
    _XOINL Vector2 OrthogonalCCW() const;
    _XOINL Vector2 OrthogonalCW() const;
    _XOINL float AngleRadians(const Vector2& v) const;
    _XOINL float AngleDegrees(const Vector2& v) const;
    _XOINL Vector2 Lerp(const Vector2& v, float t) const;

    friend std::ostream& operator <<(std::ostream& os, const Vector2& v) {
        os << "(x:" << v.x << ", y:" << v.y << ", mag:" << v.Magnitude() << ")";
        return os;
    }

    static const Vector2
        UnitX, UnitY,
        Up, Down, Left, Right,
        One, Zero;

    union {
        struct { float x, y; };
        float f[2];
    };
};

const Vector2 Vector2::UnitX(1.0f, 0.0f);
const Vector2 Vector2::UnitY(0.0f, 1.0f);

const Vector2 Vector2::Up(0.0f, 1.0f);
const Vector2 Vector2::Down(0.0f, -1.0f);
const Vector2 Vector2::Left(-1.0f, 0.0f);
const Vector2 Vector2::Right(1.0f, 0.0f);

const Vector2 Vector2::One(1.0f, 1.0f);
const Vector2 Vector2::Zero(0.0f, 0.0f);
XOMATH_END_XO_NS

#endif // XOMATH_INTERNAL