#include <span>

#define BEE_INCLUDE_ARGPARSE
#define BEE_INCLUDE_FMT
#define BEE_INCLUDE_GLM
#define BEE_EXPOSE_ALIASES
#define BEE_IMPLEMENTATION
#include <bee.hpp>


int main(int argc, char *argv[]) {
    fmt::println("");


    //-------------------------------------------------------------------------
    //----- ARGS

    auto &cli = bee::cli_init("Test", "0.0.1", "Built @byBretema");
    cli.add_argument("Num").help("Meaning of life").scan<'d', int32_t>();

    if (!bee::cli_parse(cli, argc, argv)) {
        return -1;
    }

    i32 const num = cli.get<i32>("Num");


    //-------------------------------------------------------------------------
    //----- PROCESS

    fmt::println("The input value was: {}", num);
}
