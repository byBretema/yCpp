#include <span>

#define BEE_INCLUDE_ARGPARSE
#define BEE_INCLUDE_FMT
#define BEE_INCLUDE_GLM
#define BEE_IMPLEMENTATION
#include <bee.hpp>

int main(int argc, char *argv[]) {
    fmt::println("");

    //---------------------------------
    //--- ARGS
    //---------------------------------

    // Define
    auto &cli = bee::cli_init("Test", "0.0.1", "Built @byBretema");
    cli.add_argument("Num").help("Meaning of life").scan<'d', int32_t>();

    // Parse
    if (!bee::cli_parse(cli, argc, argv)) {
        return -1;
    }

    // Gather values
    int32_t num = cli.get<int32_t>("Num");

    //---------------------------------
    //--- MAIN
    //---------------------------------

    fmt::println("The input value was: {}", num);
}
