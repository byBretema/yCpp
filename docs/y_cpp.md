# y.hpp API Reference

**y.hpp** is an opinionated, single-header C++ wrapper over standard utilities, argparse, fmt, and glm.

<br>

## Integration

1. Define configuration macros (optional) before including.
2. Include the header: `#include <y.hpp>`

<br>

## Configuration Macros

Define these macros **before** including the header to enable/disable features.

### Feature Switches

| Macro            | Description                                               |
| ---------------- | --------------------------------------------------------- |
| `yyLib_Argparse` | Includes `argparse.hpp` and exposes `y::cli_xxx` methods. |
| `yyLib_Glm`      | Includes GLM headers and exposes vector/matrix aliases.   |

### Behavior Switches

| Macro                       | Description                                             |
| --------------------------- | ------------------------------------------------------- |
| `yyEnable_Aliases`          | Exposes types (u32, Vec, Str, etc) to the global scope. |
| `yyEnable_Testing`          | Enables the `y::Test` class.                            |
| `yyEnable_Benchmarking`     | Enables the `y::Benchmark` class.                       |
| `yyEnable_PrintFileAndLine` | Adds file/line info to `y_print` calls.                 |
| `yyDisable_LogFileAndLine`  | Hides file/line info in logs (`y_info`, `y_warn`, etc). |
| `yyDisable_Log`             | Disables all logging macros completely.                 |

<br>

## Logging & Helper Macros

### Logging

```cpp
y_print(...);  // Standard print
y_println(...); // Print with newline
y_info(...);   // Log [INFO]
y_warn(...);   // Log [WARN]
y_err(...);    // Log [ERRO]
y_debug(...);  // Log [DEBG]
```

### Flow Control & Classes

```cpp
// Tries to mimic `or_return` idiom of Odin
y_or_return(cond, ret_val);

// Defers execution of code block until scope exit
y_defer(code);   // Internal lambda captures by ref
y_deferc(code);  // Internal lambda captures by copy

// Class helpers to delete constructors
y_class_nocopy(ClassType);
y_class_nomove(ClassType);
y_class_nocopynomove(ClassType);
y_class_move(ClassType, move_code);  // 'move_code' will be place inside noexcept swap.
```

<br>

## Type Aliases

If `yyEnable_Aliases` is defined, these are available globally. Otherwise, they are in `y::`.

### Namespaces

| Alias | Namespace         |
| ----- | ----------------- |
| `fs`  | `std::filesystem` |

### Numerics

| Alias             | Type                                  | Min/Max Constants          | Other Constants     |
| ----------------- | ------------------------------------- | -------------------------- | ------------------- |
| `b8`              | `bool`                                |                            |                     |
| `u8`..`u64`       | `uint8_t`..`uint64_t`                 | `u8_min`, `u8_max`, etc.   |                     |
| `i8`..`i64`       | `int8_t`..`int64_t`                   | `i8_min`, `i8_max`, etc.   |                     |
| `f32`, `f64`      | `float`, `double`                     | `f32_min`, `f32_max`, etc. | `f32_epsilon`, etc. |
| `usize`           | `size_t`                              | `usize_min`, `usize_max`   |                     |
| `isize`           | `ptrdiff_t`                           | `isize_min`, `isize_max`   |                     |
| `ull / ill / fll` | `(un)/signed long long / double long` |                            |                     |

### STL Containers & Smart Pointers

| Alias                    | Type                                       | Helper             |
| ------------------------ | ------------------------------------------ | ------------------ |
| `fs::`                   | `std::filesystem`                          |                    |
| `Uptr<T>`                | `std::unique_ptr<T>`                       | `y::u_new<T>(...)` |
| `Sptr<T>`                | `std::shared_ptr<T>`                       | `y::s_new<T>(...)` |
| `Umap<K,V>`, `Omap<K,V>` | Unordered/Ordered Map                      |                    |
| `Uset<K,V>`, `Oset<K,V>` | Unordered/Ordered Set                      |                    |
| `Vec<T>`                 | `std::vector<T>`                           |                    |
| `Arr<T, S>`              | `std::array<T, S>`                         |                    |
| `Opt<T>`                 | `std::optional<T>`                         |                    |
| `OptRef<T>`              | `std::optional<std::reference_wrapper<T>>` |                    |
| `Str`, `StrView`         | `std::string`, `std::string_view`          |                    |
| `Fn<T>`                  | `std::function<void()>`                    |                    |
| `VoidFn<T>`              | `std::function<bool()>`                    |                    |
| `BoolFn<T>`              | `std::function<T>`                         |                    |
| `Span<T>`                | `std::span<T>`                             |                    |
| `SpanConst<T>`           | `std::span<const T>`                       |                    |
| `Clock`                  | `std::chrono::high_resolution_clock`       |                    |
| `TimePoint`              | `Clock::time_point`                        |                    |

### GLM &nbsp;&nbsp;_(If `yyLib_Glm` defined)_

| Alias                  | Type                                  |
| ---------------------- | ------------------------------------- |
| `Vec2`, `Vec3`, `Vec4` | `glm::vec2`, `glm::vec3`, `glm::vec4` |
| `Mat4`                 | `glm::mat4`                           |

<br>

## Concpets

| Name          | Evaluates                                          | Notes                       |
| ------------- | -------------------------------------------------- | --------------------------- |
| `T_OneOf`     | [MetaConcept] If T is one of the given types       | `T_OneOf<T, i32, f32, ...>` |
| `T_Container` | If type is a container (or range) + Ignore StrView |                             |
| `T_Integer`   | If type is not a float point                       |                             |
| `T_Decimal`   | If type is float point                             |                             |
| `T_Number`    | If type is either integer or float                 |                             |
| `T_CharList`  | If type is `Vec<u8>` or a range of `const char *`  |                             |
| `T_MathVec`   | If type is from GLM a `Vec2`, `Vec3` or `Vec4`     | _If `yyLib_Glm` defined_    |

<br>

## Formatters

| Name          | Formats                 | Example                     |
| ------------- | ----------------------- | --------------------------- |
| `T_Container` | Any contiguos container | `{1,3,5,7,9}`               |
| `T_MathVec`   | Any GLM vector types    | `Vec3(0.345, 0.123, 0.789)` |

<br>

## Resource Management

### `Defer`

Executes a callback when the object goes out of scope.

- **Constructor:** `Defer(T &&callback)`
- Mainly to be used through `y_defer` and `y_deferc` macros

### `LazyGC`

A lazy garbage collector that accumulates callbacks and executes them on destruction or explicit release.

- `LazyGC &add(VoidFn &&cb)`: Adds a cleanup callback.
- `void release()`: Executes all callbacks immediately.

<br>

## Command Line &nbsp;&nbsp;_(If `yyLib_Argparse` defined)_

- Initializes the static argument parser instance.

  ```cpp
  CLI &cli_init(Str const &title, Str const &version, Str const &description)

  ```

- Parses command line arguments. Catches exceptions and prints help on failure.

  ```cpp
  bool cli_parse(CLI &cli, int argc, char *argv[])

  ```

<br>

## Time & Chrono

### Conversions / Constants

> Just multiply for those to do the conversion

- `s_to_ms`/`us`/`ns`
- `ms_to_s`/`us`/`ns`
- `us_to_s`/`ms`/`ns`
- `ns_to_s`/`ms`/`us`

### Utils

- **ElapsedTimer**

  ```cpp
  class ElapsedTimer;
    // ...
    ElapsedTimer &reset()
    f64 elapsed_s()   //<! Return elapsed as seconds
    f64 elapsed_ms()  //<! Return elapsed as milliseconds
    f64 elapsed_us()  //<! Return elapsed as microseconds
    f64 elapsed_ns()  //<! Return elapsed as nanoseconds
  ```

- Returns current time formatted as `DD-MM-YYYY HH-MM-SS`
  ```cpp
  Str time_stamp()
  ```

<br>

## String Manipulation

- Returns a copy of the string transformed to lower/upper/capitalized case.

  ```cpp
  Str str_lower(Str str)
  Str str_upper(Str str)
  Str str_capital(Str str)
  ```

- Checks if `str` contains `substr`.

  ```cpp
  bool str_contains(StrView str, StrView substr)
  ```

- Splits string by delimiter.

  ```cpp
  Vec<Str> str_split(StrView str, StrView delim)
  ```

- Joins a vector of strings into one using the delimiter.

  ```cpp
  Str str_join(Vec<Str> const &strlist, Str const &delim)
  ```

- Replaces occurrences of substrings.

  ```cpp
  Str str_replace(Str str, Str const &from, Str const &to, bool only_first_match = false)
  Str str_replace_many(Str str, Vec<Str> const &from, Vec<Str> const &to, bool sorted = false)
  ```

- Slicing and cutting utilities.

  ```cpp
  Str str_slice(Str const &str, usize from, usize to)
  Str str_cut(Str const &str, usize count)   // Cut count from both ends
  Str str_cut_l(Str const &str, usize count) // Cut count from left
  Str str_cut_r(Str const &str, usize count) // Cut count from right
  ```

- Trims whitespace (or specific characters) from ends.

  ```cpp
  Str str_trim(Str str, Str const &chars = " \n\r\t")
  Str str_trim_l(Str str, Str const &chars = " \n\r\t")
  Str str_trim_r(Str str, Str const &chars = " \n\r\t")
  ```

<br>

## Files

- Reads a file entirely into a String. Returns empty string on failure (with warning).

  ```cpp
  Str file_read(Str const &input_file)
  ```

- Writes data to file, creating directories if they don't exist.

  ```cpp
  bool file_write(Str const &output_file, char const *data, usize size, std::ios_base::openmode mode)
  ```

- Helpers to append or overwrite files using a container `T` (must satisfy `T_CharList`).

  ```cpp
  bool file_append(Str const &output_file, T const &v)
  bool file_overwrite(Str const &output_file, T const &v)
  ```

- Case-insensitive extension check.

  ```cpp
  bool file_check_extension(Str const &input_file, Str ext_ref)
  ```

<br>

## Binary

- Reads a file entirely into a byte vector.

  ```cpp
  Vec<u8> bin_read(Str const &path)
  ```

- Verifies if the binary data starts with the given magic bytes.

  ```cpp
  bool bin_check_magic(SpanConst<u8> bin, SpanConst<u8> magic)
  ```

<br>

## Math

- Clamps value between low and high.

  ```cpp
  T clamp(T v, T lo, T hi)
  ```

- Maps a value from one range to another.

  ```cpp
  T map(T value, T src_min, T src_max, T dst_min, T dst_max)
  T map_100(T value, T dst_min, T dst_max)
  ```

- Compares floating point numbers (or GLM vectors if enabled) with a threshold.

  ```cpp
  bool fuzzy_eq(T f1, T f2, T threshold = 0.01f)
  ```

- Normalizes angle to [0, 360) range.

  ```cpp
  T clamp_angle(T angle)
  ```

- Check if two glm-vectors are aligned. &nbsp;&nbsp;_(If `yyLib_Glm` defined)_

  ```cpp
  b8 is_aligned(GlmVec const &a, GlmVec const &b, f32 margin = 0.01f)
  ```

<br>

## Testing &nbsp;&nbsp;_(If `yyEnable_Testing` defined)_

> Simple unit testing framework.

```cpp
class Test;
  // ...
  void make_section(StrView name)
  void ok(StrView title, bool c)
  void eq(StrView title, T1 const &lhs, T2 const &rhs)
  void gt(StrView title, T1 const &lhs, T2 const &rhs)
  void lt(StrView title, T1 const &lhs, T2 const &rhs)
  void test(StrView title, Fn<bool()> const &fn, StrView msg = "")
  void show_results()
  i32 cli_result() // Returns 0 if all passed, -1 otherwise
```

<br>

## Benchmarking &nbsp;&nbsp;_(If `yyEnable_Benchmarking` defined)_

> Simple benchmarking utility. It disables `stdout` during execution.

```cpp
class Benchmark;
  // ...
  void run(StrView title, u32 executions, Fn<void()> const &callback)
```

<br>

## Nasty

> This is a _SubNamespace_ inside `y::` to place C++ stuff that sometimes are required but feels odd to have

- `stdout_off()` : Disables any output of the application.
- `stdout_on()` : Re-Enable the output of the application.
