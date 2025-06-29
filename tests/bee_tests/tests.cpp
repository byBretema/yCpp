
#define BEE_TEST_IMPLEMENTATION
#include "../helpers/bee_test.hpp"

#define BEE_BENCH_IMPLEMENTATION
// #define BEE_BENCH_STDOUT_ONCE
#include "../helpers/bee_bench.hpp"

#define BEE_IMPLEMENTATION
#define BEE_USE_FAKE_FMT
// #define BEE_INCLUDE_FMT
// #define BEE_CPP_INCLUDE_GLM
#include "../../vendor/bee.hpp"

#include <iostream>


using namespace bee::TypeAlias_GLM;
using namespace bee::TypeAlias_Numbers;
using namespace bee::TypeAlias_Pointers;
using namespace bee::TypeAlias_Containers;


// ############################################################################
// #                                                                          #
// #                                                                          #
// #                                   TESTS                                  #
// #                                                                          #
// #                                                                          #
// ############################################################################


// ==============================================
// ========== Defer

TEST("Defer Ref", {
    i32 defer_count = 0;
    {
        defer(defer_count += 2);
        CHECK("Before defer", defer_count == 0);
    }
    CHECK("After defer", defer_count == 2);
});

TEST("Defer Copy", {
    static i32 static_defer_count = 0;
    i32 defer_count = 3;
    {
        deferc(static_defer_count += defer_count);
        CHECK("Before defer", static_defer_count == 0);
    }
    CHECK("After defer", static_defer_count == 3);
});


// ==============================================
// ========== Format

#if defined(BEE_INCLUDE_FMT) || defined(BEE_USE_FAKE_FMT)
TEST_CHECK("String Format (Str)", bee_fmt("Test {}", "String") == "Test String");
TEST_CHECK("String Format (Int)", bee_fmt("Test {}", 42) == "Test 42");
TEST_CHECK("String Format (Float)", bee_fmt("Test {}", 3.14159f) == "Test 3.14159");
TEST_CHECK("String Format (Double)", bee_fmt("Test {}", 3.14159) == "Test 3.14159");
#endif

// ==============================================
// ========== Bit operations

TEST("Bit Operations", {
    CHECK("Bit 1", bee_bit(1) == 2);
    CHECK("Bit 2", bee_bit(2) == 4);
    CHECK("Bit 3", bee_bit(3) == 8);
    CHECK("Bit 4", bee_bit(4) == 16);
    CHECK("Bit 5", bee_bit(5) == 32);
});


// ==============================================
// ========== Cast

TEST("Cast Macro", {
    CHECK("As i8 (clamp)", as(i8, 3.14159) == 3);
    CHECK("As i8 (size)", sizeof(as(i8, 3.14159)) == sizeof(i8));
    CHECK("As f32", sizeof(as(f32, 3.14159)) == sizeof(f32));

    i32 *heap_i = new i32(5);
    CHECK("As void*", typeid(as(void *, heap_i)) == typeid(void *));
    delete heap_i;
});


// ==============================================
// ========== Num Aliases

TEST("Numeric Aliases", {
    CHECK("u8 min", u8_min == std::numeric_limits<uint8_t>::min());
    CHECK("u8 max", u8_max == std::numeric_limits<uint8_t>::max());
    CHECK("u16 min", u16_min == std::numeric_limits<uint16_t>::min());
    CHECK("u16 max", u16_max == std::numeric_limits<uint16_t>::max());
    CHECK("u32 min", u32_min == std::numeric_limits<uint32_t>::min());
    CHECK("u32 max", u32_max == std::numeric_limits<uint32_t>::max());
    CHECK("u64 min", u64_min == std::numeric_limits<uint64_t>::min());
    CHECK("u64 max", u64_max == std::numeric_limits<uint64_t>::max());
    CHECK("usize min", usize_min == std::numeric_limits<size_t>::min());
    CHECK("usize max", usize_max == std::numeric_limits<size_t>::max());

    CHECK("i8 min", i8_min == std::numeric_limits<int8_t>::min());
    CHECK("i8 max", i8_max == std::numeric_limits<int8_t>::max());
    CHECK("i16 min", i16_min == std::numeric_limits<int16_t>::min());
    CHECK("i16 max", i16_max == std::numeric_limits<int16_t>::max());
    CHECK("i32 min", i32_min == std::numeric_limits<int32_t>::min());
    CHECK("i32 max", i32_max == std::numeric_limits<int32_t>::max());
    CHECK("i64 min", i64_min == std::numeric_limits<int64_t>::min());
    CHECK("i64 max", i64_max == std::numeric_limits<int64_t>::max());
    CHECK("isize min", isize_min == std::numeric_limits<ptrdiff_t>::min());
    CHECK("isize max", isize_max == std::numeric_limits<ptrdiff_t>::max());

    CHECK("f32 min", f32_min == std::numeric_limits<float>::min());
    CHECK("f32 max", f32_max == std::numeric_limits<float>::max());
    CHECK("f32 epsilon", f32_epsilon == std::numeric_limits<float>::epsilon());

    CHECK("f64 min", f64_min == std::numeric_limits<double>::min());
    CHECK("f64 max", f64_max == std::numeric_limits<double>::max());
    CHECK("f64 epsilon", f64_epsilon == std::numeric_limits<double>::epsilon());
});


// ==============================================
// ========== Pointer Aliases

TEST("TypeAlias Pointers", {
    struct A {
        Str s = "A";
        f32 f = 0.f;
    };
    auto const a = A { "A", 3.14159f };
    Uptr<A> ua = Unew<A>("A", 3.14159f);
    CHECK("Uptr", a.s == ua->s && a.f == ua->f);
    Sptr<A> sa = Snew<A>("A", 3.14159f);
    CHECK("Sptr", a.s == sa->s && a.f == sa->f);
});


// ==============================================
// ========== Optionals

TEST("Optional Reference", {
    struct A {
        i32 i = 0;
    };
    auto a = A { 42 };

    auto const fn_value = [](A &received_a) -> OptRef<A> { return received_a; };
    auto opt = fn_value(a);
    CHECK("Has Value", opt.has_value());
    CHECK("Check Value", opt.value().get().i == 42);
    auto const fn_null = [](A &received_a) -> OptRef<A> { return {}; };
    CHECK("Not Has Value", !fn_null(a).has_value());
});


// ==============================================
// ========== Time stuff

TEST("Elapsed Timer", {
    using namespace std::chrono_literals;
    bee::ETimer timer {};
    CHECK("Init Invalid 1", !timer.is_valid());
    CHECK("Init Invalid 2", timer.elapsed_ns() * timer.is_valid() == 0);
    timer.reset();
    std::this_thread::sleep_for(10ms);
    CHECK("After Reset", timer.elapsed_ms() * timer.is_valid() > 9);
});


// ==============================================
// ========== String helpers/operations

TEST("String Helpers", {
    Str const to_case = "test STRING to PERFORM the tests";
    CHECK("To Lower", bee::str_lower(to_case) == "test string to perform the tests");
    CHECK("To Upper", bee::str_upper(to_case) == "TEST STRING TO PERFORM THE TESTS");
    CHECK("To Capital", bee::str_capital(to_case) == "Test string to perform the tests");

    Str const to_replace = "1,2,3,4,5";
    CHECK("Replace All", bee::str_replace(to_replace, ",", " / ") == "1 / 2 / 3 / 4 / 5");
    CHECK("Replace First", bee::str_replace(to_replace, ",", " / ", true) == "1 / 2,3,4,5");

    Str const to_replace_many = "1.2-3:4·5";
    Str const to_replace_many_ok = "1[1] 2[2] 3[2] 4[4] 5";
    Vec<Str> from = { "-", ".", "·", ":" };
    Vec<Str> to = { "[2] ", "[1] ", "[4] ", "[3] " };
    CHECK("Replace Many Unsorted", bee::str_replace_many(to_replace_many, from, to) != to_replace_many_ok);
    from = { ".", "-", ":", "·" };
    to = { "[1] ", "[2] ", "[3] ", "[4] " };
    CHECK("Replace Many Sorted", bee::str_replace_many(to_replace_many, from, to, true) != to_replace_many_ok);

    Str const to_split = "1,2,3,4,5";
    Vec<Str> const splitted = { "1", "2", "3", "4", "5" };
    CHECK("Split", bee::str_split(to_split, ",") == splitted);
    CHECK("Join", bee::str_join(splitted, ",") == to_split);

    CHECK("Cut", bee::str_cut(" / a / b / c / ", 3) == "a / b / c");
    CHECK("Cut L", bee::str_cut_l(" / a / b / c", 3) == "a / b / c");
    CHECK("Cut R", bee::str_cut_r("a / b / c / ", 3) == "a / b / c");

    CHECK("Contains", bee::str_contains("a / b / c / ", " b "));

    CHECK("Trim", bee::str_trim(" aaa ") == "aaa");
    CHECK("Trim L", bee::str_trim_l(" aaa ") == "aaa ");
    CHECK("Trim R", bee::str_trim_r(" aaa ") == " aaa");

    // bee_info("==> {}", bee::str_trim("***aaa***", "***"));
    // CHECK("Trim Not Space", bee::str_trim("***aaa***", "***") == "aaa");
});


// ==============================================
// ========== File helpers/operations

TEST("File/Bin Helpers", {
    Str const file_content = bee::file_read("./to_file_read.txt");
    Str const expected_content = "Test\nfile\nfor\nBEE\n";
    CHECK("Read", file_content == expected_content);

    auto const t = std::time(nullptr);
    auto const tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
    Str const str = oss.str();
    CHECK("Append", bee::file_write_append("./to_file_append.txt", str + "\n"));
    Str const append_content = bee::file_read("./to_file_append.txt");
    Vec<Str> const append_split = bee::str_split(append_content, "\n");
    CHECK("Append Validation", append_split[append_split.size() - 1] == str);

    Vec<u8> const bin { 'T', 'e', 's', 't', '\n', 'D', 'a', 't', 'a' };
    CHECK("Write", bee::file_write_trunc("./to_file_write.bin", recast(char const *, bin.data()), bin.size()));
    CHECK("Write Validation", bee::fs::exists("./to_file_write.bin"));

    auto const bin_content = bee::bin_read("./to_file_write.bin");
    Vec<u8> const magic { 'T', 'e', 's', 't' };
    CHECK("Magic", bee::bin_check_magic(bin_content, magic));

    CHECK("Extension", bee::file_check_extension("./to_file_write.bin", "BiN"));
});


// ############################################################################
// #                                                                          #
// #                                                                          #
// #                                BENCHMARKS                                #
// #                                                                          #
// #                                                                          #
// ############################################################################


// ==============================================
// ========== Constants

inline int32_t BENCH_COUNT = 5;


// ==============================================
// ========== Cout vs Print

BENCH("StdCout", BENCH_COUNT, {
    std::cout << std::boolalpha << "[INFO] | " << __FILE__ << ":" << __LINE__ //
              << " | " << "2 elevated to " << 1 << " is "                     //
              << bee_bit(1) << " == " << true << "\n";
});

#if defined(BEE_USE_FAKE_FMT)
BENCH("Info (fakefmt)", BENCH_COUNT, bee_info("2 elevated to {} is {} == {}", 1, bee_bit(1), true));
#elif defined(BEE_INCLUDE_FMT)
BENCH("Info (fmtlib)", BENCH_COUNT, bee_info("2 elevated to {} is {} == {}", 1, bee_bit(1), true));
#else
BENCH("Info (apped)", BENCH_COUNT, bee_info("2 elevated to {} is {} == {}", 1, bee_bit(1), true));
#endif


// ==============================================
// ========== String replacement

BENCH("Str Replace Many Unsorted", BENCH_COUNT,
      Str s = bee::str_replace_many("1.2-3:4·5", Vec<Str> { "-", ".", "·", ":" },
                                    Vec<Str> { "[2] ", "[1] ", "[4] ", "[3] " }));
BENCH("Str Replace Many Sorted", BENCH_COUNT,
      Str s = bee::str_replace_many("1.2-3:4·5", Vec<Str> { ".", "-", ":", "·" },
                                    Vec<Str> { "[1] ", "[2] ", "[3] ", "[4] " }));


// ==============================================
// ========== Glm stuff

#ifdef BEE_INCLUDE_GLM
BENCH("bee_info_glm_vec3", 5, bee_info("glm vec3 {}", glmstr(Vec3(2.f))));
#endif


// ############################################################################
// #                                                                          #
// #                                                                          #
// #                               ENTRY POINT                                #
// #                                                                          #
// #                                                                          #
// ############################################################################

int main() {
    bee::test::run();
    bee_print("{}", "");
    bee::bench::run();
}
