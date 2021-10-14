#pragma once

// Macro helpers
#define global_variable static
#define local_variable static

#define BEARD_COMPILER_MSVC 0
#define BEARD_COMPILER_CLANG 0
#define BEARD_COMPILER_GCC 0

// Clang is first to avoid problems with clang on windows
#if defined(__clang__)
#	undef BEARD_COMPILER_CLANG
#	define BEARD_COMPILER_CLANG (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#elif defined(_MSC_VER)
#	undef BEARD_COMPILER_MSVC
#	define BEARD_COMPILER_MSVC _MSC_VER
#elif defined(__GNUC__)
#	undef BEARD_COMPILER_GCC
#	define BEARD_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#	error "Undefined compiler"
#endif

#define BEARD_PLATFORM_WINDOWS 0
#define BEARD_PLATFORM_LINUX 0
#define BEARD_PLATFORM_MACOS 0 // TODO

#if defined(_WIN32) || defined(_WIN64)
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#	undef BEARD_PLATFORM_WINDOWS
#	define BEARD_PLATFORM_WINDOWS 1
#elif defined(__linux__)
#	undef BEARD_PLATFORM_LINUX
#	define BEARD_PLATFORM_LINUX 1
#else
#	error "Undefined platform"
#endif

#include <stdint.h>
#if UINTPTR_MAX == 0xffffffffffffffff
#	define BEARD_ARCH64
#elif UINTPTR_MAX == 0xffffffff
#	define BEARD_ARCH32
#else
#	error "Unsupported architecture"
#endif

#define BEARD_DEBUG 0
#define BEARD_RELWITHDEBINFO 0
#define BEARD_RELEASE 0

#if defined(_DEBUG)
#	undef BEARD_DEBUG
#	define BEARD_DEBUG 1
#elif defined(_RELWITHDEBINFO)
#	undef BEARD_RELWITHDEBINFO
#	define BEARD_RELWITHDEBINFO 1
#else
#	undef BEARD_RELEASE
#	define BEARD_RELEASE 1
#endif

#define BEARD_UNUSED(x) ((void)x)

#if BEARD_COMPILER_GCC || BEARD_COMPILER_CLANG
#	define BEARD_ALIGN(x, a) x __attribute__((aligned(a))
#	define BEARD_BREAKPOINT __asm__ volatile("int $0x03")
#	define BEARD_LIKELY(x) __builtin_expect(!!(x), 1)
#	define BEARD_NO_VTABLE
#elif BEARD_COMPILER_MSVC
#	define BEARD_ALIGN(x, a) __declspec(align(a)) x
#	define BEARD_BREAKPOINT __debugbreak()
#	define BEARD_LIKELY(x) (x)
#	define BEARD_NO_VTABLE __declspec(novtable)
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

// Types
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using isize = intptr_t;
using usize = uintptr_t;

using f32 = float;
using f64 = double;

#if defined(USE_DOUBLE)
using real = f64;
#else
using real = f32;
#endif

static constexpr usize KB = 1024;
static constexpr usize MB = 1024 * KB;
static constexpr usize GB = 1024 * MB;

static constexpr usize KiB = 1000;
static constexpr usize MiB = 1000 * KiB;
static constexpr usize GiB = 1000 * MiB;

// clang-format off
inline constexpr usize operator"" _KB(usize size) { return size * KB; }
inline constexpr usize operator"" _MB(usize size) { return size * MB; }
inline constexpr usize operator"" _GB(usize size) { return size * GB; }

inline constexpr usize operator"" _KiB(usize size) { return size * KiB; }
inline constexpr usize operator"" _MiB(usize size) { return size * MiB; }
inline constexpr usize operator"" _GiB(usize size) { return size * GiB; }
// clang-format on

#define UNUSED(x) (void)(x)

#define NONCOPYABLE(T)                                                                                                 \
	T(const T&)  = delete;                                                                                             \
	void operator=(const T&) = delete
#define NONMOVEABLE(T)                                                                                                 \
	T(T&&) noexcept = delete;                                                                                          \
	void operator=(T&&) noexcept = delete

#define DEFAULT_COPYABLE(T)                                                                                            \
	T(const T&) = default;                                                                                             \
	T& operator=(const T&) = default

#define DEFAULT_MOVEABLE(T)                                                                                            \
	T(T&&) noexcept = default;                                                                                         \
	T& operator=(T&&) noexcept = default

#define DEFAULT_CTORS(T)                                                                                               \
	DEFAULT_COPYABLE(T);                                                                                               \
	DEFAULT_MOVEABLE(T)

#if BEARD_PLATFORM_WINDOWS
#	define DEBUGBREAK __debugbreak()
#else
// TODO: Handle other cases
#	define DEBUGBREAK                                                                                                 \
		int* trap = reinterpret_cast<int*>(3L);                                                                        \
		*trap     = 3
#endif

#if BEARD_PLATFORM_WINDOWS
#	define START_EXTERNAL_INCLUDE __pragma(warning(push)) __pragma(warning(disable : 4201))
#	define END_EXTERNAL_INCLUDE __pragma(warning(pop))

#	define VARIABLE_LENGTH_ARRAY(type, name)                                                                          \
		__pragma(warning(push));                                                                                       \
		__pragma(warning(disable : 4200));                                                                             \
		type name[0];                                                                                                  \
		__pragma(warning(pop))
#else
#	define START_EXTERNAL_INCLUDE
#	define END_EXTERNAL_INCLUDE

#	define VARIABLE_LENGTH_ARRAY(type, name) type name[]
#endif

#define NOOP(...)

#ifdef _DEBUG
#	define ASSERT(x, msg, ...)                                                                                        \
		__pragma(warning(push));                                                                                       \
		__pragma(warning(disable : 4002));                                                                             \
		if (!(x))                                                                                                      \
		{                                                                                                              \
			fprintf(stderr, "Assertion `%s` failed (%s:%d): \"" msg "\"", #x, __FILE__, __LINE__, __VA_ARGS__);        \
			DEBUGBREAK;                                                                                                \
		}                                                                                                              \
		__pragma(warning(pop))

#	define ASSERT_UNREACHABLE() ASSERT(false, "Unreachable path")
#else
#	define ASSERT(x) NOOP(x)
#	define ASSERT_UNREACHABLE() NOOP()
#endif

// https://www.gingerbill.org/article/2015/08/19/defer-in-cpp
// clang-format off
namespace Priv
{
template <typename Fn>
struct PrivDefer
{
	Fn fn;

	PrivDefer(Fn fn) : fn(fn) {}
	~PrivDefer() { fn(); }
};

template <typename Fn>
PrivDefer<Fn> DeferFunc(Fn fn) { return PrivDefer<Fn>(fn); }
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)
#define Defer(code) auto DEFER_3(_defer_) = Priv::DeferFunc([&]() { code; })
// clang-format on

#include <math.h>
#include <numbers>
#include <stdio.h>

namespace Beard
{

// Math stuff
namespace F32
{
	constexpr f32 Pi   = std::numbers::pi_v<f32>;
	constexpr f32 Pi_2 = Pi * 0.5f;
	constexpr f32 Pi_3 = Pi / 3.0f;
	constexpr f32 Pi_4 = Pi / 4.0f;
	constexpr f32 Pi_6 = Pi / 6.0f;
	constexpr f32 Pi_8 = Pi / 8.0f;
	constexpr f32 Tau  = Pi * 2.0f;

	constexpr f32 DegToRad = Pi / 180.0f;
	constexpr f32 RadToDeg = 180.0f / Pi;

	constexpr f32 Sqrt_2 = std::numbers::sqrt2_v<f32>;
	constexpr f32 E      = std::numbers::e_v<f32>;
	constexpr f32 Phi    = std::numbers::phi_v<f32>;
}

namespace F64
{
	constexpr f64 Pi   = std::numbers::pi_v<f64>;
	constexpr f64 Pi_2 = Pi * 0.5;
	constexpr f64 Pi_3 = Pi / 3.0;
	constexpr f64 Pi_4 = Pi / 4.0;
	constexpr f64 Pi_6 = Pi / 6.0;
	constexpr f64 Pi_8 = Pi / 8.0;
	constexpr f64 Tau  = Pi * 2.0;

	constexpr f64 DegToRad = Pi / 180.0;
	constexpr f64 RadToDeg = 180.0 / Pi;

	constexpr f64 Sqrt_2 = std::numbers::sqrt2_v<f64>;
	constexpr f64 E      = std::numbers::e_v<f64>;
	constexpr f64 Phi    = std::numbers::phi_v<f64>;
}

// Alias Math with F32 of F64 depending on the Real type
#if defined(USE_DOUBLE)
namespace Math = F64;
#else
namespace Math = F32;
#endif

// clang-format off
template <typename T> inline T Min(const T a, const T b) { return a < b ? a : b; }
template <typename T> inline T Max(const T a, const T b) { return a > b ? a : b; }
template <typename T> inline T ClampBot(const T x, const T a) { return Max(x, a); }
template <typename T> inline T ClampTop(const T x, const T a) { return Min(x, a); }
template <typename T> inline T Clamp(const T x, const T a, const T b) { return ClampBot(a, ClampTop(x, b)); }
template <typename T> inline T Saturate(const T x) { return Clamp(x, T{0}, T{1}); }

template <typename T> inline T Abs(const T x) { return abs(x); }
template <> inline f32 Abs(const f32 x) { return fabsf(x); }
template <> inline f64 Abs(const f64 x) { return fabs(x); }

inline f32 Sine(const f32 x) { return sinf(x); }
inline f32 Cosine(const f32 x) { return cosf(x); }
inline f32 Tangent(const f32 x) { return tanf(x); }
inline f32 ArcSine(const f32 x) { return asinf(x); }
inline f32 ArcCosine(const f32 x) { return acosf(x); }
inline f32 ArcTangent(const f32 x) { return atanf(x); }
inline f32 Pow(const f32 x, const f32 e) { return powf(x, e); }
inline f32 Sqrt(const f32 x) { return sqrtf(x); }
inline f32 ToRadians(const f32 x) { return x * F32::DegToRad; }
inline f32 ToDegrees(const f32 x) { return x * F32::RadToDeg; }

inline f64 Sine(const f64 x) { return sin(x); }
inline f64 Cosine(const f64 x) { return cos(x); }
inline f64 Tangent(const f64 x) { return tan(x); }
inline f64 ArcSine(const f64 x) { return asin(x); }
inline f64 ArcCosine(const f64 x) { return acos(x); }
inline f64 ArcTangent(const f64 x) { return atan(x); }
inline f64 Pow(const f64 x, const f64 e) { return pow(x, e); }
inline f64 Sqrt(const f64 x) { return sqrt(x); }
inline f64 ToRadians(const f64 x) { return x * F64::DegToRad; }
inline f64 ToDegrees(const f64 x) { return x * F64::RadToDeg; }
// clang-format on

} // namespace Beard