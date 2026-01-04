
#define yyLib_Fmt
#define yyEnable_Aliases
#include <y.hpp>


// [[deprecated("Do not use this")]]
int foo() { return 5; }

int main() {
    auto const s = y::str_join({ "a", "b", "c" }, ",");
    y_println("{}", s);
    y_println("{}", foo());
}
