#ifndef SCENARIO_CONSTANTS_HPP
#define SCENARIO_CONSTANTS_HPP

#include "utils.hpp"


constexpr const char *YAML_SECTION_OBJECTS = "objects";
constexpr const char *YAML_SECTION_EVENTS  = "events";
constexpr const char *YAML_SECTION_MAPS    = "maps";
//constexpr const char *YAML_
//constexpr const char *DEFAULT_PARSE_ERROR

constexpr const char *DEFAULT_PARSE_ERROR  = "YAML configuration does not match the scenario specification.";
constexpr const char *DEFAULT_PLAYER_ID    = "player";
constexpr const char *DEFAULT_MAP_ID       = "map";
constexpr const char *RESERVED_WINDOW_ID   = "window";
constexpr const char *RESERVED_SCENARIO_ID = "scenario";


constexpr const int    DWARF_VISION_RANGE = 10;
constexpr const char  *DWARF_OBSTACLES    = "~#";
constexpr const char  *DWARF_UNVISIBLE    = "#";
constexpr const char   DWARF_SYMBOL       = '@';
constexpr const attr_t DWARF_ATTR         = A_BOLD;

#endif // SCENARIO_CONSTANTS_HPP
