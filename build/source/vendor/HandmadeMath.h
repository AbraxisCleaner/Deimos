/*
  HandmadeMath.h v2.0.0

  This is a single header file with a bunch of useful types and functions for
  games and graphics. Consider it a lightweight alternative to GLM that works
  both C and C++.

  =============================================================================
  CONFIG
  =============================================================================

  By default, all angles in Handmade Math are specified in radians. However, it
  can be configured to use degrees or turns instead. Use one of the following
  defines to specify the default unit for angles:

    #define HANDMADE_MATH_USE_RADIANS
    #define HANDMADE_MATH_USE_DEGREES
    #define HANDMADE_MATH_USE_TURNS

  Regardless of the default angle, you can use the following functions to
  specify an angle in a particular unit:

    HMM_AngleRad(radians)
    HMM_AngleDeg(degrees)
    HMM_AngleTurn(turns)

  The definitions of these functions change depending on the default unit.

  -----------------------------------------------------------------------------

  Handmade Math ships with SSE (SIMD) implementations of several common
  operations. To disable the use of SSE intrinsics, you must define
  HANDMADE_MATH_NO_SSE before including this file:

    #define HANDMADE_MATH_NO_SSE
    #include "HandmadeMath.h"

  -----------------------------------------------------------------------------

  To use Handmade Math without the C runtime library, you must provide your own
  implementations of basic math functions. Otherwise, HandmadeMath.h will use
  the runtime library implementation of these functions.

  Define HANDMADE_MATH_PROVIDE_MATH_FUNCTIONS and provide your own
  implementations of HMM_SINF, HMM_COSF, HMM_TANF, HMM_ACOSF, and HMM_SQRTF
  before including HandmadeMath.h, like so:

    #define HANDMADE_MATH_PROVIDE_MATH_FUNCTIONS
    #define HMM_SINF MySinF
    #define HMM_COSF MyCosF
    #define HMM_TANF MyTanF
    #define HMM_ACOSF MyACosF
    #define HMM_SQRTF MySqrtF
    #include "HandmadeMath.h"
  
  By default, it is assumed that your math functions take radians. To use
  different units, you must define HMM_ANGLE_USER_TO_INTERNAL and
  HMM_ANGLE_INTERNAL_TO_USER. For example, if you want to use degrees in your
  code but your math functions use turns:

    #define HMM_ANGLE_USER_TO_INTERNAL(a) ((a)*HMM_DegToTurn)
    #define HMM_ANGLE_INTERNAL_TO_USER(a) ((a)*HMM_TurnToDeg)

  =============================================================================
  
  LICENSE

  This software is in the public domain. Where that dedication is not
  recognized, you are granted a perpetual, irrevocable license to copy,
  distribute, and modify this file as you see fit.

  =============================================================================

  CREDITS

  Originally written by Zakary Strange.

  Functionality:
   Zakary Strange (strangezak@protonmail.com && @strangezak)
   Matt Mascarenhas (@miblo_)
   Aleph
   FieryDrake (@fierydrake)
   Gingerbill (@TheGingerBill)
   Ben Visness (@bvisness)
   Trinton Bullard (@Peliex_Dev)
   @AntonDan
   Logan Forman (@dev_dwarf)

  Fixes:
   Jeroen van Rijn (@J_vanRijn)
   Kiljacken (@Kiljacken)
   Insofaras (@insofaras)
   Daniel Gibson (@DanielGibson)
*/

#ifndef HANDMADE_MATH_H
#define HANDMADE_MATH_H

// Dummy macros for when test framework is not present.
#ifndef COVERAGE
# define COVERAGE(a, b)
#endif

#ifndef ASSERT_COVERED
# define ASSERT_COVERED(a)
#endif

/* let's figure out if SSE is really available (unless disabled anyway)
   (it isn't on non-x86/x86_64 platforms or even x86 without explicit SSE support)
   => only use "#ifdef HANDMADE_MATH__USE_SSE" to check for SSE support below this block! */
#ifndef HANDMADE_MATH_NO_SSE
# ifdef _MSC_VER /* MSVC supports SSE in amd64 mode or _M_IX86_FP >= 1 (2 means SSE2) */
#  if defined(_M_AMD64) || ( defined(_M_IX86_FP) && _M_IX86_FP >= 1 )
#   define HANDMADE_MATH__USE_SSE 1
#  endif
# else /* not MSVC, probably GCC, clang, icc or something that doesn't support SSE anyway */
#  ifdef __SSE__ /* they #define __SSE__ if it's supported */
#   define HANDMADE_MATH__USE_SSE 1
#  endif /*  __SSE__ */
# endif /* not _MSC_VER */
#endif /* #ifndef HANDMADE_MATH_NO_SSE */

#if (!defined(__cplusplus) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
# define HANDMADE_MATH__USE_C11_GENERICS 1
#endif

#ifdef HANDMADE_MATH__USE_SSE
# include <xmmintrin.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable:4201)
#endif

#if defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wfloat-equal"
# if (defined(__GNUC__) && (__GNUC__ == 4 && __GNUC_MINOR__ < 8)) || defined(__clang__)
#  pragma GCC diagnostic ignored "-Wmissing-braces"
# endif
# ifdef __clang__
#  pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
# endif
#endif

#if defined(__GNUC__) || defined(__clang__)
# define HMM_DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
# define HMM_DEPRECATED(msg) __declspec(deprecated(msg))
#else
# define HMM_DEPRECATED(msg)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(HANDMADE_MATH_USE_DEGREES) \
    && !defined(HANDMADE_MATH_USE_TURNS) \
    && !defined(HANDMADE_MATH_USE_RADIANS)
# define HANDMADE_MATH_USE_RADIANS
#endif
    
#define HMM_PI 3.14159265358979323846
#define HMM_PI32 3.14159265359f
#define HMM_DEG180 180.0
#define HMM_DEG18032 180.0f
#define HMM_TURNHALF 0.5
#define HMM_TURNHALF32 0.5f
#define HMM_RadToDeg ((float)(HMM_DEG180/HMM_PI))
#define HMM_RadToTurn ((float)(HMM_TURNHALF/HMM_PI))
#define HMM_DegToRad ((float)(HMM_PI/HMM_DEG180))
#define HMM_DegToTurn ((float)(HMM_TURNHALF/HMM_DEG180))
#define HMM_TurnToRad ((float)(HMM_PI/HMM_TURNHALF))
#define HMM_TurnToDeg ((float)(HMM_DEG180/HMM_TURNHALF))

#if defined(HANDMADE_MATH_USE_RADIANS)
# define HMM_AngleRad(a) (a)
# define HMM_AngleDeg(a) ((a)*HMM_DegToRad)
# define HMM_AngleTurn(a) ((a)*HMM_TurnToRad)
#elif defined(HANDMADE_MATH_USE_DEGREES)
# define HMM_AngleRad(a) ((a)*HMM_RadToDeg)
# define HMM_AngleDeg(a) (a)
# define HMM_AngleTurn(a) ((a)*HMM_TurnToDeg)
#elif defined(HANDMADE_MATH_USE_TURNS)
# define HMM_AngleRad(a) ((a)*HMM_RadToTurn)
# define HMM_AngleDeg(a) ((a)*HMM_DegToTurn)
# define HMM_AngleTurn(a) (a)
#endif

#if !defined(HANDMADE_MATH_PROVIDE_MATH_FUNCTIONS)
# include <math.h>
# define HMM_SINF sinf
# define HMM_COSF cosf
# define HMM_TANF tanf
# define HMM_SQRTF sqrtf
# define HMM_ACOSF acosf
#endif

#if !defined(HMM_ANGLE_USER_TO_INTERNAL)
# define HMM_ANGLE_USER_TO_INTERNAL(a) (HMM_ToRad(a))
#endif

#if !defined(HMM_ANGLE_INTERNAL_TO_USER)
# if defined(HANDMADE_MATH_USE_RADIANS)
#  define HMM_ANGLE_INTERNAL_TO_USER(a) (a) 
# elif defined(HANDMADE_MATH_USE_DEGREES)
#  define HMM_ANGLE_INTERNAL_TO_USER(a) ((a)*HMM_RadToDeg)
# elif defined(HANDMADE_MATH_USE_TURNS)
#  define HMM_ANGLE_INTERNAL_TO_USER(a) ((a)*HMM_RadToTurn)
# endif
#endif

#define HMM_MIN(a, b) ((a) > (b) ? (b) : (a))
#define HMM_MAX(a, b) ((a) < (b) ? (b) : (a))
#define HMM_ABS(a) ((a) > 0 ? (a) : -(a))
#define HMM_MOD(a, m) (((a) % (m)) >= 0 ? ((a) % (m)) : (((a) % (m)) + (m)))
#define HMM_SQUARE(x) ((x) * (x))

/*
 * Angle unit conversion functions
 */
static inline float HMM_ToRad(float Angle)
{
#if defined(HANDMADE_MATH_USE_RADIANS)
    float Result = Angle;
#elif defined(HANDMADE_MATH_USE_DEGREES) 
    float Result = Angle * HMM_DegToRad;
#elif defined(HANDMADE_MATH_USE_TURNS)
    float Result = Angle * HMM_TurnToRad;
#endif
    
    return Result;
}

static inline float HMM_ToDeg(float Angle)
{
#if defined(HANDMADE_MATH_USE_RADIANS)
    float Result = Angle * HMM_RadToDeg;
#elif defined(HANDMADE_MATH_USE_DEGREES) 
    float Result = Angle;
#elif defined(HANDMADE_MATH_USE_TURNS)
    float Result = Angle * HMM_TurnToDeg;
#endif
    
    return Result;
}

static inline float HMM_ToTurn(float Angle)
{
#if defined(HANDMADE_MATH_USE_RADIANS)
    float Result = Angle * HMM_RadToTurn;
#elif defined(HANDMADE_MATH_USE_DEGREES) 
    float Result = Angle * HMM_DegToTurn;
#elif defined(HANDMADE_MATH_USE_TURNS)
    float Result = Angle;
#endif
    
    return Result;
}

/*
 * Floating-point math functions
 */

COVERAGE(HMM_SinF, 1)
static inline float HMM_SinF(float Angle)
{
    ASSERT_COVERED(HMM_SinF);
    return HMM_SINF(HMM_ANGLE_USER_TO_INTERNAL(Angle));
}

COVERAGE(HMM_CosF, 1)
static inline float HMM_CosF(float Angle)
{
    ASSERT_COVERED(HMM_CosF);
    return HMM_COSF(HMM_ANGLE_USER_TO_INTERNAL(Angle));
}

COVERAGE(HMM_TanF, 1)
static inline float HMM_TanF(float Angle)
{
    ASSERT_COVERED(HMM_TanF);
    return HMM_TANF(HMM_ANGLE_USER_TO_INTERNAL(Angle));
}

COVERAGE(HMM_ACosF, 1)
static inline float HMM_ACosF(float Arg)
{
    ASSERT_COVERED(HMM_ACosF);
    return HMM_ANGLE_INTERNAL_TO_USER(HMM_ACOSF(Arg));
}

COVERAGE(HMM_SqrtF, 1)
static inline float HMM_SqrtF(float Float)
{
    ASSERT_COVERED(HMM_SqrtF);

    float Result;

#ifdef HANDMADE_MATH__USE_SSE
    __m128 In = _mm_set_ss(Float);
    __m128 Out = _mm_sqrt_ss(In);
    Result = _mm_cvtss_f32(Out);
#else
    Result = HMM_SQRTF(Float);
#endif

    return Result;
}

COVERAGE(HMM_InvSqrtF, 1)
static inline float HMM_InvSqrtF(float Float)
{
    ASSERT_COVERED(HMM_InvSqrtF);

    float Result;

    Result = 1.0f/HMM_SqrtF(Float);

    return Result;
}


/*
 * Utility functions
 */

COVERAGE(HMM_Lerp, 1)
static inline float HMM_Lerp(float A, float Time, float B)
{
    ASSERT_COVERED(HMM_Lerp);
    return (1.0f - Time) * A + Time * B;
}

COVERAGE(HMM_Clamp, 1)
static inline float HMM_Clamp(float Min, float Value, float Max)
{
    ASSERT_COVERED(HMM_Clamp);

    float Result = Value;

    if (Result < Min)
    {
        Result = Min;
    }

    if (Result > Max)
    {
        Result = Max;
    }

    return Result;
}

/*
 * SSE stuff
 */

COVERAGE(HMM_LinearCombineV4M4, 1)
static inline Vec4 HMM_LinearCombineV4M4(Vec4 Left, Mat4 Right)
{
	ASSERT_COVERED(HMM_LinearCombineV4M4);

	Vec4 Result;
#ifdef HANDMADE_MATH__USE_SSE
	Result.SSE = _mm_mul_ps(_mm_shuffle_ps(Left.SSE, Left.SSE, 0x00), Right.Columns[0].SSE);
	Result.SSE = _mm_add_ps(Result.SSE, _mm_mul_ps(_mm_shuffle_ps(Left.SSE, Left.SSE, 0x55), Right.Columns[1].SSE));
	Result.SSE = _mm_add_ps(Result.SSE, _mm_mul_ps(_mm_shuffle_ps(Left.SSE, Left.SSE, 0xaa), Right.Columns[2].SSE));
	Result.SSE = _mm_add_ps(Result.SSE, _mm_mul_ps(_mm_shuffle_ps(Left.SSE, Left.SSE, 0xff), Right.Columns[3].SSE));
#else
	Result.x = Left.Elements[0] * Right.Columns[0].x;
	Result.y = Left.Elements[0] * Right.Columns[0].y;
	Result.z = Left.Elements[0] * Right.Columns[0].z;
	Result.w = Left.Elements[0] * Right.Columns[0].w;

	Result.x += Left.Elements[1] * Right.Columns[1].x;
	Result.y += Left.Elements[1] * Right.Columns[1].y;
	Result.z += Left.Elements[1] * Right.Columns[1].z;
	Result.w += Left.Elements[1] * Right.Columns[1].w;

	Result.x += Left.Elements[2] * Right.Columns[2].x;
	Result.y += Left.Elements[2] * Right.Columns[2].y;
	Result.z += Left.Elements[2] * Right.Columns[2].z;
	Result.w += Left.Elements[2] * Right.Columns[2].w;

	Result.x += Left.Elements[3] * Right.Columns[3].x;
	Result.y += Left.Elements[3] * Right.Columns[3].y;
	Result.z += Left.Elements[3] * Right.Columns[3].z;
	Result.w += Left.Elements[3] * Right.Columns[3].w;
#endif

	return Result;
}


union Vec2
{
    Vec2() : Elements{} {}
    Vec2(float x, float y = 0) : Elements{ x, y } {}

    struct { float x, y; };
    float Elements[2];

    inline float Dot(Vec2 &Right) {
        return (x * Right.x) + (y * Right.y);
    }

    inline float Magnitude() {
        return HMM_SqrtF(Dot(*this));
    }

    inline Vec2 Normalized() {
        return *this * HMM_InvSqrtF(this->Dot(*this));
    }

    inline Vec2 Lerp(float Time, Vec2 &B) {
        return ((*this * (1.0f - Time)) + (B * Time));
    }
    
    inline operator float *() { return Elements; }
    inline float &operator[](const int &Index) { return Elements[Index]; }
    inline bool operator >(const Vec2 &other) { if (x > other.x && y > other.y) return true; return false; }
    inline bool operator <(const Vec2 &other) { if (x < other.x && y < other.y) return true; return false; }

	inline operator bool == (Vec2 & Right) { return (x == Right.x && y == Right.y); }
    inline Vec2 operator +(Vec2 &Right) { return Vec2(x + Right.x, y + Right.y); }
    inline Vec2 operator -(Vec2 &Right) { return Vec2(x - Right.x, y - Right.y); }
    inline Vec2 operator *(Vec2 &Right) { return Vec2(x * Right.x, y * Right.y); }
    inline Vec2 operator /(Vec2 &Right) { return Vec2(x / Right.x, y / Right.y); }
    inline Vec2 operator +(float Right) { return Vec2(x + Right, y + Right); }
    inline Vec2 operator -(float Right) { return Vec2(x - Right, y - Right); }
    inline Vec2 operator *(float Right) { return Vec2(x * Right, y * Right); }
    inline Vec2 operator /(float Right) { return Vec2(x / Right, y / Right); }
    inline void operator +=(Vec2 &Right) { x += Right.x; y += Right.y; }
    inline void operator -=(Vec2 &Right) { x -= Right.x; y -= Right.y; }
    inline void operator *=(Vec2 &Right) { x *= Right.x; y *= Right.y; }
    inline void operator /=(Vec2 &Right) { x /= Right.x; y /= Right.y; }
    inline void operator +=(float Right) { x += Right; y += Right; }
    inline void operator -=(float Right) { x -= Right; y -= Right; }
    inline void operator *=(float Right) { x *= Right; y *= Right; }
    inline void operator /=(float Right) { x /= Right; y /= Right; }
};

union Vec3
{
    Vec3() : Elements{} {}
    Vec3(float x, float y = 0, float z = 0) : Elements{ x, y, z } {}

    struct { float x, y, z; };
    float Elements[3];

    inline float Dot(Vec3 &Right) {
        return (x * Right.x) + (y * Right.y) + (z * Right.z);
    }

    inline float Magnitude() {
        return HMM_SqrtF(Dot(*this));
    }

    inline Vec3 Normalized() {
        return *this * HMM_InvSqrtF(this->Dot(*this));
    }

    inline Vec3 Cross(Vec3 &Right) {
        // TODO(dalex): SSE-ify
        Vec3 Result;
        Result.x = (y * Right.z) - (z * Right.y);
        Result.y = (z * Right.x) - (x * Right.z);
        Result.z = (x * Right.y) - (y * Right.x);
        return Result;
	}

	inline Vec3 Lerp(float Time, Vec3 &B) {
		return ((*this * (1.0f - Time)) + (B * Time));
	}

    inline operator float *() { return Elements; }
    inline float &operator[](const int &Index) { return Elements[Index]; }
    inline bool operator >(const Vec3 &other) { if (x > other.x && y > other.y && z > other.z) return true; return false; }
    inline bool operator <(const Vec3 &other) { if (x < other.x && y < other.y && z < other.z) return true; return false; }

	inline operator bool == (Vec3 & Right) { return (x == Right.x && y == Right.y && z == Right.z); }
	inline Vec3 operator +(Vec3 &Right) { return Vec3(x + Right.x, y + Right.y, z + Right.z); }
    inline Vec3 operator -(Vec3 &Right) { return Vec3(x - Right.x, y - Right.y, z - Right.z); }
    inline Vec3 operator *(Vec3 &Right) { return Vec3(x * Right.x, y * Right.y, z * Right.z); }
    inline Vec3 operator /(Vec3 &Right) { return Vec3(x / Right.x, y / Right.y, z / Right.z); }
    inline Vec3 operator +(float Right) { return Vec3(x + Right, y + Right, z + Right); }
    inline Vec3 operator -(float Right) { return Vec3(x - Right, y - Right, z - Right); }
    inline Vec3 operator *(float Right) { return Vec3(x * Right, y * Right, z * Right); }
    inline Vec3 operator /(float Right) { return Vec3(x / Right, y / Right, z / Right); }
	inline void operator +=(Vec3 &Right) { x += Right.x; y += Right.y; z += Right.z; }
    inline void operator -=(Vec3 &Right) { x -= Right.x; y -= Right.y; z -= Right.z; }
	inline void operator *=(Vec3 &Right) { x *= Right.x; y *= Right.y; z *= Right.z; }
	inline void operator /=(Vec3 &Right) { x /= Right.x; y /= Right.y; z /= Right.z; }
	inline void operator +=(float Right) { x += Right; y += Right; z += Right; }
    inline void operator -=(float Right) { x -= Right; y -= Right; z -= Right; }
    inline void operator *=(float Right) { x *= Right; y *= Right; z *= Right; }
    inline void operator /=(float Right) { x /= Right; y /= Right; z /= Right; }
};

union Vec4
{
    Vec4() : Elements{ 0, 0, 0, 0 } {}
    Vec4(float x, float y = 0, float z = 0, float w = 0) : Elements{ x, y, z, w } {}

    struct { float x, y, z, w; };
    struct { float r, g, b, a; }
    float Elements[4];
#ifdef HANDMADE_MATH__USE_SSE
    __m128 SSE;
#endif

    inline float Dot(Vec4 &Right) {
        float Result;

        // NOTE(zak): IN the future if we wanna check what version SSE is support
        // we can use _mm_dp_ps (4.3) but for now we will use the old way.
        // Or a r = _mm_mul_ps(v1, v2), r = _mm_hadd_ps(r, r), r = _mm_hadd_ps(r, r) for SSE3
#ifdef HANDMADE_MATH__USE_SSE
        __m128 SSEResultOne = _mm_mul_ps(SSE, Right.SSE);
        __m128 SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(2, 3, 0, 1));
        SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
        SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(0, 1, 2, 3));
        SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
        _mm_store_ss(&Result, SSEResultOne);
#else
        Result = ((x * Right.x) + (z * Right.z)) + ((y * Right.y) + (w * Right.w));
#endif

        return Result;
    }

    inline float Magnitude() {
        return HMM_SqrtF(Dot(*this));
    }

    inline Vec4 Normalized() {
        return *this * HMM_InvSqrtF(this->Dot(*this));
	}

	inline Vec4 Lerp(float Time, Vec4 &B) {
		return ((*this * (1.0f - Time)) + (B * Time));
	}

    inline operator float *() { return Elements; }
    inline float &operator[](const int &Index) { return Elements[Index]; }

    inline operator bool == (Vec4 & Right) { return (x == Right.x && y == Right.y && z == Right.z && w == Right.w); }
	inline Vec4 operator +(Vec4 &Right) { return Vec4(x + Right.x, y + Right.y, z + Right.z, w + Right.w); }
	inline Vec4 operator -(Vec4 &Right) { return Vec4(x - Right.x, y - Right.y, z - Right.z, w - Right.w); }
	inline Vec4 operator *(Vec4 &Right) { return Vec4(x * Right.x, y * Right.y, z * Right.z, w * Right.w); }
	inline Vec4 operator /(Vec4 &Right) { return Vec4(x / Right.x, y / Right.y, z / Right.z, w / Right.w); }
	inline Vec4 operator +(float Right) { return Vec4(x + Right, y + Right, z + Right, w + Right); }
	inline Vec4 operator -(float Right) { return Vec4(x - Right, y - Right, z - Right, w + Right); }
	inline Vec4 operator *(float Right) { return Vec4(x * Right, y * Right, z * Right, w + Right); }
	inline Vec4 operator /(float Right) { return Vec4(x / Right, y / Right, z / Right, w + Right); }
	inline void operator +=(Vec4 &Right) { x += Right.x; y += Right.y; z += Right.z; w += Right.w; }
	inline void operator -=(Vec4 &Right) { x -= Right.x; y -= Right.y; z -= Right.z; w -= Right.w; }
	inline void operator *=(Vec4 &Right) { x *= Right.x; y *= Right.y; z *= Right.z; w *= Right.w; }
	inline void operator /=(Vec4 &Right) { x /= Right.x; y /= Right.y; z /= Right.z; w /= Right.w; }
	inline void operator +=(float Right) { x += Right; y += Right; z += Right; w += Right; }
	inline void operator -=(float Right) { x -= Right; y -= Right; z -= Right; w -= Right; }
	inline void operator *=(float Right) { x *= Right; y *= Right; z *= Right; w *= Right; }
	inline void operator /=(float Right) { x /= Right; y /= Right; z /= Right; w /= Right; }
};

union Mat2
{
    Mat2() : Columns{ Vec2(1, 0), Vec2(0, 1) } {}

    float Elements[2][2];
    Vec2 Columns[2];

    inline Mat2 Transpose() {
        Mat2 Dst = *this;
        Dst.Elements[0][1] = Elements[1][0];
        Dst.Elements[1][0] = Elements[0][1];
        return Dst;
    }

    inline float Determinant() {
        return Elements[0][0] * Elements[1][1] - Elements[0][1] * Elements[1][0];
    }

    inline Mat2 Inverse() {
        Mat2 Dst;
        float InvDeterminant = 1.0f / this->Determinant();
        Dst.Elements[0][0] = InvDeterminant * +Elements[1][1];
        Dst.Elements[1][1] = InvDeterminant * +Elements[0][0];
        Dst.Elements[0][1] = InvDeterminant * -Elements[0][1];
        Dst.Elements[1][0] = InvDeterminant * -Elements[1][0];
        return Dst;
    }

    inline Vec2 &operator[](const int &Index) { return Columns[Index]; }

    inline Mat2 operator *(Mat2 &Right) {
        Mat2 Dst;
        // 1
        Dst.Elements[0][0] = (Elements[0][0] * Right.Elements[0][0]) + (Elements[0][1] * Right.Elements[1][0]);
        Dst.Elements[0][1] = (Elements[0][0] * Right.Elements[0][1]) + (Elements[0][1] * Right.Elements[1][1]);
        // 2
        Dst.Elements[1][0] = (Elements[1][0] * Right.Elements[0][0]) + (Elements[1][1] * Right.Elements[1][0]);
        Dst.Elements[1][1] = (Elements[1][1] * Right.Elements[0][1]) + (Elements[1][1] * Right.Elements[1][1]);
        return Dst;
    }
};

union Mat3
{
    Mat3() : Columns{ Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1) } {}

    float Elements[3][3];
    Vec3 Columns[3];

    inline Mat3 Transpose() {
		Mat3 Dst = *this;
		Dst.Elements[0][1] = Elements[1][0];
		Dst.Elements[0][2] = Elements[2][0];
		Dst.Elements[1][0] = Elements[0][1];
		Dst.Elements[1][2] = Elements[2][1];
		Dst.Elements[2][1] = Elements[1][2];
		Dst.Elements[2][0] = Elements[0][2];
        return Dst;
    }

	inline float Determinant()
	{
		Mat3 _Cross;
		_Cross.Columns[0] = Columns[1].Cross(Matrix.Columns[2]);
		_Cross.Columns[1] = Columns[2].Cross(Matrix.Columns[0]);
		_Cross.Columns[2] = Columns[0].Cross(Matrix.Columns[1]);
        return _Cross.Columns[2].Dot(Columns[2]);
	}

	inline Mat3 Inverse()
	{
		Mat3 _Cross;
		_Cross.Columns[0] = Matrix.Columns[1].Cross(Matrix.Columns[2]);
		_Cross.Columns[1] = Matrix.Columns[2].Cross(Matrix.Columns[0]);
		_Cross.Columns[2] = Matrix.Columns[0].Cross(Matrix.Columns[1]);

        float InvDeterminant = 1.0f / _Cross.Columns[2].Dot(Columns[2]);

		Mat3 Dst;
		Dst.Columns[0] = Cross.Columns[0] * InvDeterminant;
		Dst.Columns[1] = Cross.Columns[1] * InvDeterminant;
		Dst.Columns[2] = Cross.Columns[2] * InvDeterminant;

        return Dst.Transpose();
	}

    inline Vec3 &operator[](const int &Index) { return Columns[Index]; }

    inline Mat3 operator *(Mat4 &Right) {
        Mat3 Dst;
        // 1
        Dst[0][0] = (Columns[0][0] * Right[0][0]) + (Columns[0][1] * Right[1][0]) + (Columns[0][2] * Right[2][0]);
        Dst[0][1] = (Columns[0][0] * Right[1][0]) + (Columns[0][1] * Right[1][1]) + (Columns[0][2] * Right[2][1]);
        Dst[0][2] = (Columns[0][0] * Right[2][0]) + (Columns[0][1] * Right[1][2]) + (Columns[0][2] * Right[2][2]);
        // 2
        Dst[1][0] = (Columns[1][0] * Right[0][0]) + (Columns[1][1] * Right[1][0]) + (Columns[1][2] * Right[2][0]);
        return Dst;
    }
};

union Mat4
{
    Mat4() : Columns{ Vec4(1, 0, 0), Vec4(0, 1, 0, 0), Vec4(0, 0, 1, 0), Vec4(0, 0, 0, 1) } {}

    float Elements[4][4];
    Vec4 Columns[4];

	inline Mat4 Transpose() {
		Mat4 Dst = *this;
#ifdef HANDMADE_MATH__USE_SSE
		_MM_TRANSPOSE4_PS(Dst.Columns[0].SSE, Dst.Columns[1].SSE, Dst.Columns[2].SSE, Dst.Columns[3].SSE);
#else
		Dst.Elements[0][1] = Elements[1][0];
		Dst.Elements[0][2] = Elements[2][0];
		Dst.Elements[0][3] = Elements[3][0];
		Dst.Elements[1][0] = Elements[0][1];
		Dst.Elements[1][2] = Elements[2][1];
		Dst.Elements[1][3] = Elements[3][1];
		Dst.Elements[2][1] = Elements[1][2];
		Dst.Elements[2][0] = Elements[0][2];
		Dst.Elements[2][3] = Elements[3][2];
		Dst.Elements[3][1] = Elements[1][3];
		Dst.Elements[3][2] = Elements[2][3];
		Dst.Elements[3][0] = Elements[0][3];
#endif
		return Dst;
    }

    inline Mat4 Transpose()
	{
		Mat4 Dst = *this;
#ifdef HANDMADE_MATH__USE_SSE
		_MM_TRANSPOSE4_PS(Dst.Columns[0].SSE, Dst.Columns[1].SSE, Dst.Columns[2].SSE, Dst.Columns[3].SSE);
#else
		Dst.Elements[0][1] = Elements[1][0];
		Dst.Elements[0][2] = Elements[2][0];
		Dst.Elements[0][3] = Elements[3][0];
		Dst.Elements[1][0] = Elements[0][1];
		Dst.Elements[1][2] = Elements[2][1];
		Dst.Elements[1][3] = Elements[3][1];
		Dst.Elements[2][1] = Elements[1][2];
		Dst.Elements[2][0] = Elements[0][2];
		Dst.Elements[2][3] = Elements[3][2];
		Dst.Elements[3][1] = Elements[1][3];
		Dst.Elements[3][2] = Elements[2][3];
		Dst.Elements[3][0] = Elements[0][3];
#endif
		return Dst;
	}

    // TODO: Inverse().

    inline Vec4 &operator[](const int &Index) { return Columns[Index]; }
};

union Quat
{
    Quat() : Elements{} {}
    Quat(float X, float Y, float Z, float W) : Elements{ X, Y, Z, W } {}

    struct { float x, y, z, w; };
    float Elements[4];
#ifdef HANDMADE_MATH__USE_SSE
    __m128 SSE;
#endif
};

typedef signed int HMM_Bool;

/*
 * 2x2 Matrices
 */

COVERAGE(HMM_AddM2, 1)
static inline Mat2 HMM_AddM2(Mat2 Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_AddM2);
    
    Mat2 Result;

    Result.Elements[0][0] = Left.Elements[0][0] + Right.Elements[0][0];
    Result.Elements[0][1] = Left.Elements[0][1] + Right.Elements[0][1];
    Result.Elements[1][0] = Left.Elements[1][0] + Right.Elements[1][0];
    Result.Elements[1][1] = Left.Elements[1][1] + Right.Elements[1][1];
   
    return Result;    
}

COVERAGE(HMM_SubM2, 1)
static inline Mat2 HMM_SubM2(Mat2 Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_SubM2);
    
    Mat2 Result;

    Result.Elements[0][0] = Left.Elements[0][0] - Right.Elements[0][0];
    Result.Elements[0][1] = Left.Elements[0][1] - Right.Elements[0][1];
    Result.Elements[1][0] = Left.Elements[1][0] - Right.Elements[1][0];
    Result.Elements[1][1] = Left.Elements[1][1] - Right.Elements[1][1];
    
    return Result;
}

COVERAGE(HMM_MulM2V2, 1)
static inline Vec2 HMM_MulM2V2(Mat2 Matrix, Vec2 Vector)
{
    ASSERT_COVERED(HMM_MulM2V2);
    
    Vec2 Result;

    Result.x = Vector.Elements[0] * Matrix.Columns[0].x;
    Result.y = Vector.Elements[0] * Matrix.Columns[0].y;

    Result.x += Vector.Elements[1] * Matrix.Columns[1].x;
    Result.y += Vector.Elements[1] * Matrix.Columns[1].y;

    return Result;    
}

COVERAGE(HMM_MulM2, 1)
static inline Mat2 HMM_MulM2(Mat2 Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_MulM2);
    
    Mat2 Result;
    Result.Columns[0] = HMM_MulM2V2(Left, Right.Columns[0]);
    Result.Columns[1] = HMM_MulM2V2(Left, Right.Columns[1]);

    return Result;    
}

COVERAGE(HMM_MulM2F, 1)
static inline Mat2 HMM_MulM2F(Mat2 Matrix, float Scalar)
{
    ASSERT_COVERED(HMM_MulM2F);
    
    Mat2 Result;

    Result.Elements[0][0] = Matrix.Elements[0][0] * Scalar;
    Result.Elements[0][1] = Matrix.Elements[0][1] * Scalar;
    Result.Elements[1][0] = Matrix.Elements[1][0] * Scalar;
    Result.Elements[1][1] = Matrix.Elements[1][1] * Scalar;
    
    return Result;
}

COVERAGE(HMM_DivM2F, 1)
static inline Mat2 HMM_DivM2F(Mat2 Matrix, float Scalar)
{
    ASSERT_COVERED(HMM_DivM2F);
    
    Mat2 Result;

    Result.Elements[0][0] = Matrix.Elements[0][0] / Scalar;
    Result.Elements[0][1] = Matrix.Elements[0][1] / Scalar;
    Result.Elements[1][0] = Matrix.Elements[1][0] / Scalar;
    Result.Elements[1][1] = Matrix.Elements[1][1] / Scalar;

    return Result;
}

/*
 * 3x3 Matrices
 */

COVERAGE(HMM_AddM3, 1)
static inline Mat3 HMM_AddM3(Mat3 Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_AddM3);
    
    Mat3 Result;
    
    Result.Elements[0][0] = Left.Elements[0][0] + Right.Elements[0][0];
    Result.Elements[0][1] = Left.Elements[0][1] + Right.Elements[0][1];
    Result.Elements[0][2] = Left.Elements[0][2] + Right.Elements[0][2];
    Result.Elements[1][0] = Left.Elements[1][0] + Right.Elements[1][0];
    Result.Elements[1][1] = Left.Elements[1][1] + Right.Elements[1][1];
    Result.Elements[1][2] = Left.Elements[1][2] + Right.Elements[1][2];
    Result.Elements[2][0] = Left.Elements[2][0] + Right.Elements[2][0];
    Result.Elements[2][1] = Left.Elements[2][1] + Right.Elements[2][1];
    Result.Elements[2][2] = Left.Elements[2][2] + Right.Elements[2][2];

    return Result;    
}

COVERAGE(HMM_SubM3, 1)
static inline Mat3 HMM_SubM3(Mat3 Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_SubM3);

    Mat3 Result;

    Result.Elements[0][0] = Left.Elements[0][0] - Right.Elements[0][0];
    Result.Elements[0][1] = Left.Elements[0][1] - Right.Elements[0][1];
    Result.Elements[0][2] = Left.Elements[0][2] - Right.Elements[0][2];
    Result.Elements[1][0] = Left.Elements[1][0] - Right.Elements[1][0];
    Result.Elements[1][1] = Left.Elements[1][1] - Right.Elements[1][1];
    Result.Elements[1][2] = Left.Elements[1][2] - Right.Elements[1][2];
    Result.Elements[2][0] = Left.Elements[2][0] - Right.Elements[2][0];
    Result.Elements[2][1] = Left.Elements[2][1] - Right.Elements[2][1];
    Result.Elements[2][2] = Left.Elements[2][2] - Right.Elements[2][2];

    return Result;
}

COVERAGE(HMM_MulM3V3, 1)
static inline Vec3 HMM_MulM3V3(Mat3 Matrix, Vec3 Vector)
{
    ASSERT_COVERED(HMM_MulM3V3);
    
    Vec3 Result;

    Result.x = Vector.Elements[0] * Matrix.Columns[0].x;
    Result.y = Vector.Elements[0] * Matrix.Columns[0].y;
    Result.z = Vector.Elements[0] * Matrix.Columns[0].z;

    Result.x += Vector.Elements[1] * Matrix.Columns[1].x;
    Result.y += Vector.Elements[1] * Matrix.Columns[1].y;
    Result.z += Vector.Elements[1] * Matrix.Columns[1].z;

    Result.x += Vector.Elements[2] * Matrix.Columns[2].x;
    Result.y += Vector.Elements[2] * Matrix.Columns[2].y;
    Result.z += Vector.Elements[2] * Matrix.Columns[2].z;
    
    return Result;    
}

COVERAGE(HMM_MulM3, 1)
static inline Mat3 HMM_MulM3(Mat3 Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_MulM3);

    Mat3 Result;
    Result.Columns[0] = HMM_MulM3V3(Left, Right.Columns[0]);
    Result.Columns[1] = HMM_MulM3V3(Left, Right.Columns[1]);
    Result.Columns[2] = HMM_MulM3V3(Left, Right.Columns[2]);

    return Result;    
}

COVERAGE(HMM_MulM3F, 1)
static inline Mat3 HMM_MulM3F(Mat3 Matrix, float Scalar)
{
    ASSERT_COVERED(HMM_MulM3F);

    Mat3 Result;

    Result.Elements[0][0] = Matrix.Elements[0][0] * Scalar;
    Result.Elements[0][1] = Matrix.Elements[0][1] * Scalar;
    Result.Elements[0][2] = Matrix.Elements[0][2] * Scalar;
    Result.Elements[1][0] = Matrix.Elements[1][0] * Scalar;
    Result.Elements[1][1] = Matrix.Elements[1][1] * Scalar;
    Result.Elements[1][2] = Matrix.Elements[1][2] * Scalar;
    Result.Elements[2][0] = Matrix.Elements[2][0] * Scalar;
    Result.Elements[2][1] = Matrix.Elements[2][1] * Scalar;
    Result.Elements[2][2] = Matrix.Elements[2][2] * Scalar;

    return Result;            
}

COVERAGE(HMM_DivM3, 1)
static inline Mat3 HMM_DivM3F(Mat3 Matrix, float Scalar)
{
    ASSERT_COVERED(HMM_DivM3);

    Mat3 Result;
    
    Result.Elements[0][0] = Matrix.Elements[0][0] / Scalar;
    Result.Elements[0][1] = Matrix.Elements[0][1] / Scalar;
    Result.Elements[0][2] = Matrix.Elements[0][2] / Scalar;
    Result.Elements[1][0] = Matrix.Elements[1][0] / Scalar;
    Result.Elements[1][1] = Matrix.Elements[1][1] / Scalar;
    Result.Elements[1][2] = Matrix.Elements[1][2] / Scalar;
    Result.Elements[2][0] = Matrix.Elements[2][0] / Scalar;
    Result.Elements[2][1] = Matrix.Elements[2][1] / Scalar;
    Result.Elements[2][2] = Matrix.Elements[2][2] / Scalar;

    return Result;                    
}

/*
 * 4x4 Matrices
 */

COVERAGE(HMM_AddM4, 1)
static inline Mat4 HMM_AddM4(Mat4 Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_AddM4);

    Mat4 Result;

#ifdef HANDMADE_MATH__USE_SSE
    Result.Columns[0].SSE = _mm_add_ps(Left.Columns[0].SSE, Right.Columns[0].SSE);
    Result.Columns[1].SSE = _mm_add_ps(Left.Columns[1].SSE, Right.Columns[1].SSE);
    Result.Columns[2].SSE = _mm_add_ps(Left.Columns[2].SSE, Right.Columns[2].SSE);
    Result.Columns[3].SSE = _mm_add_ps(Left.Columns[3].SSE, Right.Columns[3].SSE);
#else
    Result.Elements[0][0] = Left.Elements[0][0] + Right.Elements[0][0];
    Result.Elements[0][1] = Left.Elements[0][1] + Right.Elements[0][1];
    Result.Elements[0][2] = Left.Elements[0][2] + Right.Elements[0][2];
    Result.Elements[0][3] = Left.Elements[0][3] + Right.Elements[0][3];
    Result.Elements[1][0] = Left.Elements[1][0] + Right.Elements[1][0];
    Result.Elements[1][1] = Left.Elements[1][1] + Right.Elements[1][1];
    Result.Elements[1][2] = Left.Elements[1][2] + Right.Elements[1][2];
    Result.Elements[1][3] = Left.Elements[1][3] + Right.Elements[1][3];
    Result.Elements[2][0] = Left.Elements[2][0] + Right.Elements[2][0];
    Result.Elements[2][1] = Left.Elements[2][1] + Right.Elements[2][1];
    Result.Elements[2][2] = Left.Elements[2][2] + Right.Elements[2][2];
    Result.Elements[2][3] = Left.Elements[2][3] + Right.Elements[2][3];
    Result.Elements[3][0] = Left.Elements[3][0] + Right.Elements[3][0];
    Result.Elements[3][1] = Left.Elements[3][1] + Right.Elements[3][1];
    Result.Elements[3][2] = Left.Elements[3][2] + Right.Elements[3][2];
    Result.Elements[3][3] = Left.Elements[3][3] + Right.Elements[3][3];
#endif

    return Result;
}

COVERAGE(HMM_SubM4, 1)
static inline Mat4 HMM_SubM4(Mat4 Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_SubM4);

    Mat4 Result;

#ifdef HANDMADE_MATH__USE_SSE
    Result.Columns[0].SSE = _mm_sub_ps(Left.Columns[0].SSE, Right.Columns[0].SSE);
    Result.Columns[1].SSE = _mm_sub_ps(Left.Columns[1].SSE, Right.Columns[1].SSE);
    Result.Columns[2].SSE = _mm_sub_ps(Left.Columns[2].SSE, Right.Columns[2].SSE);
    Result.Columns[3].SSE = _mm_sub_ps(Left.Columns[3].SSE, Right.Columns[3].SSE);
#else
    Result.Elements[0][0] = Left.Elements[0][0] - Right.Elements[0][0];
    Result.Elements[0][1] = Left.Elements[0][1] - Right.Elements[0][1];
    Result.Elements[0][2] = Left.Elements[0][2] - Right.Elements[0][2];
    Result.Elements[0][3] = Left.Elements[0][3] - Right.Elements[0][3];
    Result.Elements[1][0] = Left.Elements[1][0] - Right.Elements[1][0];
    Result.Elements[1][1] = Left.Elements[1][1] - Right.Elements[1][1];
    Result.Elements[1][2] = Left.Elements[1][2] - Right.Elements[1][2];
    Result.Elements[1][3] = Left.Elements[1][3] - Right.Elements[1][3];
    Result.Elements[2][0] = Left.Elements[2][0] - Right.Elements[2][0];
    Result.Elements[2][1] = Left.Elements[2][1] - Right.Elements[2][1];
    Result.Elements[2][2] = Left.Elements[2][2] - Right.Elements[2][2];
    Result.Elements[2][3] = Left.Elements[2][3] - Right.Elements[2][3];
    Result.Elements[3][0] = Left.Elements[3][0] - Right.Elements[3][0];
    Result.Elements[3][1] = Left.Elements[3][1] - Right.Elements[3][1];
    Result.Elements[3][2] = Left.Elements[3][2] - Right.Elements[3][2];
    Result.Elements[3][3] = Left.Elements[3][3] - Right.Elements[3][3];
#endif
 
    return Result;
}

COVERAGE(HMM_MulM4, 1)
static inline Mat4 HMM_MulM4(Mat4 Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_MulM4);

    Mat4 Result;
    Result.Columns[0] = HMM_LinearCombineV4M4(Right.Columns[0], Left);
    Result.Columns[1] = HMM_LinearCombineV4M4(Right.Columns[1], Left);
    Result.Columns[2] = HMM_LinearCombineV4M4(Right.Columns[2], Left);
    Result.Columns[3] = HMM_LinearCombineV4M4(Right.Columns[3], Left);

    return Result;
}

COVERAGE(HMM_MulM4F, 1)
static inline Mat4 HMM_MulM4F(Mat4 Matrix, float Scalar)
{
    ASSERT_COVERED(HMM_MulM4F);

    Mat4 Result;

#ifdef HANDMADE_MATH__USE_SSE
    __m128 SSEScalar = _mm_set1_ps(Scalar);
    Result.Columns[0].SSE = _mm_mul_ps(Matrix.Columns[0].SSE, SSEScalar);
    Result.Columns[1].SSE = _mm_mul_ps(Matrix.Columns[1].SSE, SSEScalar);
    Result.Columns[2].SSE = _mm_mul_ps(Matrix.Columns[2].SSE, SSEScalar);
    Result.Columns[3].SSE = _mm_mul_ps(Matrix.Columns[3].SSE, SSEScalar);
#else
    Result.Elements[0][0] = Matrix.Elements[0][0] * Scalar;
    Result.Elements[0][1] = Matrix.Elements[0][1] * Scalar;
    Result.Elements[0][2] = Matrix.Elements[0][2] * Scalar;
    Result.Elements[0][3] = Matrix.Elements[0][3] * Scalar;
    Result.Elements[1][0] = Matrix.Elements[1][0] * Scalar;
    Result.Elements[1][1] = Matrix.Elements[1][1] * Scalar;
    Result.Elements[1][2] = Matrix.Elements[1][2] * Scalar;
    Result.Elements[1][3] = Matrix.Elements[1][3] * Scalar;
    Result.Elements[2][0] = Matrix.Elements[2][0] * Scalar;
    Result.Elements[2][1] = Matrix.Elements[2][1] * Scalar;
    Result.Elements[2][2] = Matrix.Elements[2][2] * Scalar;
    Result.Elements[2][3] = Matrix.Elements[2][3] * Scalar;
    Result.Elements[3][0] = Matrix.Elements[3][0] * Scalar;
    Result.Elements[3][1] = Matrix.Elements[3][1] * Scalar;
    Result.Elements[3][2] = Matrix.Elements[3][2] * Scalar;
    Result.Elements[3][3] = Matrix.Elements[3][3] * Scalar;
#endif

    return Result;
}

COVERAGE(HMM_MulM4V4, 1)
static inline Vec4 HMM_MulM4V4(Mat4 Matrix, Vec4 Vector)
{
    ASSERT_COVERED(HMM_MulM4V4);
    return HMM_LinearCombineV4M4(Vector, Matrix);
}

COVERAGE(HMM_DivM4F, 1)
static inline Mat4 HMM_DivM4F(Mat4 Matrix, float Scalar)
{
    ASSERT_COVERED(HMM_DivM4F);

    Mat4 Result;

#ifdef HANDMADE_MATH__USE_SSE
    __m128 SSEScalar = _mm_set1_ps(Scalar);
    Result.Columns[0].SSE = _mm_div_ps(Matrix.Columns[0].SSE, SSEScalar);
    Result.Columns[1].SSE = _mm_div_ps(Matrix.Columns[1].SSE, SSEScalar);
    Result.Columns[2].SSE = _mm_div_ps(Matrix.Columns[2].SSE, SSEScalar);
    Result.Columns[3].SSE = _mm_div_ps(Matrix.Columns[3].SSE, SSEScalar);
#else
    Result.Elements[0][0] = Matrix.Elements[0][0] / Scalar;
    Result.Elements[0][1] = Matrix.Elements[0][1] / Scalar;
    Result.Elements[0][2] = Matrix.Elements[0][2] / Scalar;
    Result.Elements[0][3] = Matrix.Elements[0][3] / Scalar;
    Result.Elements[1][0] = Matrix.Elements[1][0] / Scalar;
    Result.Elements[1][1] = Matrix.Elements[1][1] / Scalar;
    Result.Elements[1][2] = Matrix.Elements[1][2] / Scalar;
    Result.Elements[1][3] = Matrix.Elements[1][3] / Scalar;
    Result.Elements[2][0] = Matrix.Elements[2][0] / Scalar;
    Result.Elements[2][1] = Matrix.Elements[2][1] / Scalar;
    Result.Elements[2][2] = Matrix.Elements[2][2] / Scalar;
    Result.Elements[2][3] = Matrix.Elements[2][3] / Scalar;
    Result.Elements[3][0] = Matrix.Elements[3][0] / Scalar;
    Result.Elements[3][1] = Matrix.Elements[3][1] / Scalar;
    Result.Elements[3][2] = Matrix.Elements[3][2] / Scalar;
    Result.Elements[3][3] = Matrix.Elements[3][3] / Scalar;
#endif

    return Result;
}

/*
 * Common graphics transformations
 */

COVERAGE(HMM_Orthographic_RH_NO, 1)
// Produces a right-handed orthographic projection get_matrix with Z ranging from -1 to 1 (the GL convention).
// Left, Right, Bottom, and Top specify the coordinates of their respective clipping planes.
// Near and Far specify the distances to the near and far clipping planes.
static inline Mat4 HMM_Orthographic_RH_NO(float Left, float Right, float Bottom, float Top, float Near, float Far)
{
    ASSERT_COVERED(HMM_Orthographic_RH_NO);

    Mat4 Result;

    Result.Elements[0][0] = 2.0f / (Right - Left);
    Result.Elements[1][1] = 2.0f / (Top - Bottom);
    Result.Elements[2][2] = 2.0f / (Near - Far);
    Result.Elements[3][3] = 1.0f;

    Result.Elements[3][0] = (Left + Right) / (Left - Right);
    Result.Elements[3][1] = (Bottom + Top) / (Bottom - Top);
    Result.Elements[3][2] = (Near + Far) / (Near - Far);

    return Result;
}

COVERAGE(HMM_Orthographic_RH_ZO, 1)
// Produces a right-handed orthographic projection get_matrix with Z ranging from 0 to 1 (the DirectX convention).
// Left, Right, Bottom, and Top specify the coordinates of their respective clipping planes.
// Near and Far specify the distances to the near and far clipping planes.
static inline Mat4 HMM_Orthographic_RH_ZO(float Left, float Right, float Bottom, float Top, float Near, float Far)
{
    ASSERT_COVERED(HMM_Orthographic_RH_ZO);

    Mat4 Result;

    Result.Elements[0][0] = 2.0f / (Right - Left);
    Result.Elements[1][1] = 2.0f / (Top - Bottom);
    Result.Elements[2][2] = 1.0f / (Near - Far);
    Result.Elements[3][3] = 1.0f;

    Result.Elements[3][0] = (Left + Right) / (Left - Right);
    Result.Elements[3][1] = (Bottom + Top) / (Bottom - Top);
    Result.Elements[3][2] = (Near) / (Near - Far);

    return Result;
}

COVERAGE(HMM_Orthographic_LH_NO, 1)
// Produces a left-handed orthographic projection get_matrix with Z ranging from -1 to 1 (the GL convention).
// Left, Right, Bottom, and Top specify the coordinates of their respective clipping planes.
// Near and Far specify the distances to the near and far clipping planes.
static inline Mat4 HMM_Orthographic_LH_NO(float Left, float Right, float Bottom, float Top, float Near, float Far)
{
    ASSERT_COVERED(HMM_Orthographic_LH_NO);

    Mat4 Result = HMM_Orthographic_RH_NO(Left, Right, Bottom, Top, Near, Far);
    Result.Elements[2][2] = -Result.Elements[2][2];
    
    return Result;
}

COVERAGE(HMM_Orthographic_LH_ZO, 1)
// Produces a left-handed orthographic projection get_matrix with Z ranging from 0 to 1 (the DirectX convention).
// Left, Right, Bottom, and Top specify the coordinates of their respective clipping planes.
// Near and Far specify the distances to the near and far clipping planes.
static inline Mat4 HMM_Orthographic_LH_ZO(float Left, float Right, float Bottom, float Top, float Near, float Far)
{
    ASSERT_COVERED(HMM_Orthographic_LH_ZO);

    Mat4 Result = HMM_Orthographic_RH_ZO(Left, Right, Bottom, Top, Near, Far);
    Result.Elements[2][2] = -Result.Elements[2][2];
    
    return Result;
}

COVERAGE(HMM_InvOrthographic, 1)
// Returns an inverse for the given orthographic projection get_matrix. Works for all orthographic
// projection matrices, regardless of handedness or NDC convention.
static inline Mat4 HMM_InvOrthographic(Mat4 OrthoMatrix)
{
    ASSERT_COVERED(HMM_InvOrthographic);

    Mat4 Result;
    Result.Elements[0][0] = 1.0f / OrthoMatrix.Elements[0][0];
    Result.Elements[1][1] = 1.0f / OrthoMatrix.Elements[1][1];
    Result.Elements[2][2] = 1.0f / OrthoMatrix.Elements[2][2];
    Result.Elements[3][3] = 1.0f;
    
    Result.Elements[3][0] = -OrthoMatrix.Elements[3][0] * Result.Elements[0][0];
    Result.Elements[3][1] = -OrthoMatrix.Elements[3][1] * Result.Elements[1][1];
    Result.Elements[3][2] = -OrthoMatrix.Elements[3][2] * Result.Elements[2][2];

    return Result;
}

COVERAGE(HMM_Perspective_RH_NO, 1)
static inline Mat4 HMM_Perspective_RH_NO(float FOV, float AspectRatio, float Near, float Far)
{
    ASSERT_COVERED(HMM_Perspective_RH_NO);

    Mat4 Result;

    // See https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml

    float Cotangent = 1.0f / HMM_TanF(FOV / 2.0f);
    Result.Elements[0][0] = Cotangent / AspectRatio;
    Result.Elements[1][1] = Cotangent;
    Result.Elements[2][3] = -1.0f;

    Result.Elements[2][2] = (Near + Far) / (Near - Far);
    Result.Elements[3][2] = (2.0f * Near * Far) / (Near - Far);
    
    return Result;
}

COVERAGE(HMM_Perspective_RH_ZO, 1)
static inline Mat4 HMM_Perspective_RH_ZO(float FOV, float AspectRatio, float Near, float Far)
{
    ASSERT_COVERED(HMM_Perspective_RH_ZO);

    Mat4 Result;

    // See https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml

    float Cotangent = 1.0f / HMM_TanF(FOV / 2.0f);
    Result.Elements[0][0] = Cotangent / AspectRatio;
    Result.Elements[1][1] = Cotangent;
    Result.Elements[2][3] = -1.0f;

    Result.Elements[2][2] = (Far) / (Near - Far);
    Result.Elements[3][2] = (Near * Far) / (Near - Far);

    return Result;
}

COVERAGE(HMM_Perspective_LH_NO, 1)
static inline Mat4 HMM_Perspective_LH_NO(float FOV, float AspectRatio, float Near, float Far)
{ 
    ASSERT_COVERED(HMM_Perspective_LH_NO);

    Mat4 Result = HMM_Perspective_RH_NO(FOV, AspectRatio, Near, Far);
    Result.Elements[2][2] = -Result.Elements[2][2];
    Result.Elements[2][3] = -Result.Elements[2][3];
    
    return Result;
}

COVERAGE(HMM_Perspective_LH_ZO, 1)
static inline Mat4 HMM_Perspective_LH_ZO(float FOV, float AspectRatio, float Near, float Far)
{ 
    ASSERT_COVERED(HMM_Perspective_LH_ZO);

    Mat4 Result = HMM_Perspective_RH_ZO(FOV, AspectRatio, Near, Far);
    Result.Elements[2][2] = -Result.Elements[2][2];
    Result.Elements[2][3] = -Result.Elements[2][3];
    
    return Result;
}

COVERAGE(HMM_InvPerspective_RH, 1)
static inline Mat4 HMM_InvPerspective_RH(Mat4 PerspectiveMatrix)
{
    ASSERT_COVERED(HMM_InvPerspective_RH);

    Mat4 Result;
    Result.Elements[0][0] = 1.0f / PerspectiveMatrix.Elements[0][0];
    Result.Elements[1][1] = 1.0f / PerspectiveMatrix.Elements[1][1];
    Result.Elements[2][2] = 0.0f;

    Result.Elements[2][3] = 1.0f / PerspectiveMatrix.Elements[3][2];
    Result.Elements[3][3] = PerspectiveMatrix.Elements[2][2] * Result.Elements[2][3];
    Result.Elements[3][2] = PerspectiveMatrix.Elements[2][3];

    return Result;
}

COVERAGE(HMM_InvPerspective_LH, 1)
static inline Mat4 HMM_InvPerspective_LH(Mat4 PerspectiveMatrix)
{
    ASSERT_COVERED(HMM_InvPerspective_LH);

    Mat4 Result;
    Result.Elements[0][0] = 1.0f / PerspectiveMatrix.Elements[0][0];
    Result.Elements[1][1] = 1.0f / PerspectiveMatrix.Elements[1][1];
    Result.Elements[2][2] = 0.0f;

    Result.Elements[2][3] = 1.0f / PerspectiveMatrix.Elements[3][2];
    Result.Elements[3][3] = PerspectiveMatrix.Elements[2][2] * -Result.Elements[2][3];
    Result.Elements[3][2] = PerspectiveMatrix.Elements[2][3];

    return Result;
}

COVERAGE(HMM_Translate, 1)
static inline Mat4 HMM_Translate(Vec3 Translation)
{
    ASSERT_COVERED(HMM_Translate);

    Mat4 Result;
    Result.Elements[3][0] = Translation.x;
    Result.Elements[3][1] = Translation.y;
    Result.Elements[3][2] = Translation.z;

    return Result;
}

COVERAGE(HMM_InvTranslate, 1)
static inline Mat4 HMM_InvTranslate(Mat4 TranslationMatrix)
{
    ASSERT_COVERED(HMM_InvTranslate);

    Mat4 Result = TranslationMatrix;
    Result.Elements[3][0] = -Result.Elements[3][0];
    Result.Elements[3][1] = -Result.Elements[3][1];
    Result.Elements[3][2] = -Result.Elements[3][2];

    return Result;
}

COVERAGE(HMM_Rotate_RH, 1)
static inline Mat4 HMM_Rotate_RH(float Angle, Vec3 Axis)
{
    ASSERT_COVERED(HMM_Rotate_RH);

    Mat4 Result;

    Axis = HMM_NormV3(Axis);

    float SinTheta = HMM_SinF(Angle);
    float CosTheta = HMM_CosF(Angle);
    float CosValue = 1.0f - CosTheta;

    Result.Elements[0][0] = (Axis.x * Axis.x * CosValue) + CosTheta;
    Result.Elements[0][1] = (Axis.x * Axis.y * CosValue) + (Axis.z * SinTheta);
    Result.Elements[0][2] = (Axis.x * Axis.z * CosValue) - (Axis.y * SinTheta);

    Result.Elements[1][0] = (Axis.y * Axis.x * CosValue) - (Axis.z * SinTheta);
    Result.Elements[1][1] = (Axis.y * Axis.y * CosValue) + CosTheta;
    Result.Elements[1][2] = (Axis.y * Axis.z * CosValue) + (Axis.x * SinTheta);

    Result.Elements[2][0] = (Axis.z * Axis.x * CosValue) + (Axis.y * SinTheta);
    Result.Elements[2][1] = (Axis.z * Axis.y * CosValue) - (Axis.x * SinTheta);
    Result.Elements[2][2] = (Axis.z * Axis.z * CosValue) + CosTheta;

    return Result;
}

COVERAGE(HMM_Rotate_LH, 1)
static inline Mat4 HMM_Rotate_LH(float Angle, Vec3 Axis)
{
    ASSERT_COVERED(HMM_Rotate_LH);
    /* NOTE(lcf): Matrix will be inverse/transpose of RH. */
    return HMM_Rotate_RH(-Angle, Axis);
}

COVERAGE(HMM_RotateV3_LH, 1)
static inline Mat4 HMM_RotateV3_LH(Vec3 Axes)
{
    ASSERT_COVERED(HMM_RotateV3_LH);
    return HMM_MulM4(HMM_Rotate_LH(Axes[0], HMM_V3(1, 0, 0)), HMM_MulM4(HMM_Rotate_LH(Axes[1], HMM_V3(0, 1, 0)), HMM_Rotate_LH(Axes[2], HMM_V3(0, 0, 1))));
}

COVERAGE(HMM_InvRotate, 1)
static inline Mat4 HMM_InvRotate(Mat4 RotationMatrix)
{
    ASSERT_COVERED(HMM_InvRotate);
    return HMM_TransposeM4(RotationMatrix);
}

COVERAGE(HMM_Scale, 1)
static inline Mat4 HMM_Scale(Vec3 Scale)
{
    ASSERT_COVERED(HMM_Scale);

    Mat4 Result;
    Result.Elements[0][0] = Scale.x;
    Result.Elements[1][1] = Scale.y;
    Result.Elements[2][2] = Scale.z;

    return Result;
}

COVERAGE(HMM_InvScale, 1)
static inline Mat4 HMM_InvScale(Mat4 ScaleMatrix) 
{
    ASSERT_COVERED(HMM_InvScale);

    Mat4 Result = ScaleMatrix;
    Result.Elements[0][0] = 1.0f / Result.Elements[0][0];
    Result.Elements[1][1] = 1.0f / Result.Elements[1][1];
    Result.Elements[2][2] = 1.0f / Result.Elements[2][2];

    return Result;
}

static inline Mat4 _HMM_LookAt(Vec3 F,  Vec3 S, Vec3 U,  Vec3 Eye)
{
    Mat4 Result;

    Result.Elements[0][0] = S.x;
    Result.Elements[0][1] = U.x;
    Result.Elements[0][2] = -F.x;
    Result.Elements[0][3] = 0.0f;

    Result.Elements[1][0] = S.y;
    Result.Elements[1][1] = U.y;
    Result.Elements[1][2] = -F.y;
    Result.Elements[1][3] = 0.0f;

    Result.Elements[2][0] = S.z;
    Result.Elements[2][1] = U.z;
    Result.Elements[2][2] = -F.z;
    Result.Elements[2][3] = 0.0f;

    Result.Elements[3][0] = -HMM_DotV3(S, Eye);
    Result.Elements[3][1] = -HMM_DotV3(U, Eye);
    Result.Elements[3][2] = HMM_DotV3(F, Eye);
    Result.Elements[3][3] = 1.0f;

    return Result;
}

COVERAGE(HMM_LookAt_RH, 1)
static inline Mat4 HMM_LookAt_RH(Vec3 Eye, Vec3 Center, Vec3 Up)
{
    ASSERT_COVERED(HMM_LookAt_RH);

    Vec3 F = HMM_NormV3(HMM_SubV3(Center, Eye));
    Vec3 S = HMM_NormV3(HMM_Cross(F, Up));
    Vec3 U = HMM_Cross(S, F);

    return _HMM_LookAt(F, S, U, Eye);
}

COVERAGE(HMM_LookAt_LH, 1)
static inline Mat4 HMM_LookAt_LH(Vec3 Eye, Vec3 Center, Vec3 Up)
{
    ASSERT_COVERED(HMM_LookAt_LH);

    Vec3 F = HMM_NormV3(HMM_SubV3(Eye, Center));
    Vec3 S = HMM_NormV3(HMM_Cross(F, Up));
    Vec3 U = HMM_Cross(S, F);

    return _HMM_LookAt(F, S, U, Eye);
}

COVERAGE(HMM_InvLookAt, 1)
static inline Mat4 HMM_InvLookAt(Mat4 Matrix)
{
    ASSERT_COVERED(HMM_InvLookAt);
    Mat4 Result;

    Mat3 Rotation;
    Rotation.Columns[0] = Matrix.Columns[0].XYZ;
    Rotation.Columns[1] = Matrix.Columns[1].XYZ;
    Rotation.Columns[2] = Matrix.Columns[2].XYZ;
    Rotation = HMM_TransposeM3(Rotation);

    Result.Columns[0] = HMM_V4V(Rotation.Columns[0], 0.0f);
    Result.Columns[1] = HMM_V4V(Rotation.Columns[1], 0.0f);
    Result.Columns[2] = HMM_V4V(Rotation.Columns[2], 0.0f);
    Result.Columns[3] = HMM_MulV4F(Matrix.Columns[3], -1.0f);
    Result.Elements[3][0] = -1.0f * Matrix.Elements[3][0] /
        (Rotation.Elements[0][0] + Rotation.Elements[0][1] + Rotation.Elements[0][2]);
    Result.Elements[3][1] = -1.0f * Matrix.Elements[3][1] /
        (Rotation.Elements[1][0] + Rotation.Elements[1][1] + Rotation.Elements[1][2]);
    Result.Elements[3][2] = -1.0f * Matrix.Elements[3][2] /
        (Rotation.Elements[2][0] + Rotation.Elements[2][1] + Rotation.Elements[2][2]);
    Result.Elements[3][3] = 1.0f;

    return Result;
}

static inline Mat4 HMM_Transform(Vec3 Position, Vec3 Rotation, Vec3 Scaling)
{
    Mat4 Dst = HMM_Translate(Position);
    Dst = HMM_MulM4(Dst, HMM_MulM4(HMM_Rotate_LH(Rotation[0], HMM_V3(1, 0, 0)), HMM_MulM4(HMM_Rotate_LH(Rotation[1], HMM_V3(0, 1, 0)), HMM_Rotate_LH(Rotation[2], HMM_V3(0, 0, 1)))));
    Dst = HMM_MulM4(Dst, HMM_Scale(Scaling));
    return Dst;
}

/*
 * Quaternion operations
 */

COVERAGE(HMM_Q, 1)
static inline Quat HMM_Q(float X, float Y, float Z, float W)
{
    ASSERT_COVERED(HMM_Q);

    Quat Result;

#ifdef HANDMADE_MATH__USE_SSE
    Result.SSE = _mm_setr_ps(X, Y, Z, W);
#else
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    Result.w = W;
#endif

    return Result;
}

COVERAGE(HMM_QV4, 1)
static inline Quat HMM_QV4(Vec4 Vector)
{
    ASSERT_COVERED(HMM_QV4);

    Quat Result;

#ifdef HANDMADE_MATH__USE_SSE
    Result.SSE = Vector.SSE;
#else
    Result.x = Vector.x;
    Result.y = Vector.y;
    Result.z = Vector.z;
    Result.w = Vector.w;
#endif

    return Result;
}

COVERAGE(HMM_AddQ, 1)
static inline Quat HMM_AddQ(Quat Left, Quat Right)
{
    ASSERT_COVERED(HMM_AddQ);

    Quat Result;

#ifdef HANDMADE_MATH__USE_SSE
    Result.SSE = _mm_add_ps(Left.SSE, Right.SSE);
#else

    Result.x = Left.x + Right.x;
    Result.y = Left.y + Right.y;
    Result.z = Left.z + Right.z;
    Result.w = Left.w + Right.w;
#endif

    return Result;
}

COVERAGE(HMM_SubQ, 1)
static inline Quat HMM_SubQ(Quat Left, Quat Right)
{
    ASSERT_COVERED(HMM_SubQ);

    Quat Result;

#ifdef HANDMADE_MATH__USE_SSE
    Result.SSE = _mm_sub_ps(Left.SSE, Right.SSE);
#else
    Result.x = Left.x - Right.x;
    Result.y = Left.y - Right.y;
    Result.z = Left.z - Right.z;
    Result.w = Left.w - Right.w;
#endif

    return Result;
}

COVERAGE(HMM_MulQ, 1)
static inline Quat HMM_MulQ(Quat Left, Quat Right)
{
    ASSERT_COVERED(HMM_MulQ);

    Quat Result;

#ifdef HANDMADE_MATH__USE_SSE
    __m128 SSEResultOne = _mm_xor_ps(_mm_shuffle_ps(Left.SSE, Left.SSE, _MM_SHUFFLE(0, 0, 0, 0)), _mm_setr_ps(0.f, -0.f, 0.f, -0.f));
    __m128 SSEResultTwo = _mm_shuffle_ps(Right.SSE, Right.SSE, _MM_SHUFFLE(0, 1, 2, 3));
    __m128 SSEResultThree = _mm_mul_ps(SSEResultTwo, SSEResultOne);

    SSEResultOne = _mm_xor_ps(_mm_shuffle_ps(Left.SSE, Left.SSE, _MM_SHUFFLE(1, 1, 1, 1)) , _mm_setr_ps(0.f, 0.f, -0.f, -0.f));
    SSEResultTwo = _mm_shuffle_ps(Right.SSE, Right.SSE, _MM_SHUFFLE(1, 0, 3, 2));
    SSEResultThree = _mm_add_ps(SSEResultThree, _mm_mul_ps(SSEResultTwo, SSEResultOne));

    SSEResultOne = _mm_xor_ps(_mm_shuffle_ps(Left.SSE, Left.SSE, _MM_SHUFFLE(2, 2, 2, 2)), _mm_setr_ps(-0.f, 0.f, 0.f, -0.f));
    SSEResultTwo = _mm_shuffle_ps(Right.SSE, Right.SSE, _MM_SHUFFLE(2, 3, 0, 1));
    SSEResultThree = _mm_add_ps(SSEResultThree, _mm_mul_ps(SSEResultTwo, SSEResultOne));

    SSEResultOne = _mm_shuffle_ps(Left.SSE, Left.SSE, _MM_SHUFFLE(3, 3, 3, 3));
    SSEResultTwo = _mm_shuffle_ps(Right.SSE, Right.SSE, _MM_SHUFFLE(3, 2, 1, 0));
    Result.SSE = _mm_add_ps(SSEResultThree, _mm_mul_ps(SSEResultTwo, SSEResultOne));
#else
    Result.x =  Right.Elements[3] * +Left.Elements[0];
    Result.y =  Right.Elements[2] * -Left.Elements[0];
    Result.z =  Right.Elements[1] * +Left.Elements[0];
    Result.w =  Right.Elements[0] * -Left.Elements[0];

    Result.x += Right.Elements[2] * +Left.Elements[1];
    Result.y += Right.Elements[3] * +Left.Elements[1];
    Result.z += Right.Elements[0] * -Left.Elements[1];
    Result.w += Right.Elements[1] * -Left.Elements[1];
    
    Result.x += Right.Elements[1] * -Left.Elements[2];
    Result.y += Right.Elements[0] * +Left.Elements[2];
    Result.z += Right.Elements[3] * +Left.Elements[2];
    Result.w += Right.Elements[2] * -Left.Elements[2];

    Result.x += Right.Elements[0] * +Left.Elements[3];
    Result.y += Right.Elements[1] * +Left.Elements[3];
    Result.z += Right.Elements[2] * +Left.Elements[3];
    Result.w += Right.Elements[3] * +Left.Elements[3];
#endif

    return Result;
}

COVERAGE(HMM_MulQF, 1)
static inline Quat HMM_MulQF(Quat Left, float Multiplicative)
{
    ASSERT_COVERED(HMM_MulQF);

    Quat Result;

#ifdef HANDMADE_MATH__USE_SSE
    __m128 Scalar = _mm_set1_ps(Multiplicative);
    Result.SSE = _mm_mul_ps(Left.SSE, Scalar);
#else
    Result.x = Left.x * Multiplicative;
    Result.y = Left.y * Multiplicative;
    Result.z = Left.z * Multiplicative;
    Result.w = Left.w * Multiplicative;
#endif

    return Result;
}

COVERAGE(HMM_DivQF, 1)
static inline Quat HMM_DivQF(Quat Left, float Divnd)
{
    ASSERT_COVERED(HMM_DivQF);

    Quat Result;

#ifdef HANDMADE_MATH__USE_SSE
    __m128 Scalar = _mm_set1_ps(Divnd);
    Result.SSE = _mm_div_ps(Left.SSE, Scalar);
#else
    Result.x = Left.x / Divnd;
    Result.y = Left.y / Divnd;
    Result.z = Left.z / Divnd;
    Result.w = Left.w / Divnd;
#endif

    return Result;
}

COVERAGE(HMM_DotQ, 1)
static inline float HMM_DotQ(Quat Left, Quat Right)
{
    ASSERT_COVERED(HMM_DotQ);

    float Result;

#ifdef HANDMADE_MATH__USE_SSE
    __m128 SSEResultOne = _mm_mul_ps(Left.SSE, Right.SSE);
    __m128 SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(2, 3, 0, 1));
    SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
    SSEResultTwo = _mm_shuffle_ps(SSEResultOne, SSEResultOne, _MM_SHUFFLE(0, 1, 2, 3));
    SSEResultOne = _mm_add_ps(SSEResultOne, SSEResultTwo);
    _mm_store_ss(&Result, SSEResultOne);
#else
    Result = ((Left.x * Right.x) + (Left.z * Right.z)) + ((Left.y * Right.y) + (Left.w * Right.w));
#endif

    return Result;
}

COVERAGE(HMM_InvQ, 1)
static inline Quat HMM_InvQ(Quat Left)
{
    ASSERT_COVERED(HMM_InvQ);
    
    Quat Result;
    Result.x = -Left.x;
    Result.y = -Left.y;
    Result.z = -Left.z;
    Result.w = Left.w;

    return HMM_DivQF(Result, (HMM_DotQ(Left, Left)));
}

COVERAGE(HMM_NormQ, 1)
static inline Quat HMM_NormQ(Quat Quat)
{
    ASSERT_COVERED(HMM_NormQ);

    /* NOTE(lcf): Take advantage of SSE implementation in HMM_NormV4 */
    Vec4 Vec = {Quat.x, Quat.y, Quat.z, Quat.w};
    Vec = HMM_NormV4(Vec);
    Quat Result(Vec.x, Vec.y, Vec.z, Vec.w);

    return Result;
}

static inline Quat _HMM_MixQ(Quat Left, float MixLeft, Quat Right, float MixRight) {
    Quat Result;

#ifdef HANDMADE_MATH__USE_SSE
    __m128 ScalarLeft = _mm_set1_ps(MixLeft);
    __m128 ScalarRight = _mm_set1_ps(MixRight);
    __m128 SSEResultOne = _mm_mul_ps(Left.SSE, ScalarLeft);
    __m128 SSEResultTwo = _mm_mul_ps(Right.SSE, ScalarRight);
    Result.SSE = _mm_add_ps(SSEResultOne, SSEResultTwo);
#else
    Result.x = Left.x*MixLeft + Right.x*MixRight;
    Result.y = Left.y*MixLeft + Right.y*MixRight;
    Result.z = Left.z*MixLeft + Right.z*MixRight;
    Result.w = Left.w*MixLeft + Right.w*MixRight;
#endif

    return Result;
}

COVERAGE(HMM_NLerp, 1)
static inline Quat HMM_NLerp(Quat Left, float Time, Quat Right)
{
    ASSERT_COVERED(HMM_NLerp);

    Quat Result = _HMM_MixQ(Left, 1.0f-Time, Right, Time);
    Result = HMM_NormQ(Result);

    return Result;
}

COVERAGE(HMM_SLerp, 1)
static inline Quat HMM_SLerp(Quat Left, float Time, Quat Right)
{
    ASSERT_COVERED(HMM_SLerp);

    Quat Result;

    float Cos_Theta = HMM_DotQ(Left, Right);

    if (Cos_Theta < 0.0f) { /* NOTE(lcf): Take shortest path on Hyper-sphere */
        Cos_Theta = -Cos_Theta;
        Right = HMM_Q(-Right.x, -Right.y, -Right.z, -Right.w);
    }
    
    /* NOTE(lcf): Use Normalized Linear interpolation when vectors are roughly not L.I. */
    if (Cos_Theta > 0.9995f) {
        Result = HMM_NLerp(Left, Time, Right);
    } else {
        float Angle = HMM_ACosF(Cos_Theta);
        float MixLeft = HMM_SinF((1.0f - Time) * Angle);
        float MixRight = HMM_SinF(Time * Angle);

        Result = _HMM_MixQ(Left, MixLeft, Right, MixRight);
        Result = HMM_NormQ(Result);
    }
    
    return Result;
}

COVERAGE(HMM_QToM4, 1)
static inline Mat4 HMM_QToM4(Quat Left)
{
    ASSERT_COVERED(HMM_QToM4);

    Mat4 Result;

    Quat NormalizedQ = HMM_NormQ(Left);

    float XX, YY, ZZ,
          XY, XZ, YZ,
          WX, WY, WZ;

    XX = NormalizedQ.x * NormalizedQ.x;
    YY = NormalizedQ.y * NormalizedQ.y;
    ZZ = NormalizedQ.z * NormalizedQ.z;
    XY = NormalizedQ.x * NormalizedQ.y;
    XZ = NormalizedQ.x * NormalizedQ.z;
    YZ = NormalizedQ.y * NormalizedQ.z;
    WX = NormalizedQ.w * NormalizedQ.x;
    WY = NormalizedQ.w * NormalizedQ.y;
    WZ = NormalizedQ.w * NormalizedQ.z;

    Result.Elements[0][0] = 1.0f - 2.0f * (YY + ZZ);
    Result.Elements[0][1] = 2.0f * (XY + WZ);
    Result.Elements[0][2] = 2.0f * (XZ - WY);
    Result.Elements[0][3] = 0.0f;

    Result.Elements[1][0] = 2.0f * (XY - WZ);
    Result.Elements[1][1] = 1.0f - 2.0f * (XX + ZZ);
    Result.Elements[1][2] = 2.0f * (YZ + WX);
    Result.Elements[1][3] = 0.0f;

    Result.Elements[2][0] = 2.0f * (XZ + WY);
    Result.Elements[2][1] = 2.0f * (YZ - WX);
    Result.Elements[2][2] = 1.0f - 2.0f * (XX + YY);
    Result.Elements[2][3] = 0.0f;

    Result.Elements[3][0] = 0.0f;
    Result.Elements[3][1] = 0.0f;
    Result.Elements[3][2] = 0.0f;
    Result.Elements[3][3] = 1.0f;

    return Result;
}

// This method taken from Mike Day at Insomniac Games.
// https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/get_matrix-to-quat.pdf
//
// Note that as mentioned at the top of the paper, the paper assumes the get_matrix
// would be *post*-multiplied to a vector to rotate it, meaning the get_matrix is
// the transpose of what we're dealing with. But, because our matrices are
// stored in column-major order, the indices *appear* to match the paper.
//
// For example, m12 in the paper is row 1, column 2. We need to transpose it to
// row 2, column 1. But, because the column comes first when referencing
// elements, it looks like M.Elements[1][2].
//
// Don't be confused! Or if you must be confused, at least trust this
// comment. :)
COVERAGE(HMM_M4ToQ_RH, 4)
static inline Quat HMM_M4ToQ_RH(Mat4 M)
{
    float T;
    Quat Q;

    if (M.Elements[2][2] < 0.0f) {
        if (M.Elements[0][0] > M.Elements[1][1]) {
            ASSERT_COVERED(HMM_M4ToQ_RH);

            T = 1 + M.Elements[0][0] - M.Elements[1][1] - M.Elements[2][2];
            Q = HMM_Q(
                T,
                M.Elements[0][1] + M.Elements[1][0],
                M.Elements[2][0] + M.Elements[0][2],
                M.Elements[1][2] - M.Elements[2][1]
            );
        } else {
            ASSERT_COVERED(HMM_M4ToQ_RH);

            T = 1 - M.Elements[0][0] + M.Elements[1][1] - M.Elements[2][2];
            Q = HMM_Q(
                M.Elements[0][1] + M.Elements[1][0],
                T,
                M.Elements[1][2] + M.Elements[2][1],
                M.Elements[2][0] - M.Elements[0][2]
            );
        }
    } else {
        if (M.Elements[0][0] < -M.Elements[1][1]) {
            ASSERT_COVERED(HMM_M4ToQ_RH);

            T = 1 - M.Elements[0][0] - M.Elements[1][1] + M.Elements[2][2];
            Q = HMM_Q(
                M.Elements[2][0] + M.Elements[0][2],
                M.Elements[1][2] + M.Elements[2][1],
                T,
                M.Elements[0][1] - M.Elements[1][0]
            );
        } else {
            ASSERT_COVERED(HMM_M4ToQ_RH);

            T = 1 + M.Elements[0][0] + M.Elements[1][1] + M.Elements[2][2];
            Q = HMM_Q(
                M.Elements[1][2] - M.Elements[2][1],
                M.Elements[2][0] - M.Elements[0][2],
                M.Elements[0][1] - M.Elements[1][0],
                T
            );
        }
    }

    Q = HMM_MulQF(Q, 0.5f / HMM_SqrtF(T));

    return Q;
}

COVERAGE(HMM_M4ToQ_LH, 4)
static inline Quat HMM_M4ToQ_LH(Mat4 M)
{
    float T;
    Quat Q;

    if (M.Elements[2][2] < 0.0f) {
        if (M.Elements[0][0] > M.Elements[1][1]) {
            ASSERT_COVERED(HMM_M4ToQ_LH);

            T = 1 + M.Elements[0][0] - M.Elements[1][1] - M.Elements[2][2];
            Q = HMM_Q(
                T,
                M.Elements[0][1] + M.Elements[1][0],
                M.Elements[2][0] + M.Elements[0][2],
                M.Elements[2][1] - M.Elements[1][2]
            );
        } else {
            ASSERT_COVERED(HMM_M4ToQ_LH);

            T = 1 - M.Elements[0][0] + M.Elements[1][1] - M.Elements[2][2];
            Q = HMM_Q(
                M.Elements[0][1] + M.Elements[1][0],
                T,
                M.Elements[1][2] + M.Elements[2][1],
                M.Elements[0][2] - M.Elements[2][0]
            );
        }
    } else {
        if (M.Elements[0][0] < -M.Elements[1][1]) {
            ASSERT_COVERED(HMM_M4ToQ_LH);

            T = 1 - M.Elements[0][0] - M.Elements[1][1] + M.Elements[2][2];
            Q = HMM_Q(
                M.Elements[2][0] + M.Elements[0][2],
                M.Elements[1][2] + M.Elements[2][1],
                T,
                M.Elements[1][0] - M.Elements[0][1]
            );
        } else {
            ASSERT_COVERED(HMM_M4ToQ_LH);

            T = 1 + M.Elements[0][0] + M.Elements[1][1] + M.Elements[2][2];
            Q = HMM_Q(
                M.Elements[2][1] - M.Elements[1][2],
                M.Elements[0][2] - M.Elements[2][0],
                M.Elements[1][0] - M.Elements[0][2],
                T
            );
        }
    }

    Q = HMM_MulQF(Q, 0.5f / HMM_SqrtF(T));

    return Q;
}


COVERAGE(HMM_QFromAxisAngle_RH, 1)
static inline Quat HMM_QFromAxisAngle_RH(Vec3 Axis, float AngleOfRotation)
{
    ASSERT_COVERED(HMM_QFromAxisAngle_RH);

    Quat Result;

    Vec3 AxisNormalized = HMM_NormV3(Axis);
    float SineOfRotation = HMM_SinF(AngleOfRotation / 2.0f);

    Result.XYZ = HMM_MulV3F(AxisNormalized, SineOfRotation);
    Result.w = HMM_CosF(AngleOfRotation / 2.0f);

    return Result;
}

COVERAGE(HMM_QFromAxisAngle_LH, 1)
static inline Quat HMM_QFromAxisAngle_LH(Vec3 Axis, float AngleOfRotation)
{
    ASSERT_COVERED(HMM_QFromAxisAngle_LH);

    return HMM_QFromAxisAngle_RH(Axis, -AngleOfRotation);
}


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

COVERAGE(HMM_LenV2CPP, 1)
static inline float HMM_Len(Vec2 A)
{
    ASSERT_COVERED(HMM_LenV2CPP);
    return HMM_LenV2(A);
}

COVERAGE(HMM_LenV3CPP, 1)
static inline float HMM_Len(Vec3 A)
{
    ASSERT_COVERED(HMM_LenV3CPP);
    return HMM_LenV3(A);
}

COVERAGE(HMM_LenV4CPP, 1)
static inline float HMM_Len(Vec4 A)
{
    ASSERT_COVERED(HMM_LenV4CPP);
    return HMM_LenV4(A);
}

COVERAGE(HMM_LenSqrV2CPP, 1)
static inline float HMM_LenSqr(Vec2 A)
{
    ASSERT_COVERED(HMM_LenSqrV2CPP);
    return HMM_LenSqrV2(A);
}

COVERAGE(HMM_LenSqrV3CPP, 1)
static inline float HMM_LenSqr(Vec3 A)
{
    ASSERT_COVERED(HMM_LenSqrV3CPP);
    return HMM_LenSqrV3(A);
}

COVERAGE(HMM_LenSqrV4CPP, 1)
static inline float HMM_LenSqr(Vec4 A)
{
    ASSERT_COVERED(HMM_LenSqrV4CPP);
    return HMM_LenSqrV4(A);
}

COVERAGE(HMM_NormV2CPP, 1)
static inline Vec2 HMM_Norm(Vec2 A)
{
    ASSERT_COVERED(HMM_NormV2CPP);
    return HMM_NormV2(A);
}

COVERAGE(HMM_NormV3CPP, 1)
static inline Vec3 HMM_Norm(Vec3 A)
{
    ASSERT_COVERED(HMM_NormV3CPP);
    return HMM_NormV3(A);
}

COVERAGE(HMM_NormV4CPP, 1)
static inline Vec4 HMM_Norm(Vec4 A)
{
    ASSERT_COVERED(HMM_NormV4CPP);
    return HMM_NormV4(A);
}

COVERAGE(HMM_NormQCPP, 1)
static inline Quat HMM_Norm(Quat A)
{
    ASSERT_COVERED(HMM_NormQCPP);
    return HMM_NormQ(A);
}

COVERAGE(HMM_DotV2CPP, 1)
static inline float HMM_Dot(Vec2 Left, Vec2 VecTwo)
{
    ASSERT_COVERED(HMM_DotV2CPP);
    return HMM_DotV2(Left, VecTwo);
}

COVERAGE(HMM_DotV3CPP, 1)
static inline float HMM_Dot(Vec3 Left, Vec3 VecTwo)
{
    ASSERT_COVERED(HMM_DotV3CPP);
    return HMM_DotV3(Left, VecTwo);
}

COVERAGE(HMM_DotV4CPP, 1)
static inline float HMM_Dot(Vec4 Left, Vec4 VecTwo)
{
    ASSERT_COVERED(HMM_DotV4CPP);
    return HMM_DotV4(Left, VecTwo);
}
 
COVERAGE(HMM_LerpV2CPP, 1)
static inline Vec2 HMM_Lerp(Vec2 Left, float Time, Vec2 Right) 
{
    ASSERT_COVERED(HMM_LerpV2CPP);
    return HMM_LerpV2(Left, Time, Right);
}

COVERAGE(HMM_LerpV3CPP, 1)
static inline Vec3 HMM_Lerp(Vec3 Left, float Time, Vec3 Right) 
{
    ASSERT_COVERED(HMM_LerpV3CPP);
    return HMM_LerpV3(Left, Time, Right);
}

COVERAGE(HMM_LerpV4CPP, 1)
static inline Vec4 HMM_Lerp(Vec4 Left, float Time, Vec4 Right) 
{
    ASSERT_COVERED(HMM_LerpV4CPP);
    return HMM_LerpV4(Left, Time, Right);
}

COVERAGE(HMM_TransposeM2CPP, 1)
static inline Mat2 HMM_Transpose(Mat2 Matrix)
{
    ASSERT_COVERED(HMM_TransposeM2CPP);
    return HMM_TransposeM2(Matrix);
}

COVERAGE(HMM_TransposeM3CPP, 1)
static inline Mat3 HMM_Transpose(Mat3 Matrix)
{
    ASSERT_COVERED(HMM_TransposeM3CPP);
    return HMM_TransposeM3(Matrix);
}

COVERAGE(HMM_TransposeM4CPP, 1)
static inline Mat4 HMM_Transpose(Mat4 Matrix)
{
    ASSERT_COVERED(HMM_TransposeM4CPP);
    return HMM_TransposeM4(Matrix);
}

COVERAGE(HMM_DeterminantM2CPP, 1)
static inline float HMM_Determinant(Mat2 Matrix)
{
    ASSERT_COVERED(HMM_DeterminantM2CPP);
    return HMM_DeterminantM2(Matrix);
}

COVERAGE(HMM_DeterminantM3CPP, 1)
static inline float HMM_Determinant(Mat3 Matrix)
{
    ASSERT_COVERED(HMM_DeterminantM3CPP);
    return HMM_DeterminantM3(Matrix);
}

COVERAGE(HMM_DeterminantM4CPP, 1)
static inline float HMM_Determinant(Mat4 Matrix)
{
    ASSERT_COVERED(HMM_DeterminantM4CPP);
    return HMM_DeterminantM4(Matrix);
}

COVERAGE(HMM_InvGeneralM2CPP, 1)
static inline Mat2 HMM_InvGeneral(Mat2 Matrix)
{
    ASSERT_COVERED(HMM_InvGeneralM2CPP);
    return HMM_InvGeneralM2(Matrix);
}

COVERAGE(HMM_InvGeneralM3CPP, 1)
static inline Mat3 HMM_InvGeneral(Mat3 Matrix)
{
    ASSERT_COVERED(HMM_InvGeneralM3CPP);
    return HMM_InvGeneralM3(Matrix);
}

COVERAGE(HMM_InvGeneralM4CPP, 1)
static inline Mat4 HMM_InvGeneral(Mat4 Matrix)
{
    ASSERT_COVERED(HMM_InvGeneralM4CPP);
    return HMM_InvGeneralM4(Matrix);
}

COVERAGE(HMM_DotQCPP, 1)
static inline float HMM_Dot(Quat QuatOne, Quat QuatTwo)
{
    ASSERT_COVERED(HMM_DotQCPP);
    return HMM_DotQ(QuatOne, QuatTwo);
}

COVERAGE(HMM_AddV2CPP, 1)
static inline Vec2 HMM_Add(Vec2 Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_AddV2CPP);
    return HMM_AddV2(Left, Right);
}

COVERAGE(HMM_AddV3CPP, 1)
static inline Vec3 HMM_Add(Vec3 Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_AddV3CPP);
    return HMM_AddV3(Left, Right);
}

COVERAGE(HMM_AddV4CPP, 1)
static inline Vec4 HMM_Add(Vec4 Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_AddV4CPP);
    return HMM_AddV4(Left, Right);
}

COVERAGE(HMM_AddM2CPP, 1)
static inline Mat2 HMM_Add(Mat2 Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_AddM2CPP);
    return HMM_AddM2(Left, Right);
}

COVERAGE(HMM_AddM3CPP, 1)
static inline Mat3 HMM_Add(Mat3 Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_AddM3CPP);
    return HMM_AddM3(Left, Right);
}

COVERAGE(HMM_AddM4CPP, 1)
static inline Mat4 HMM_Add(Mat4 Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_AddM4CPP);
    return HMM_AddM4(Left, Right);
}

COVERAGE(HMM_AddQCPP, 1)
static inline Quat HMM_Add(Quat Left, Quat Right)
{
    ASSERT_COVERED(HMM_AddQCPP);
    return HMM_AddQ(Left, Right);
}

COVERAGE(HMM_SubV2CPP, 1)
static inline Vec2 HMM_Sub(Vec2 Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_SubV2CPP);
    return HMM_SubV2(Left, Right);
}

COVERAGE(HMM_SubV3CPP, 1)
static inline Vec3 HMM_Sub(Vec3 Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_SubV3CPP);
    return HMM_SubV3(Left, Right);
}

COVERAGE(HMM_SubV4CPP, 1)
static inline Vec4 HMM_Sub(Vec4 Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_SubV4CPP);
    return HMM_SubV4(Left, Right);
}

COVERAGE(HMM_SubM2CPP, 1)
static inline Mat2 HMM_Sub(Mat2 Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_SubM2CPP);
    return HMM_SubM2(Left, Right);
}

COVERAGE(HMM_SubM3CPP, 1)
static inline Mat3 HMM_Sub(Mat3 Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_SubM3CPP);
    return HMM_SubM3(Left, Right);
}

COVERAGE(HMM_SubM4CPP, 1)
static inline Mat4 HMM_Sub(Mat4 Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_SubM4CPP);
    return HMM_SubM4(Left, Right);
}

COVERAGE(HMM_SubQCPP, 1)
static inline Quat HMM_Sub(Quat Left, Quat Right)
{
    ASSERT_COVERED(HMM_SubQCPP);
    return HMM_SubQ(Left, Right);
}

COVERAGE(HMM_MulV2CPP, 1)
static inline Vec2 HMM_Mul(Vec2 Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_MulV2CPP);
    return HMM_MulV2(Left, Right);
}

COVERAGE(HMM_MulV2FCPP, 1)
static inline Vec2 HMM_Mul(Vec2 Left, float Right)
{
    ASSERT_COVERED(HMM_MulV2FCPP);
    return HMM_MulV2F(Left, Right);
}

COVERAGE(HMM_MulV3CPP, 1)
static inline Vec3 HMM_Mul(Vec3 Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_MulV3CPP);
    return HMM_MulV3(Left, Right);
}

COVERAGE(HMM_MulV3FCPP, 1)
static inline Vec3 HMM_Mul(Vec3 Left, float Right)
{
    ASSERT_COVERED(HMM_MulV3FCPP);
    return HMM_MulV3F(Left, Right);
}

COVERAGE(HMM_MulV4CPP, 1)
static inline Vec4 HMM_Mul(Vec4 Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_MulV4CPP);
    return HMM_MulV4(Left, Right);
}

COVERAGE(HMM_MulV4FCPP, 1)
static inline Vec4 HMM_Mul(Vec4 Left, float Right)
{
    ASSERT_COVERED(HMM_MulV4FCPP);
    return HMM_MulV4F(Left, Right);
}

COVERAGE(HMM_MulM2CPP, 1)
static inline Mat2 HMM_Mul(Mat2 Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_MulM2CPP);
    return HMM_MulM2(Left, Right);
}

COVERAGE(HMM_MulM3CPP, 1)
static inline Mat3 HMM_Mul(Mat3 Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_MulM3CPP);
    return HMM_MulM3(Left, Right);
}

COVERAGE(HMM_MulM4CPP, 1)
static inline Mat4 HMM_Mul(Mat4 Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_MulM4CPP);
    return HMM_MulM4(Left, Right);
}

COVERAGE(HMM_MulM2FCPP, 1)
static inline Mat2 HMM_Mul(Mat2 Left, float Right)
{
    ASSERT_COVERED(HMM_MulM2FCPP);
    return HMM_MulM2F(Left, Right);
}

COVERAGE(HMM_MulM3FCPP, 1)
static inline Mat3 HMM_Mul(Mat3 Left, float Right)
{
    ASSERT_COVERED(HMM_MulM3FCPP);
    return HMM_MulM3F(Left, Right);
}

COVERAGE(HMM_MulM4FCPP, 1)
static inline Mat4 HMM_Mul(Mat4 Left, float Right)
{
    ASSERT_COVERED(HMM_MulM4FCPP);
    return HMM_MulM4F(Left, Right);
}

COVERAGE(HMM_MulM2V2CPP, 1)
static inline Vec2 HMM_Mul(Mat2 Matrix, Vec2 Vector)
{
    ASSERT_COVERED(HMM_MulM2V2CPP);
    return HMM_MulM2V2(Matrix, Vector);
}

COVERAGE(HMM_MulM3V3CPP, 1)
static inline Vec3 HMM_Mul(Mat3 Matrix, Vec3 Vector)
{
    ASSERT_COVERED(HMM_MulM3V3CPP);
    return HMM_MulM3V3(Matrix, Vector);
}

COVERAGE(HMM_MulM4V4CPP, 1)
static inline Vec4 HMM_Mul(Mat4 Matrix, Vec4 Vector)
{
    ASSERT_COVERED(HMM_MulM4V4CPP);
    return HMM_MulM4V4(Matrix, Vector);
}

COVERAGE(HMM_MulQCPP, 1)
static inline Quat HMM_Mul(Quat Left, Quat Right)
{
    ASSERT_COVERED(HMM_MulQCPP);
    return HMM_MulQ(Left, Right);
}

COVERAGE(HMM_MulQFCPP, 1)
static inline Quat HMM_Mul(Quat Left, float Right)
{
    ASSERT_COVERED(HMM_MulQFCPP);
    return HMM_MulQF(Left, Right);
}

COVERAGE(HMM_DivV2CPP, 1)
static inline Vec2 HMM_Div(Vec2 Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_DivV2CPP);
    return HMM_DivV2(Left, Right);
}

COVERAGE(HMM_DivV2FCPP, 1)
static inline Vec2 HMM_Div(Vec2 Left, float Right)
{
    ASSERT_COVERED(HMM_DivV2FCPP);
    return HMM_DivV2F(Left, Right);
}

COVERAGE(HMM_DivV3CPP, 1)
static inline Vec3 HMM_Div(Vec3 Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_DivV3CPP);
    return HMM_DivV3(Left, Right);
}

COVERAGE(HMM_DivV3FCPP, 1)
static inline Vec3 HMM_Div(Vec3 Left, float Right)
{
    ASSERT_COVERED(HMM_DivV3FCPP);
    return HMM_DivV3F(Left, Right);
}

COVERAGE(HMM_DivV4CPP, 1)
static inline Vec4 HMM_Div(Vec4 Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_DivV4CPP);
    return HMM_DivV4(Left, Right);
}

COVERAGE(HMM_DivV4FCPP, 1)
static inline Vec4 HMM_Div(Vec4 Left, float Right)
{
    ASSERT_COVERED(HMM_DivV4FCPP);
    return HMM_DivV4F(Left, Right);
}

COVERAGE(HMM_DivM2FCPP, 1)
static inline Mat2 HMM_Div(Mat2 Left, float Right)
{
    ASSERT_COVERED(HMM_DivM2FCPP);
    return HMM_DivM2F(Left, Right);
}

COVERAGE(HMM_DivM3FCPP, 1)
static inline Mat3 HMM_Div(Mat3 Left, float Right)
{
    ASSERT_COVERED(HMM_DivM3FCPP);
    return HMM_DivM3F(Left, Right);
}

COVERAGE(HMM_DivM4FCPP, 1)
static inline Mat4 HMM_Div(Mat4 Left, float Right)
{
    ASSERT_COVERED(HMM_DivM4FCPP);
    return HMM_DivM4F(Left, Right);
}

COVERAGE(HMM_DivQFCPP, 1)
static inline Quat HMM_Div(Quat Left, float Right)
{
    ASSERT_COVERED(HMM_DivQFCPP);
    return HMM_DivQF(Left, Right);
}

COVERAGE(HMM_EqV2CPP, 1)
static inline HMM_Bool HMM_Eq(Vec2 Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_EqV2CPP);
    return HMM_EqV2(Left, Right);
}

COVERAGE(HMM_EqV3CPP, 1)
static inline HMM_Bool HMM_Eq(Vec3 Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_EqV3CPP);
    return HMM_EqV3(Left, Right);
}

COVERAGE(HMM_EqV4CPP, 1)
static inline HMM_Bool HMM_Eq(Vec4 Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_EqV4CPP);
    return HMM_EqV4(Left, Right);
}

COVERAGE(HMM_AddV2Op, 1)
static inline Vec2 operator+(Vec2 Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_AddV2Op);
    return HMM_AddV2(Left, Right);
}

COVERAGE(HMM_AddV3Op, 1)
static inline Vec3 operator+(Vec3 Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_AddV3Op);
    return HMM_AddV3(Left, Right);
}

COVERAGE(HMM_AddV4Op, 1)
static inline Vec4 operator+(Vec4 Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_AddV4Op);
    return HMM_AddV4(Left, Right);
}

COVERAGE(HMM_AddM2Op, 1)
static inline Mat2 operator+(Mat2 Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_AddM2Op);
    return HMM_AddM2(Left, Right);
}

COVERAGE(HMM_AddM3Op, 1)
static inline Mat3 operator+(Mat3 Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_AddM3Op);
    return HMM_AddM3(Left, Right);
}

COVERAGE(HMM_AddM4Op, 1)
static inline Mat4 operator+(Mat4 Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_AddM4Op);
    return HMM_AddM4(Left, Right);
}

COVERAGE(HMM_AddQOp, 1)
static inline Quat operator+(Quat Left, Quat Right)
{
    ASSERT_COVERED(HMM_AddQOp);
    return HMM_AddQ(Left, Right);
}

COVERAGE(HMM_SubV2Op, 1)
static inline Vec2 operator-(Vec2 Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_SubV2Op);
    return HMM_SubV2(Left, Right);
}

COVERAGE(HMM_SubV3Op, 1)
static inline Vec3 operator-(Vec3 Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_SubV3Op);
    return HMM_SubV3(Left, Right);
}

COVERAGE(HMM_SubV4Op, 1)
static inline Vec4 operator-(Vec4 Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_SubV4Op);
    return HMM_SubV4(Left, Right);
}

COVERAGE(HMM_SubM2Op, 1)
static inline Mat2 operator-(Mat2 Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_SubM2Op);
    return HMM_SubM2(Left, Right);
}

COVERAGE(HMM_SubM3Op, 1)
static inline Mat3 operator-(Mat3 Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_SubM3Op);
    return HMM_SubM3(Left, Right);
}

COVERAGE(HMM_SubM4Op, 1)
static inline Mat4 operator-(Mat4 Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_SubM4Op);
    return HMM_SubM4(Left, Right);
}

COVERAGE(HMM_SubQOp, 1)
static inline Quat operator-(Quat Left, Quat Right)
{
    ASSERT_COVERED(HMM_SubQOp);
    return HMM_SubQ(Left, Right);
}

COVERAGE(HMM_MulV2Op, 1)
static inline Vec2 operator*(Vec2 Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_MulV2Op);
    return HMM_MulV2(Left, Right);
}

COVERAGE(HMM_MulV3Op, 1)
static inline Vec3 operator*(Vec3 Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_MulV3Op);
    return HMM_MulV3(Left, Right);
}

COVERAGE(HMM_MulV4Op, 1)
static inline Vec4 operator*(Vec4 Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_MulV4Op);
    return HMM_MulV4(Left, Right);
}

COVERAGE(HMM_MulM2Op, 1)
static inline Mat2 operator*(Mat2 Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_MulM2Op);
    return HMM_MulM2(Left, Right);
}

COVERAGE(HMM_MulM3Op, 1)
static inline Mat3 operator*(Mat3 Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_MulM3Op);
    return HMM_MulM3(Left, Right);
}

COVERAGE(HMM_MulM4Op, 1)
static inline Mat4 operator*(Mat4 Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_MulM4Op);
    return HMM_MulM4(Left, Right);
}

COVERAGE(HMM_MulQOp, 1)
static inline Quat operator*(Quat Left, Quat Right)
{
    ASSERT_COVERED(HMM_MulQOp);
    return HMM_MulQ(Left, Right);
}

COVERAGE(HMM_MulV2FOp, 1)
static inline Vec2 operator*(Vec2 Left, float Right)
{
    ASSERT_COVERED(HMM_MulV2FOp);
    return HMM_MulV2F(Left, Right);
}

COVERAGE(HMM_MulV3FOp, 1)
static inline Vec3 operator*(Vec3 Left, float Right)
{
    ASSERT_COVERED(HMM_MulV3FOp);
    return HMM_MulV3F(Left, Right);
}

COVERAGE(HMM_MulV4FOp, 1)
static inline Vec4 operator*(Vec4 Left, float Right)
{
    ASSERT_COVERED(HMM_MulV4FOp);
    return HMM_MulV4F(Left, Right);
}

COVERAGE(HMM_MulM2FOp, 1)
static inline Mat2 operator*(Mat2 Left, float Right)
{
    ASSERT_COVERED(HMM_MulM2FOp);
    return HMM_MulM2F(Left, Right);
}

COVERAGE(HMM_MulM3FOp, 1)
static inline Mat3 operator*(Mat3 Left, float Right)
{
    ASSERT_COVERED(HMM_MulM3FOp);
    return HMM_MulM3F(Left, Right);
}

COVERAGE(HMM_MulM4FOp, 1)
static inline Mat4 operator*(Mat4 Left, float Right)
{
    ASSERT_COVERED(HMM_MulM4FOp);
    return HMM_MulM4F(Left, Right);
}

COVERAGE(HMM_MulQFOp, 1)
static inline Quat operator*(Quat Left, float Right)
{
    ASSERT_COVERED(HMM_MulQFOp);
    return HMM_MulQF(Left, Right);
}

COVERAGE(HMM_MulV2FOpLeft, 1)
static inline Vec2 operator*(float Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_MulV2FOpLeft);
    return HMM_MulV2F(Right, Left);
}

COVERAGE(HMM_MulV3FOpLeft, 1)
static inline Vec3 operator*(float Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_MulV3FOpLeft);
    return HMM_MulV3F(Right, Left);
}

COVERAGE(HMM_MulV4FOpLeft, 1)
static inline Vec4 operator*(float Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_MulV4FOpLeft);
    return HMM_MulV4F(Right, Left);
}

COVERAGE(HMM_MulM2FOpLeft, 1)
static inline Mat2 operator*(float Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_MulM2FOpLeft);
    return HMM_MulM2F(Right, Left);
}

COVERAGE(HMM_MulM3FOpLeft, 1)
static inline Mat3 operator*(float Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_MulM3FOpLeft);
    return HMM_MulM3F(Right, Left);
}

COVERAGE(HMM_MulM4FOpLeft, 1)
static inline Mat4 operator*(float Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_MulM4FOpLeft);
    return HMM_MulM4F(Right, Left);
}

COVERAGE(HMM_MulQFOpLeft, 1)
static inline Quat operator*(float Left, Quat Right)
{
    ASSERT_COVERED(HMM_MulQFOpLeft);
    return HMM_MulQF(Right, Left);
}

COVERAGE(HMM_MulM2V2Op, 1)
static inline Vec2 operator*(Mat2 Matrix, Vec2 Vector)
{
    ASSERT_COVERED(HMM_MulM2V2Op);
    return HMM_MulM2V2(Matrix, Vector);
}

COVERAGE(HMM_MulM3V3Op, 1)
static inline Vec3 operator*(Mat3 Matrix, Vec3 Vector)
{
    ASSERT_COVERED(HMM_MulM3V3Op);
    return HMM_MulM3V3(Matrix, Vector);
}

COVERAGE(HMM_MulM4V4Op, 1)
static inline Vec4 operator*(Mat4 Matrix, Vec4 Vector)
{
    ASSERT_COVERED(HMM_MulM4V4Op);
    return HMM_MulM4V4(Matrix, Vector);
}

COVERAGE(HMM_DivV2Op, 1)
static inline Vec2 operator/(Vec2 Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_DivV2Op);
    return HMM_DivV2(Left, Right);
}

COVERAGE(HMM_DivV3Op, 1)
static inline Vec3 operator/(Vec3 Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_DivV3Op);
    return HMM_DivV3(Left, Right);
}

COVERAGE(HMM_DivV4Op, 1)
static inline Vec4 operator/(Vec4 Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_DivV4Op);
    return HMM_DivV4(Left, Right);
}

COVERAGE(HMM_DivV2FOp, 1)
static inline Vec2 operator/(Vec2 Left, float Right)
{
    ASSERT_COVERED(HMM_DivV2FOp);
    return HMM_DivV2F(Left, Right);
}

COVERAGE(HMM_DivV3FOp, 1)
static inline Vec3 operator/(Vec3 Left, float Right)
{
    ASSERT_COVERED(HMM_DivV3FOp);
    return HMM_DivV3F(Left, Right);
}

COVERAGE(HMM_DivV4FOp, 1)
static inline Vec4 operator/(Vec4 Left, float Right)
{
    ASSERT_COVERED(HMM_DivV4FOp);
    return HMM_DivV4F(Left, Right);
}

COVERAGE(HMM_DivM4FOp, 1)
static inline Mat4 operator/(Mat4 Left, float Right)
{
    ASSERT_COVERED(HMM_DivM4FOp);
    return HMM_DivM4F(Left, Right);
}

COVERAGE(HMM_DivM3FOp, 1)
static inline Mat3 operator/(Mat3 Left, float Right)
{
    ASSERT_COVERED(HMM_DivM3FOp);
    return HMM_DivM3F(Left, Right);
}

COVERAGE(HMM_DivM2FOp, 1)
static inline Mat2 operator/(Mat2 Left, float Right)
{
    ASSERT_COVERED(HMM_DivM2FOp);
    return HMM_DivM2F(Left, Right);
}

COVERAGE(HMM_DivQFOp, 1)
static inline Quat operator/(Quat Left, float Right)
{
    ASSERT_COVERED(HMM_DivQFOp);
    return HMM_DivQF(Left, Right);
}

COVERAGE(HMM_AddV2Assign, 1)
static inline Vec2 &operator+=(Vec2 &Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_AddV2Assign);
    return Left = Left + Right;
}

COVERAGE(HMM_AddV3Assign, 1)
static inline Vec3 &operator+=(Vec3 &Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_AddV3Assign);
    return Left = Left + Right;
}

COVERAGE(HMM_AddV4Assign, 1)
static inline Vec4 &operator+=(Vec4 &Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_AddV4Assign);
    return Left = Left + Right;
}

COVERAGE(HMM_AddM2Assign, 1)
static inline Mat2 &operator+=(Mat2 &Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_AddM2Assign);
    return Left = Left + Right;
}

COVERAGE(HMM_AddM3Assign, 1)
static inline Mat3 &operator+=(Mat3 &Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_AddM3Assign);
    return Left = Left + Right;
}

COVERAGE(HMM_AddM4Assign, 1)
static inline Mat4 &operator+=(Mat4 &Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_AddM4Assign);
    return Left = Left + Right;
}

COVERAGE(HMM_AddQAssign, 1)
static inline Quat &operator+=(Quat &Left, Quat Right)
{
    ASSERT_COVERED(HMM_AddQAssign);
    return Left = Left + Right;
}

COVERAGE(HMM_SubV2Assign, 1)
static inline Vec2 &operator-=(Vec2 &Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_SubV2Assign);
    return Left = Left - Right;
}

COVERAGE(HMM_SubV3Assign, 1)
static inline Vec3 &operator-=(Vec3 &Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_SubV3Assign);
    return Left = Left - Right;
}

COVERAGE(HMM_SubV4Assign, 1)
static inline Vec4 &operator-=(Vec4 &Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_SubV4Assign);
    return Left = Left - Right;
}

COVERAGE(HMM_SubM2Assign, 1)
static inline Mat2 &operator-=(Mat2 &Left, Mat2 Right)
{
    ASSERT_COVERED(HMM_SubM2Assign);
    return Left = Left - Right;
}

COVERAGE(HMM_SubM3Assign, 1)
static inline Mat3 &operator-=(Mat3 &Left, Mat3 Right)
{
    ASSERT_COVERED(HMM_SubM3Assign);
    return Left = Left - Right;
}

COVERAGE(HMM_SubM4Assign, 1)
static inline Mat4 &operator-=(Mat4 &Left, Mat4 Right)
{
    ASSERT_COVERED(HMM_SubM4Assign);
    return Left = Left - Right;
}

COVERAGE(HMM_SubQAssign, 1)
static inline Quat &operator-=(Quat &Left, Quat Right)
{
    ASSERT_COVERED(HMM_SubQAssign);
    return Left = Left - Right;
}

COVERAGE(HMM_MulV2Assign, 1)
static inline Vec2 &operator*=(Vec2 &Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_MulV2Assign);
    return Left = Left * Right;
}

COVERAGE(HMM_MulV3Assign, 1)
static inline Vec3 &operator*=(Vec3 &Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_MulV3Assign);
    return Left = Left * Right;
}

COVERAGE(HMM_MulV4Assign, 1)
static inline Vec4 &operator*=(Vec4 &Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_MulV4Assign);
    return Left = Left * Right;
}

COVERAGE(HMM_MulV2FAssign, 1)
static inline Vec2 &operator*=(Vec2 &Left, float Right)
{
    ASSERT_COVERED(HMM_MulV2FAssign);
    return Left = Left * Right;
}

COVERAGE(HMM_MulV3FAssign, 1)
static inline Vec3 &operator*=(Vec3 &Left, float Right)
{
    ASSERT_COVERED(HMM_MulV3FAssign);
    return Left = Left * Right;
}

COVERAGE(HMM_MulV4FAssign, 1)
static inline Vec4 &operator*=(Vec4 &Left, float Right)
{
    ASSERT_COVERED(HMM_MulV4FAssign);
    return Left = Left * Right;
}

COVERAGE(HMM_MulM2FAssign, 1)
static inline Mat2 &operator*=(Mat2 &Left, float Right)
{
    ASSERT_COVERED(HMM_MulM2FAssign);
    return Left = Left * Right;
}

COVERAGE(HMM_MulM3FAssign, 1)
static inline Mat3 &operator*=(Mat3 &Left, float Right)
{
    ASSERT_COVERED(HMM_MulM3FAssign);
    return Left = Left * Right;
}

COVERAGE(HMM_MulM4FAssign, 1)
static inline Mat4 &operator*=(Mat4 &Left, float Right)
{
    ASSERT_COVERED(HMM_MulM4FAssign);
    return Left = Left * Right;
}

COVERAGE(HMM_MulQFAssign, 1)
static inline Quat &operator*=(Quat &Left, float Right)
{
    ASSERT_COVERED(HMM_MulQFAssign);
    return Left = Left * Right;
}

COVERAGE(HMM_DivV2Assign, 1)
static inline Vec2 &operator/=(Vec2 &Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_DivV2Assign);
    return Left = Left / Right;
}

COVERAGE(HMM_DivV3Assign, 1)
static inline Vec3 &operator/=(Vec3 &Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_DivV3Assign);
    return Left = Left / Right;
}

COVERAGE(HMM_DivV4Assign, 1)
static inline Vec4 &operator/=(Vec4 &Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_DivV4Assign);
    return Left = Left / Right;
}

COVERAGE(HMM_DivV2FAssign, 1)
static inline Vec2 &operator/=(Vec2 &Left, float Right)
{
    ASSERT_COVERED(HMM_DivV2FAssign);
    return Left = Left / Right;
}

COVERAGE(HMM_DivV3FAssign, 1)
static inline Vec3 &operator/=(Vec3 &Left, float Right)
{
    ASSERT_COVERED(HMM_DivV3FAssign);
    return Left = Left / Right;
}

COVERAGE(HMM_DivV4FAssign, 1)
static inline Vec4 &operator/=(Vec4 &Left, float Right)
{
    ASSERT_COVERED(HMM_DivV4FAssign);
    return Left = Left / Right;
}

COVERAGE(HMM_DivM4FAssign, 1)
static inline Mat4 &operator/=(Mat4 &Left, float Right)
{
    ASSERT_COVERED(HMM_DivM4FAssign);
    return Left = Left / Right;
}

COVERAGE(HMM_DivQFAssign, 1)
static inline Quat &operator/=(Quat &Left, float Right)
{
    ASSERT_COVERED(HMM_DivQFAssign);
    return Left = Left / Right;
}

COVERAGE(HMM_EqV2Op, 1)
static inline HMM_Bool operator==(Vec2 Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_EqV2Op);
    return HMM_EqV2(Left, Right);
}

COVERAGE(HMM_EqV3Op, 1)
static inline HMM_Bool operator==(Vec3 Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_EqV3Op);
    return HMM_EqV3(Left, Right);
}

COVERAGE(HMM_EqV4Op, 1)
static inline HMM_Bool operator==(Vec4 Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_EqV4Op);
    return HMM_EqV4(Left, Right);
}

COVERAGE(HMM_EqV2OpNot, 1)
static inline HMM_Bool operator!=(Vec2 Left, Vec2 Right)
{
    ASSERT_COVERED(HMM_EqV2OpNot);
    return !HMM_EqV2(Left, Right);
}

COVERAGE(HMM_EqV3OpNot, 1)
static inline HMM_Bool operator!=(Vec3 Left, Vec3 Right)
{
    ASSERT_COVERED(HMM_EqV3OpNot);
    return !HMM_EqV3(Left, Right);
}

COVERAGE(HMM_EqV4OpNot, 1)
static inline HMM_Bool operator!=(Vec4 Left, Vec4 Right)
{
    ASSERT_COVERED(HMM_EqV4OpNot);
    return !HMM_EqV4(Left, Right);
}

COVERAGE(HMM_UnaryMinusV2, 1)
static inline Vec2 operator-(Vec2 In)
{
    ASSERT_COVERED(HMM_UnaryMinusV2);

    Vec2 Result;
    Result.x = -In.x;
    Result.y = -In.y;

    return Result;
}

COVERAGE(HMM_UnaryMinusV3, 1)
static inline Vec3 operator-(Vec3 In)
{
    ASSERT_COVERED(HMM_UnaryMinusV3);

    Vec3 Result;
    Result.x = -In.x;
    Result.y = -In.y;
    Result.z = -In.z;

    return Result;
}

COVERAGE(HMM_UnaryMinusV4, 1)
static inline Vec4 operator-(Vec4 In)
{
    ASSERT_COVERED(HMM_UnaryMinusV4);

    Vec4 Result;
#if HANDMADE_MATH__USE_SSE
    Result.SSE = _mm_xor_ps(In.SSE, _mm_set1_ps(-0.0f));
#else
    Result.x = -In.x;
    Result.y = -In.y;
    Result.z = -In.z;
    Result.w = -In.w;
#endif

    return Result;
}

#endif /* __cplusplus*/

#ifdef HANDMADE_MATH__USE_C11_GENERICS
#define HMM_Add(A, B) _Generic((A), \
        vec2: HMM_AddV2, \
        vec3: HMM_AddV3, \
        vec4: HMM_AddV4, \
        mat2: HMM_AddM2, \
        mat3: HMM_AddM3, \
        mat4: HMM_AddM4, \
        HMM_Quat: HMM_AddQ \
)(A, B)

#define HMM_Sub(A, B) _Generic((A), \
        vec2: HMM_SubV2, \
        vec3: HMM_SubV3, \
        vec4: HMM_SubV4, \
        mat2: HMM_SubM2, \
        mat3: HMM_SubM3, \
        mat4: HMM_SubM4, \
        HMM_Quat: HMM_SubQ \
)(A, B)

#define HMM_Mul(A, B) _Generic((B), \
     float: _Generic((A), \
        vec2: HMM_MulV2F, \
        vec3: HMM_MulV3F, \
        vec4: HMM_MulV4F, \
        mat2: HMM_MulM2F, \
        mat3: HMM_MulM3F, \
        mat4: HMM_MulM4F, \
        HMM_Quat: HMM_MulQF \
     ), \
     mat2: HMM_MulM2, \
     mat3: HMM_MulM3, \
     mat4: HMM_MulM4, \
     HMM_Quat: HMM_MulQ, \
     default: _Generic((A), \
        vec2: HMM_MulV2, \
        vec3: HMM_MulV3, \
        vec4: HMM_MulV4, \
        mat2: HMM_MulM2V2, \
        mat3: HMM_MulM3V3, \
        mat4: HMM_MulM4V4 \
    ) \
)(A, B)

#define HMM_Div(A, B) _Generic((B), \
     float: _Generic((A), \
        mat2: HMM_DivM2F, \
        mat3: HMM_DivM3F, \
        mat4: HMM_DivM4F, \
        vec2: HMM_DivV2F, \
        vec3: HMM_DivV3F, \
        vec4: HMM_DivV4F, \
        HMM_Quat: HMM_DivQF  \
     ), \
     mat2: HMM_DivM2, \
     mat3: HMM_DivM3, \
     mat4: HMM_DivM4, \
     HMM_Quat: HMM_DivQ, \
     default: _Generic((A), \
        vec2: HMM_DivV2, \
        vec3: HMM_DivV3, \
        vec4: HMM_DivV4  \
    ) \
)(A, B)

#define HMM_Len(A) _Generic((A), \
        vec2: HMM_LenV2, \
        vec3: HMM_LenV3, \
        vec4: HMM_LenV4  \
)(A)

#define HMM_LenSqr(A) _Generic((A), \
        vec2: HMM_LenSqrV2, \
        vec3: HMM_LenSqrV3, \
        vec4: HMM_LenSqrV4  \
)(A)

#define HMM_Norm(A) _Generic((A), \
        vec2: HMM_NormV2, \
        vec3: HMM_NormV3, \
        vec4: HMM_NormV4  \
)(A)

#define HMM_Dot(A, B) _Generic((A), \
        vec2: HMM_DotV2, \
        vec3: HMM_DotV3, \
        vec4: HMM_DotV4  \
)(A, B)

#define HMM_Lerp(A, T, B) _Generic((A), \
        float: HMM_Lerp, \
        vec2: HMM_LerpV2, \
        vec3: HMM_LerpV3, \
        vec4: HMM_LerpV4 \
)(A, T, B)

#define HMM_Eq(A, B) _Generic((A), \
        vec2: HMM_EqV2, \
        vec3: HMM_EqV3, \
        vec4: HMM_EqV4  \
)(A, B)

#define HMM_Transpose(M) _Generic((M), \
        mat2: HMM_TransposeM2, \
        mat3: HMM_TransposeM3, \
        mat4: HMM_TransposeM4  \
)(M)

#define HMM_Determinant(M) _Generic((M), \
        mat2: HMM_DeterminantM2, \
        mat3: HMM_DeterminantM3, \
        mat4: HMM_DeterminantM4  \
)(M)

#define HMM_InvGeneral(M) _Generic((M), \
        mat2: HMM_InvGeneralM2, \
        mat3: HMM_InvGeneralM3, \
        mat4: HMM_InvGeneralM4  \
)(M)

#endif

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif /* HANDMADE_MATH_H */



