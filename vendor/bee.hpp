#pragma once

/* bee - v0.0.1

    An alternative take on cpp.
    Basically some aliases and helpers.

    Also a bunch of code that I found myself repeating between projects.

    No warranty implied, use at your own risk.

    Original Repository ->  https://github.com/byBretema/bee

    =============================================
    ! How to include
    =============================================

    -- Classic header-only stuff, add this:

    #define BEE_IMPLEMENTATION

    -- Before you include this file in *one* C++ file to create the
    implementation, something like this:

    #include ...
    #include ...
    #define BEE_IMPLEMENTATION
    #include "bee.hpp"

    =============================================
    ! Define-Based options:
    =============================================

    -- Expose all the aliases defined by the 'bee' library
    (i32, f32, Vec, Arr, Str, Sptr, Uptr, Vec2, Vec3, ...)

    #define BEE_EXPOSE_ALIASES


    -- If you use fmt-lib, 'bee' will include basic fmt header file(s) and
    expose, basic log methods: bee_info/warn/err/debug("", ...),
    this will also undefine 'BEE_USE_FAKE_FMT'

    #define BEE_INCLUDE_FMT


    -- If you use argparse, 'bee' will include argparse header file(s) and
    expose some wrapper methods under bee::cli_xxx, namespace

    #define BEE_INCLUDE_ARGPARSE


    -- If you use glm-lib, 'bee' will include basic glm header files

    #define BEE_INCLUDE_GLM


    -- Use a naïve fmt-like custom implemenation (will be disabled if
    'BEE_INCLUDE_FMT' is present)

    #define BEE_USE_FAKE_FMT
*/


// ############################################################################
/* #region Bee-Header */
// ############################################################################


//=============================================================================
//= INCLUDES
//=============================================================================

//--- STD ---------------------------------------------------------------------

#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>

#include <array>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <thread>

#include <algorithm>
#include <functional>
#include <memory>

#include <filesystem>

#include <iso646.h>

//--- SPAN --------------------------------------------------------------------

#if __cplusplus >= 202002L
#include <span>
#else
#define TCB_SPAN_NAMESPACE_NAME std
#include "tcb_span.hpp"
#endif

//--- ARGs --------------------------------------------------------------------

#ifdef BEE_INCLUDE_ARGPARSE
//! https://github.com/p-ranav/argparse?tab=readme-ov-file#table-of-contents
#include <argparse/argparse.hpp>
#endif

//--- FMT ---------------------------------------------------------------------

#ifdef BEE_INCLUDE_FMT
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/std.h>
#endif

//--- GLM ---------------------------------------------------------------------

#ifdef BEE_INCLUDE_GLM
// #define GLM_FORCE_SSE
// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_SILENT_WARNINGS
#define GLM_FORCE_RADIANS
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtx/string_cast.hpp> // Before of 'GLM_FORCE_INLINE'
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/gtx/vector_angle.hpp>
#define glmstr(x) glm::to_string(x)
#endif


//=============================================================================
//= CONCAT
//=============================================================================

#ifndef BEE_CONCAT
#define __BEE_CONCAT2(l, r) l##r
#define __BEE_CONCAT1(l, r) __BEE_CONCAT2(l, r)
#define BEE_CONCAT(l, r) __BEE_CONCAT1(l, r)
#endif

//=============================================================================
//= DEFER
//=============================================================================

#ifndef defer
#define defer(fn) const auto BEE_CONCAT(defer__, __LINE__) = bee::details::Defer([&]() { fn; })
#else
#warning "[bee] :: 'defer' is already defined using it might end in a missbehave"
#endif

#ifndef deferc
#define deferc(fn) const auto BEE_CONCAT(defer__, __LINE__) = bee::details::Defer([=]() { fn; })
#else
#warning "[bee] :: 'deferc' is already defined using it might end in a missbehave"
#endif

namespace bee::details {
template <typename T> // Faster compilation than std::function : Credits to @javiersalcedopuyo
class Defer {
public:
    Defer() = delete;
    Defer(T fn) : fn(fn) {}
    ~Defer() { fn(); }

private:
    const T fn;
};
} // namespace bee::details


//=============================================================================
//= FORMAT and PRINT
//=============================================================================

//--- FMT Or Fake -------------------------------------------------------------

#ifdef BEE_INCLUDE_FMT //  <-- Using fmtlib
#undef BEE_USE_FAKE_FMT
// String Builder
#define bee_fmt(msg, ...) fmt::format(msg, __VA_ARGS__)
// Log Builder
#define __BEE_LOG(level, msg, ...)                                                                                     \
    fmt::println("[{}] | {}:{} | {}", level, __FILE__, __LINE__, bee_fmt(msg, __VA_ARGS__))
#define __BEE_LOG_FLAT(msg, ...) fmt::println("{}", bee_fmt(msg, __VA_ARGS__))

#else //  <-- Not using fmtlib (rely on std::cout)

#include <iostream>
#include <regex>

#ifdef _WIN32
#include <windows.h>
static const int ___BEE_COUT_SETUP = []() {
    SetConsoleOutputCP(CP_UTF8);
    return 0;
}();
#endif

namespace bee::detail {

template <typename... Args>
inline std::vector<std::string> format_getlist(Args &&...args) {

    std::vector<std::string> str_list;
    str_list.reserve(sizeof...(args));

    static auto oss = []() {
        std::ostringstream oss;
        oss << std::boolalpha;
        return oss;
    }();

    using fake_loop = int[];
    (void)fake_loop { 0, (oss.str(""),                      //  Clear prev arg
                          oss << std::forward<Args>(args),  //  Arg to string
                          str_list.emplace_back(oss.str()), //  Place arg into the list
                          0)... };                          //  Just for int return

    return str_list;
}

inline std::string format(std::string const &msg, std::vector<std::string> const &args) {
    // Trying to capture fmt mini-language
    static const std::regex pattern(R"(\{:?.?:?[^}^ ]*\})", std::regex_constants::optimize);

    std::ostringstream oss;
    oss.str().reserve(msg.size() + args.size() * 8); // Size estimation

    size_t last = 0;
    auto arg_it = args.begin();
    std::sregex_iterator it(msg.begin(), msg.end(), pattern), end;

    for (; it != end && arg_it != args.end(); ++it, ++arg_it) {
        oss.write(&msg[last], it->position() - last);
        oss << *arg_it;
        last = it->position() + it->length();
    }

    oss.write(&msg[last], msg.size() - last);
    return oss.str();
}

} // namespace bee::detail

// String Builder
#define bee_fmt(msg, ...) bee::detail::format(msg, bee::detail::format_getlist(__VA_ARGS__))

// Log Builder
#define __BEE_LOG(level, msg, ...)                                                                                     \
    std::cout << "[" << level << "] | " << __FILE__ << ":" << __LINE__ << " | " << bee_fmt(msg, __VA_ARGS__) << "\n"
#define __BEE_LOG_FLAT(msg, ...) std::cout << bee_fmt(msg, __VA_ARGS__) << "\n"
#endif

//--- Actual print API --------------------------------------------------------

#define bee_print(msg, ...) __BEE_LOG_FLAT(msg, __VA_ARGS__)
#define bee_info(msg, ...) __BEE_LOG("INFO", msg, __VA_ARGS__)
#define bee_warn(msg, ...) __BEE_LOG("WARN", msg, __VA_ARGS__)
#define bee_err(msg, ...) __BEE_LOG("ERRO", msg, __VA_ARGS__)
#define bee_debug(msg, ...) __BEE_LOG("DEBG", msg, __VA_ARGS__)


//=============================================================================
//= OTHER MACROS
//=============================================================================

//--- Misc --------------------------------------------------------------------

#define bee_bind(fn)                                                                                                   \
    [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define bee_bit(x) (1 << x)

//--- Class helpers -----------------------------------------------------------

#define bee_nocopy(T)                                                                                                  \
public:                                                                                                                \
    T(T const &) = delete;                                                                                             \
    T &operator=(T const &) = delete;

#define bee_nomove(T)                                                                                                  \
public:                                                                                                                \
    T(T &&) noexcept = delete;                                                                                         \
    T &operator=(T &&) noexcept = delete;

#define bee_nocopy_nomove(T) bee_nocopy(T) bee_nomove(T)

//--- Cast helpers ------------------------------------------------------------

#ifndef as
#define as(T, x) static_cast<T>(x)
#else
#warning "[bee] :: 'as' is already defined using it might end in a missbehave"
#endif

#ifndef recast
#define recast(T, x) ((T)(x))
#else
#warning "[bee] :: 'recast' is already defined using it might end in a missbehave"
#endif

//=============================================================================
//= NAMESPACE
//=============================================================================

namespace bee {


//--- Numbers Aliases ---------------------------------------------------------

namespace TypeAlias_Numbers {

// Bool
using b8 = bool;

// Unsigned
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using usize = size_t;
inline constexpr u8 u8_min = std::numeric_limits<u8>::min();
inline constexpr u8 u8_max = std::numeric_limits<u8>::max();
inline constexpr u16 u16_min = std::numeric_limits<u16>::min();
inline constexpr u16 u16_max = std::numeric_limits<u16>::max();
inline constexpr u32 u32_min = std::numeric_limits<u32>::min();
inline constexpr u32 u32_max = std::numeric_limits<u32>::max();
inline constexpr u64 u64_min = std::numeric_limits<u64>::min();
inline constexpr u64 u64_max = std::numeric_limits<u64>::max();
inline constexpr usize usize_min = std::numeric_limits<usize>::min();
inline constexpr usize usize_max = std::numeric_limits<usize>::max();

// Signed
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using isize = ptrdiff_t;
inline constexpr i8 i8_min = std::numeric_limits<i8>::min();
inline constexpr i8 i8_max = std::numeric_limits<i8>::max();
inline constexpr i16 i16_min = std::numeric_limits<i16>::min();
inline constexpr i16 i16_max = std::numeric_limits<i16>::max();
inline constexpr i32 i32_min = std::numeric_limits<i32>::min();
inline constexpr i32 i32_max = std::numeric_limits<i32>::max();
inline constexpr i64 i64_min = std::numeric_limits<i64>::min();
inline constexpr i64 i64_max = std::numeric_limits<i64>::max();
inline constexpr isize isize_min = std::numeric_limits<isize>::min();
inline constexpr isize isize_max = std::numeric_limits<isize>::max();

// Floating point
using f32 = float;
using f64 = double;
inline constexpr f32 f32_min = std::numeric_limits<f32>::min();
inline constexpr f32 f32_max = std::numeric_limits<f32>::max();
inline constexpr f32 f32_epsilon = std::numeric_limits<f32>::epsilon();
inline constexpr f64 f64_min = std::numeric_limits<f64>::min();
inline constexpr f64 f64_max = std::numeric_limits<f64>::max();
inline constexpr f64 f64_epsilon = std::numeric_limits<f64>::epsilon();

} // namespace TypeAlias_Numbers
using namespace TypeAlias_Numbers;


//--- Pointers Aliases --------------------------------------------------------

namespace TypeAlias_Pointers {

// Unique pointer
template <typename T>
using Uptr = std::unique_ptr<T>;
template <typename T, typename... Args>
[[nodiscard]] constexpr Uptr<T> Unew(Args &&...args) {
    // return std::make_unique<T>(std::forward<Args>(args)...); // C++20 might support this
    return Uptr<T>(new T { args... }); // Below C++20
}

// Shared pointer
template <typename T>
using Sptr = std::shared_ptr<T>;
template <typename T, typename... Args>
[[nodiscard]] constexpr Sptr<T> Snew(Args &&...args) {
    // return std::make_shared<T>(std::forward<Args>(args)...); // C++20 might support this
    return Sptr<T>(new T { args... }); // Below C++20
}

} // namespace TypeAlias_Pointers
using namespace TypeAlias_Pointers;


//--- Containers Aliases --------------------------------------------------------

namespace TypeAlias_Containers {

// Unordered Map
template <typename K, typename V>
using Umap = std::unordered_map<K, V>;

// Ordered Map
template <typename K, typename V>
using Omap = std::map<K, V>;

// Unordered Set
template <typename T>
using Uset = std::unordered_set<T>;

// Ordered Set
template <typename T>
using Oset = std::set<T>;

// Dynamic Array
template <typename T>
using Vec = std::vector<T>;

// Array
template <typename T, size_t S>
using Arr = std::array<T, S>;

// Optional
template <typename T>
using Opt = std::optional<T>;
template <typename T>
using OptRef = std::optional<std::reference_wrapper<T>>;

// String
using Str = std::string;

// Function
template <typename T>
using Fn = std::function<T>;

// Span
template <typename T>
using Span = std::span<T>;
template <typename T>
using SpanConst = std::span<const T>;

} // namespace TypeAlias_Containers
using namespace TypeAlias_Containers;


//--- GLM Aliases -------------------------------------------------------------

#ifdef BEE_INCLUDE_GLM

namespace TypeAlias_GLM {
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;
} // namespace TypeAlias_GLM
#else
namespace TypeAlias_GLM {} // namespace TypeAlias_GLM
#endif
using namespace TypeAlias_GLM;


//--- Time Consts -------------------------------------------------------------

inline constexpr f64 s_to_ms = 1e+3;
inline constexpr f64 s_to_us = 1e+6;
inline constexpr f64 s_to_ns = 1e+9;

inline constexpr f64 ms_to_s = 1e-3;
inline constexpr f64 ms_to_us = 1e+3;
inline constexpr f64 ms_to_ns = 1e+6;

inline constexpr f64 us_to_s = 1e-6;
inline constexpr f64 us_to_ms = 1e-3;
inline constexpr f64 us_to_ns = 1e+3;

inline constexpr f64 ns_to_s = 1e-9;
inline constexpr f64 ns_to_ms = 1e-6;
inline constexpr f64 ns_to_us = 1e-3;


//--- Argparse ----------------------------------------------------------------

#ifdef BEE_INCLUDE_ARGPARSE
using CLI = argparse::ArgumentParser;
[[nodiscard]] CLI &cli_init(Str const &title, Str const &version, Str const &description);
[[nodiscard]] bool cli_parse(CLI &cli, int argc, char *argv[]);
#endif


//--- Elapsed Timer -------------------------------------------------------------

class ElapsedTimer {
public:
    void reset();
    [[nodiscard]] f64 elapsed_s() const;
    [[nodiscard]] f64 elapsed_ms() const;
    [[nodiscard]] f64 elapsed_us() const;
    [[nodiscard]] f64 elapsed_ns() const;
    [[nodiscard]] b8 is_valid() const;

private:
    [[nodiscard]] i64 elapsed() const;

private:
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point m_ref = Clock::now();
    b8 m_valid = false;
};
using ETimer = ElapsedTimer;


//--- String Utils ------------------------------------------------------------

[[nodiscard]] Str str_lower(Str str);
[[nodiscard]] Str str_upper(Str str);
[[nodiscard]] Str str_capital(Str str);

[[nodiscard]] b8 str_contains(Str const &str, Str const &substr);
[[nodiscard]] Vec<Str> str_split(Str const &str, Str const &delimeter);
[[nodiscard]] Str str_join(Vec<Str> const &strlist, Str const &delimeter);
[[nodiscard]] Str str_replace(Str str, Str const &from, Str const &to, b8 only_first_match = false);
[[nodiscard]] Str str_replace_many(Str str, Vec<Str> const &from, Vec<Str> const &to, b8 sorted = false);

[[nodiscard]] Str str_cut(Str const &str, i32 count);
[[nodiscard]] Str str_cut_l(Str const &str, i32 count);
[[nodiscard]] Str str_cut_r(Str const &str, i32 count);

[[nodiscard]] Str str_trim(Str str, Str const &individual_chars_to_remove = " \n\r\t");
[[nodiscard]] Str str_trim_l(Str str, Str const &individual_chars_to_remove = " \n\r\t");
[[nodiscard]] Str str_trim_r(Str str, Str const &individual_chars_to_remove = " \n\r\t");


//--- Binary Utils ------------------------------------------------------------

[[nodiscard]] Vec<u8> bin_read(Str const &path);
[[nodiscard]] b8 bin_check_magic(SpanConst<u8> bin, SpanConst<u8> magic);


//--- Files Utils -------------------------------------------------------------

[[nodiscard]] Str file_read(Str const &input_file);

b8 file_write_append(Str const &output_file, Str const &to_write);
b8 file_write_trunc(Str const &output_file, Str const &to_write);

b8 file_write_append(Str const &output_file, const char *data, usize data_size);
b8 file_write_trunc(Str const &output_file, const char *data, usize data_size);

b8 file_check_extension(Str const &input_file, Str ext);


//--- Math Utils ------------------------------------------------------------

[[nodiscard]] f32 map(f32 value, f32 src_min, f32 src_max, f32 dst_min, f32 dst_max);
[[nodiscard]] f32 map_100(f32 value, f32 dst_min, f32 dst_max);

[[nodiscard]] b8 fuzzy_eq(f32 f1, f32 f2, f32 threshold = 0.01f);

[[nodiscard]] f32 clamp_angle(f32 angle);

#ifdef BEE_INCLUDE_GLM
[[nodiscard]] b8 fuzzy_eq(Vec2 const &v1, Vec2 const &v2, f32 t = 0.01f);
[[nodiscard]] b8 fuzzy_eq(Vec3 const &v1, Vec3 const &v2, f32 t = 0.01f);
[[nodiscard]] b8 fuzzy_eq(Vec4 const &v1, Vec4 const &v2, f32 t = 0.01f);

template <typename T>
[[nodiscard]] inline b8 is_aligned(T const &a, T const &b, f32 margin = 0.f) {
    return abs(glm::dot(glm::normalize(a), glm::normalize(b))) >= (1.f - f32_epsilon - margin);
}
#endif

} // namespace bee


//=============================================================================
//= ALIASES
//=============================================================================

#ifdef BEE_EXPOSE_ALIASES
using namespace bee::TypeAlias_Containers;
using namespace bee::TypeAlias_Pointers;
using namespace bee::TypeAlias_Numbers;
using namespace bee::TypeAlias_GLM;
#endif


// ############################################################################
/* #endregion Bee-Header*/
// ############################################################################


// ############################################################################
/* #region Bee-Implementation */
// ############################################################################


#ifdef BEE_IMPLEMENTATION

#ifndef __BEE_IMPLEMENTATION_GUARD
#define __BEE_IMPLEMENTATION_GUARD

#include <fstream>

namespace bee {
namespace fs = std::filesystem;


//=============================================================================
//= Argparse
//=============================================================================

#ifdef BEE_INCLUDE_ARGPARSE
CLI &cli_init(Str const &title, Str const &version, Str const &description) {
    static CLI cli { title, version };
    static auto once = [&description] {
        cli.add_description(description);
        return 0;
    }();
    return cli;
}
bool cli_parse(CLI &cli, int argc, char *argv[]) {
    try {
        cli.parse_args(argc, argv);
        return true;
    } catch (const std::exception &err) {
        fmt::println("{}", err.what());
        fmt::println("{}", cli.help().str());
        return false;
    }
}
#endif


//=============================================================================
//= Elapsed Timer
//=============================================================================

void ElapsedTimer::reset() {
    m_valid = true;
    m_ref = Clock::now();
}
f64 ElapsedTimer::elapsed_s() const { return as(f64, elapsed()) * ns_to_s; }
f64 ElapsedTimer::elapsed_ms() const { return as(f64, elapsed()) * ns_to_ms; }
f64 ElapsedTimer::elapsed_us() const { return as(f64, elapsed()) * ns_to_us; }
f64 ElapsedTimer::elapsed_ns() const { return as(f64, elapsed()); }
b8 ElapsedTimer::is_valid() const { return m_valid; }
i64 ElapsedTimer::elapsed() const {
    auto const now = Clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_ref).count();
}


//=============================================================================
//= String Utils
//=============================================================================

Str str_lower(Str str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}
Str str_upper(Str str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}
Str str_capital(Str str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::transform(str.begin(), str.begin() + 1, str.begin(), ::toupper);
    return str;
}

b8 str_contains(Str const &str, Str const &substr) { return str.find(substr) < str.size(); }

Vec<Str> str_split(Str const &str, Str const &delimeter) {

    Str token;
    Vec<Str> splitted;
    usize ini = 0;
    usize end = 0;

    // Split and store the string body
    while ((end = str.find(delimeter, ini)) < str.size()) {
        token = str.substr(ini, end - ini);
        ini = end + delimeter.size();
        splitted.push_back(token);
    }

    // Store the string tail
    if (ini < str.size()) {
        token = str.substr(ini);
        splitted.push_back(token);
    }

    return splitted;
}

Str str_join(Vec<Str> const &strlist, Str const &delimeter) {

    if (strlist.empty()) {
        return "";
    }

    Str s;
    for (usize i = 0; i < strlist.size() - 1; ++i) {
        s += strlist[i] + delimeter;
    }
    s += strlist[strlist.size() - 1];

    return s;
}

Str str_replace(Str str, Str const &from, Str const &to, b8 only_first_match) {
    usize pos = 0;
    while ((pos = str.find(from)) < str.size()) {
        str.replace(pos, from.length(), to);
        if (only_first_match) {
            break;
        }
    }
    return str;
}

Str str_replace_many(Str str, Vec<Str> const &from, Vec<Str> const &to, b8 sorted) {

    b8 const same_size = from.size() == to.size();
    b8 const is_empty = same_size && from.size() < 1;
    if (!same_size || is_empty) {
        if (!same_size) {
            assert(0);
        }
        return str;
    }

    usize pos = 0;
    usize i = 0;

    usize anchor = 0;
    while (i < from.size() && (pos = str.find(from[i], anchor)) < str.size()) {
        str.replace(pos, from[i].length(), to[i]);
        if (sorted) {
            anchor = pos;
        }
        ++i;
    }
    return str;
}

Str str_cut(Str const &str, i32 count) { return str_cut_r(str_cut_l(str, count), count); }
Str str_cut_l(Str const &str, i32 count) { return str.substr(count); }
Str str_cut_r(Str const &str, i32 count) { return str.substr(0, std::max(i32(str.size()) - count, count)); }

Str str_trim(Str str, Str const &individual_chars_to_remove) {
    auto const l = str_trim_l(str, individual_chars_to_remove);
    return str_trim_r(l, individual_chars_to_remove);
}
Str str_trim_l(Str str, Str const &individual_chars_to_remove) {
    return str.erase(0, str.find_first_not_of(individual_chars_to_remove));
}
Str str_trim_r(Str str, Str const &individual_chars_to_remove) {
    return str.erase(str.find_last_not_of(individual_chars_to_remove) + 1);
}


//=============================================================================
//= Binary Utils
//=============================================================================

Vec<u8> bin_read(Str const &path) {
    std::ifstream file { path, std::ios::binary };
    auto file_begin = std::istreambuf_iterator<char>(file);
    auto file_end = std::istreambuf_iterator<char>();
    return { file_begin, file_end };
}

b8 bin_check_magic(SpanConst<u8> bin, SpanConst<u8> magic) {
    // Validation
    if (magic.empty() || bin.size() < magic.size()) {
        return false;
    }
    // Iteration
    b8 match = true;
    for (usize i = 0; i < magic.size(); ++i) {
        match &= (bin[i] == magic[i]);
    }
    // Result
    return match;
}


//=============================================================================
//= Files Utils
//=============================================================================

Str file_read(Str const &input_file) {
    std::ifstream file(input_file, std::ios::ate | std::ios::binary);
    defer(file.close());

    if (!file.is_open()) {
        return "";
        bee_err("Issues opening file [r]: {}", input_file);
    }

    Str content;
    content.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&content[0], content.size());

    return content;
}

b8 file_write(Str const &output_file, char const *data, usize data_size, std::ios_base::openmode mode) {
    if (!data || data_size < 1) {
        return false;
        bee_err("[file_write] Invalid data: {}", output_file);
    }

    std::ofstream file(output_file, std::ios::out | std::ios::binary | mode);
    defer(file.close());

    if (!file.is_open()) {
        return false;
        bee_err("[file_write] Opening file: {}", output_file);
    }

    file.write(data, data_size);

    return true;
}
b8 file_write_append(Str const &output_file, Str const &to_write) {
    return file_write(output_file, to_write.data(), to_write.size(), std::ios::app);
}
b8 file_write_trunc(Str const &output_file, Str const &to_write) {
    return file_write(output_file, to_write.data(), to_write.size(), std::ios::trunc);
}
b8 file_write_append(Str const &output_file, const char *data, usize data_size) {
    return file_write(output_file, data, data_size, std::ios::app);
}
b8 file_write_trunc(Str const &output_file, const char *data, usize data_size) {
    return file_write(output_file, data, data_size, std::ios::trunc);
}

b8 file_check_extension(Str const &input_file, Str ext) {
    auto to_check = input_file.substr(input_file.find_last_of('.') + 1);
    std::transform(to_check.begin(), to_check.end(), to_check.begin(), ::tolower);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return to_check == ext;
}


//=============================================================================
//= Math Utils
//=============================================================================

f32 map(f32 value, f32 src_min, f32 src_max, f32 dst_min, f32 dst_max) {
    return dst_min + (dst_max - dst_min) * (value - src_min) / (src_max - src_min);
}
f32 map_100(f32 value, f32 dst_min, f32 dst_max) { return map(value, 0, 100, dst_min, dst_max); }

b8 fuzzy_eq(f32 f1, f32 f2, f32 threshold) {
    auto const diff = abs(f1 - f2);
    auto const is_eq = diff <= threshold;
    return is_eq;
}

f32 clamp_angle(f32 angle) {
    auto const turns = floorf(angle / 360.f);
    return angle - 360.f * turns;
}

#ifdef BEE_INCLUDE_GLM
b8 fuzzy_eq(Vec2 const &v1, Vec2 const &v2, f32 t) { return fuzzy_eq(v1.x, v2.x, t) && fuzzy_eq(v1.y, v2.y, t); }
b8 fuzzy_eq(Vec3 const &v1, Vec3 const &v2, f32 t) {
    return fuzzy_eq(v1.x, v2.x, t) && fuzzy_eq(v1.y, v2.y, t) && fuzzy_eq(v1.z, v2.z, t);
}
b8 fuzzy_eq(Vec4 const &v1, Vec4 const &v2, f32 t) {
    return fuzzy_eq(v1.x, v2.x, t) && fuzzy_eq(v1.y, v2.y, t) && fuzzy_eq(v1.z, v2.z, t) && fuzzy_eq(v1.w, v2.w, t);
}
#endif

} // namespace bee

#endif // __BEE_IMPLEMENTATION_GUARD
#endif // BEE_IMPLEMENTATION


// ############################################################################
/* #endregion Bee-Implementation */
// ############################################################################
