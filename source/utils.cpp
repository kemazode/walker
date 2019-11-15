#include "utils.hpp"
#include <utility>

std::string CONFIG = "/.config/walker/"; // cannot be fully initialized in static scope

const char *DIR_SCENARIOS = "scenarios/";
const char *DIR_GENERATIONS = "generations/";

game_error::~game_error() = default;
