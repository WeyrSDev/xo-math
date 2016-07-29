#ifndef XOMATH_INTERNAL
static_assert(false, "Don't include Vector3.h directly. Include GameMath.h, which fully implements this type.");
#else // XOMATH_INTERNAL

XOMATH_BEGIN_XO_NS
class Vector3 {
    static const unsigned IDX_X = 0;
    static const unsigned IDX_Y = 1;
    static const unsigned IDX_Z = 2;
    static const unsigned IDX_W = 3;
    static const __m128 MASK;
public:
    constexpr static const float Epsilon = FloatEpsilon+FloatEpsilon+FloatEpsilon;

    XOMATH_INLINE Vector3() : m(_mm_setzero_ps()) {
    }
    XOMATH_INLINE XOMATH_FAST(Vector3(float f)) : m(_mm_set1_ps(f)) {
    }
    XOMATH_INLINE XOMATH_FAST(Vector3(float x, float y, float z)) : m(_mm_set_ps(0.0f, z, y, x)) {
    }
    XOMATH_INLINE XOMATH_FAST(Vector3(const Vector3& vec)) : m(vec.m) {
    }
    XOMATH_INLINE XOMATH_FAST(Vector3(const __m128& vec)) : m(vec) {
    }
    //Vector3(const class Vector2& v);

    XOMATH_INLINE float& XOMATH_FAST(operator [](int i)) {
#ifdef XO_SAFE_VEC_RANGE // paranoid about crashes?
        return f[i & 0b11];
#else
        return f[i];
#endif
    }

    XOMATH_INLINE const float& XOMATH_FAST(operator [](int i) const) {
#ifdef XO_SAFE_VEC_RANGE // paranoid about crashes?
        return f[i & 0b11];
#else
        return f[i];
#endif
    }

    XOMATH_INLINE void XOMATH_FAST(Set(float x, float y, float z)) {
        m = _mm_set_ps(0.0f, z, y, x);
    }

    XOMATH_INLINE void XOMATH_FAST(Set(float f)) {
        m = _mm_set1_ps(f);
    }

    XOMATH_INLINE void XOMATH_FAST(Set(const Vector3& vec)) {
        m = vec.m;
    }

    XOMATH_INLINE void XOMATH_FAST(Set(const __m128& vec)) {
        m = vec;
    }

    XOMATH_INLINE void XOMATH_FAST(Get(float& x, float& y, float &z) const) {
        x = this->x;
        y = this->y;
        z = this->z;
    }

    XOMATH_INLINE void XOMATH_FAST(Get(float* f) const) {
        _mm_store_ps(f, m);
    }

    VEC3D_SIMPLE_OP(+, _mm_add_ps)
    VEC3D_SIMPLE_OP(-, _mm_sub_ps)
    VEC3D_SIMPLE_OP(*, _mm_mul_ps)
    VEC3D_SIMPLE_OP(/ , _mm_div_ps)

    VEC3D_SIMPLE_OP_ADD(+=, _mm_add_ps)
    VEC3D_SIMPLE_OP_ADD(-=, _mm_sub_ps)
    VEC3D_SIMPLE_OP_ADD(*=, _mm_mul_ps)
    VEC3D_SIMPLE_OP_ADD(/=, _mm_div_ps)

    XOMATH_INLINE Vector3 operator - () const {
        static const __m128 anti = _mm_set1_ps(-1.0f);
        return Vector3(_mm_mul_ps(m, anti));
    }

    XOMATH_INLINE Vector3 operator ~ () const {
        return Vector3(_mm_shuffle_ps(m, m, _MM_SHUFFLE(IDX_W, IDX_X, IDX_Y, IDX_Z)));
    }

    XOMATH_INLINE Vector3 ZYX() const {
        return Vector3(_mm_shuffle_ps(m, m, _MM_SHUFFLE(IDX_W, IDX_X, IDX_Y, IDX_Z)));
    }

    XOMATH_INLINE float Magnitude() const {
        auto square = _mm_and_ps(_mm_mul_ps(m, m), MASK);
        square = _mm_hadd_ps(square, square);
        square = _mm_hadd_ps(square, square);
        return Sqrt(_mm_cvtss_f32(square));
    }

    XOMATH_INLINE float MagnitudeSquared() const {
        auto square = _mm_and_ps(_mm_mul_ps(m, m), MASK);
        square = _mm_hadd_ps(square, square);
        square = _mm_hadd_ps(square, square);
        return _mm_cvtss_f32(square);
    }

    XOMATH_INLINE void Normalize() {
        float magnitude = MagnitudeSquared();
        if (magnitude - 1.0f <= Epsilon)
            return; // already normalized
        magnitude = Sqrt(magnitude);
        if (magnitude < Epsilon)
            return; // zero vec
        magnitude = 1.0f / magnitude;
        (*this) *= magnitude;
    }

    XOMATH_INLINE Vector3 Normalized() const {
        float magnitude = MagnitudeSquared();
        if (magnitude - 1.0f <= Epsilon)
            return *this; // already normalized
        magnitude = Sqrt(magnitude);
        if (magnitude < Epsilon)
            return *this; // zero vec
        magnitude = 1.0f / magnitude;
        return (*this) * magnitude;
    }

    XOMATH_INLINE bool IsZero() const {
        return MagnitudeSquared() <= Epsilon;
    }

    XOMATH_INLINE bool IsNormalized() const {
      // todo: check closeness
        return MagnitudeSquared() - 1.0f <= Epsilon;
    }

    // Note: comparisons against another vector will compare the magnitude.
    // comparisons against a number will also compare against magnitude (ignoring sign)
    VEC3_COMPARE_OP(<)
    VEC3_COMPARE_OP(<=)
    VEC3_COMPARE_OP(>)
    VEC3_COMPARE_OP(>=)
    // Note: '== and !=' against another Vector3 will check for the exact X,Y,Z match.
    // '== and !=' against a number will check against the magnitude (ignoring sign)
    VEC3_COMPARE_CLOSE_OP(==)
    VEC3_COMPARE_CLOSE_OP(!=)

    XOMATH_INLINE static Vector3 XOMATH_FAST(Max(const Vector3& a, const Vector3& b)) {
        if (a.MagnitudeSquared() >= b.MagnitudeSquared())
            return a;
        return b;
    }

    XOMATH_INLINE static Vector3 XOMATH_FAST(Min(const Vector3& a, const Vector3& b)) {
        if (a.MagnitudeSquared() <= b.MagnitudeSquared())
            return a;
        return b;
    }

    XOMATH_INLINE static float XOMATH_FAST(Dot(const Vector3& a, const Vector3& b)) {
        auto d = _mm_and_ps(_mm_mul_ps(a.m, b.m), MASK);
        d = _mm_hadd_ps(d, d);
        d = _mm_hadd_ps(d, d);
        return _mm_cvtss_f32(d);
    }

    XOMATH_INLINE static Vector3 XOMATH_FAST(Cross(const Vector3& a, const Vector3& b)) {
        auto l = _mm_mul_ps(_mm_shuffle_ps(a.m, a.m, _MM_SHUFFLE(IDX_W, IDX_X, IDX_Z, IDX_Y)), _mm_shuffle_ps(b.m, b.m, _MM_SHUFFLE(IDX_W, IDX_Y, IDX_X, IDX_Z)));
        auto r = _mm_mul_ps(_mm_shuffle_ps(a.m, a.m, _MM_SHUFFLE(IDX_W, IDX_Y, IDX_X, IDX_Z)), _mm_shuffle_ps(b.m, b.m, _MM_SHUFFLE(IDX_W, IDX_X, IDX_Z, IDX_Y)));
        return Vector3(_mm_sub_ps(l, r));
    }

    XOMATH_INLINE static float XOMATH_FAST(AngleRadians(const Vector3& a, const Vector3& b)) {
        auto l = _mm_mul_ps(_mm_shuffle_ps(a.m, a.m, _MM_SHUFFLE(IDX_W, IDX_X, IDX_Z, IDX_Y)), _mm_shuffle_ps(b.m, b.m, _MM_SHUFFLE(IDX_W, IDX_Y, IDX_X, IDX_Z)));
        auto r = _mm_mul_ps(_mm_shuffle_ps(a.m, a.m, _MM_SHUFFLE(IDX_W, IDX_Y, IDX_X, IDX_Z)), _mm_shuffle_ps(b.m, b.m, _MM_SHUFFLE(IDX_W, IDX_X, IDX_Z, IDX_Y)));

        auto s = _mm_sub_ps(l, r);
        s = _mm_and_ps(_mm_mul_ps(s, s), MASK); // exclude the w before any hadd
        s = _mm_hadd_ps(s, s);
        s = _mm_hadd_ps(s, s);
        return Atan2(Sqrt(_mm_cvtss_f32(s)) + Epsilon, Dot(a, b));
    }

    XOMATH_INLINE static float XOMATH_FAST(AngleDegrees(const Vector3& a, const Vector3& b)) {
        return AngleRadians(a, b) * Rad2Deg;
    }

    XOMATH_INLINE static float XOMATH_FAST(DistanceSquared(const Vector3& a, const Vector3& b)) {
        return (a - b).MagnitudeSquared();
    }

    XOMATH_INLINE static float XOMATH_FAST(Distance(const Vector3&a, const Vector3&b)) {
        return (a - b).Magnitude();
    }

    XOMATH_INLINE static Vector3 XOMATH_FAST(Lerp(const Vector3& a, const Vector3& b, float t)) {
        return a + ((b - a) * t);
    }

    XOMATH_INLINE float XOMATH_FAST(Dot(const Vector3& v) const) { return Dot(*this, v); }
    XOMATH_INLINE Vector3 XOMATH_FAST(Cross(const Vector3& v) const) { return Cross(*this, v); }
    XOMATH_INLINE float XOMATH_FAST(AngleRadians(const Vector3& v) const) { return AngleRadians(*this, v); }
    XOMATH_INLINE float XOMATH_FAST(AngleDegrees(const Vector3& v) const) { return AngleDegrees(*this, v); }
    XOMATH_INLINE float XOMATH_FAST(DistanceSquared(const Vector3& v) const) { return DistanceSquared(*this, v); }
    XOMATH_INLINE float XOMATH_FAST(Distance(const Vector3& v) const) { return Distance(*this, v); }

    friend std::ostream& operator <<(std::ostream& os, const Vector3& v) {
        os << "( x:" << v.x << ", y:" << v.y << ", z:" << v.z << ", w:" << v.w << ", mag:" << v.Magnitude() << ")";
        return os;
    }

    static const Vector3
        Origin,
        UnitX, UnitY, UnitZ,
        Up, Down, Left, Right, Forward, Backward,
        One, Zero;

    union {
        struct {
            float x, y, z, w;
        };
        float f[4];
        __m128 m;
    };
};

const __m128 Vector3::MASK = _mm_castsi128_ps(_mm_set_epi32(0, 0xffffffff, 0xffffffff, 0xffffffff));

const Vector3 Vector3::Origin(0.0f, 0.0f, 0.0f);

const Vector3 Vector3::UnitX(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UnitY(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UnitZ(0.0f, 0.0f, 1.0f);

const Vector3 Vector3::Up(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::Down(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::Left(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::Right(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::Forward(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::Backward(0.0f, 0.0f, -1.0f);

const Vector3 Vector3::One(1.0f, 1.0f, 1.0f);
const Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);

XOMATH_END_XO_NS

#endif // XOMATH_INTERNAL