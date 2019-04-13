#ifndef SCENARIO_CONSTANTS_HPP
#define SCENARIO_CONSTANTS_HPP

#include "utils.hpp"
#include "window.hpp"

constexpr const char *YAML_SECTION_OBJECTS = "objects";
constexpr const char *YAML_SECTION_EVENTS  = "events";
constexpr const char *YAML_SECTION_MAPS    = "maps";

constexpr const char *YAML_OBJECT_TYPE       = "type";
constexpr const char *YAML_OBJECT_POSITION_X = "x";
constexpr const char *YAML_OBJECT_POSITION_Y = "y";

constexpr const char *YAML_EVENT_CONDITIONS    = "if";
constexpr const char *YAML_EVENT_MESSAGE       = "message";
constexpr const char *YAML_EVENT_TITLE         = "title";
constexpr const char *YAML_EVENT_SIZE          = "size";
constexpr const char *YAML_EVENT_ITEMS         = "items";
constexpr const char *YAML_EVENT_COMMANDS      = "do";
constexpr const char *YAML_EVENT_ITEM_LABEL    = "label";
constexpr const char *YAML_EVENT_ITEM_COMMANDS = "do";

constexpr const char *YAML_MAP_WIDTH  = "width";
constexpr const char *YAML_MAP_HEIGHT = "height";
constexpr const char *YAML_MAP_TEXT   = "text";

constexpr const char *DEFAULT_PARSE_ERROR  = "YAML configuration does not match the scenario specification.";
constexpr const char *DEFAULT_EVENT_TITLE  = "Event";
constexpr position    DEFAULT_EVENT_SIZE   = POSITION_SMALL;
constexpr const char *DEFAULT_PLAYER_ID    = "player";
constexpr const char *DEFAULT_MAP_ID       = "map";

constexpr const char *RESERVED_WINDOW_ID   = "window";
constexpr const char *RESERVED_SCENARIO_ID = "scenario";

constexpr const char *YAML_WINDOW_SIZE_SMALL   = "Small";
constexpr const char *YAML_WINDOW_SIZE_AVERAGE = "Average";
constexpr const char *YAML_WINDOW_SIZE_FULL    = "Full";

constexpr const char  *DWARF_TYPE         = "Dwarf";
constexpr int          DWARF_VISION_RANGE = 10;
constexpr const char  *DWARF_OBSTACLES    = "~#";
constexpr const char  *DWARF_UNVISIBLE    = "#";
constexpr const char   DWARF_SYMBOL       = '@';
constexpr const attr_t DWARF_ATTR         = A_BOLD;

#endif // SCENARIO_CONSTANTS_HPP
