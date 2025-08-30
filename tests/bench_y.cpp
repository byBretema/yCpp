
#define yyCustom_Fmt
#define yyEnable_Aliases
#define yyEnable_Benchmarking
#include "../vendor/y.hpp"

int main() {
    y::Benchmark B;
    usize executions = 5;
    B.set_align_column(42);

    B.run("StdCout", executions, [] {});
}
