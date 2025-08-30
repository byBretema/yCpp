#include <span>

#define yyEnable_Aliases
#define yyLib_Argparse
#define yyLib_Glm
#include <y.hpp>


i32 main(i32 argc, char *argv[]) {
    y_println("");

    auto &cli = y::cli_init("Test", "0.0.1", "Built @byBretema");
    cli.add_argument("Num").help("Meaning of life").scan<'d', i32>();
    y_or_return(y::cli_parse(cli, argc, argv), -1);

    i32 const num = cli.get<i32>("Num");
    y_println("The input value was: {}", num);
}
