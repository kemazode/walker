#include "utils.hpp"
#include <utility>

const char *CUSTOM_CONFIG = nullptr;
const char *DEFAULT_CONFIG = ".config/walker/";
const char *DIR_SCENARIOS = "scenarios/";
const char *DIR_GENERATIONS = "generations/";
const char *HELP_MESSAGE =
        "Usage: walker [options] [-h]\n"
        "-c <dir>: set config directory (default: $HOME/.config/walker)\n"
        "-h: print this message\n";

game_error::~game_error() = default;
