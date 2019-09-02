/* This file is part of Walker.
 * 
 * Walker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Walker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Walker.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef SCENARIO_CONSTANTS_HPP
#define SCENARIO_CONSTANTS_HPP

#include "images.hpp"
#include "utils.hpp"
#include "window.hpp"

constexpr const char *YAML_SECTION_OBJECTS = "objects";
constexpr const char *YAML_SECTION_EVENTS  = "events";
constexpr const char *YAML_SECTION_MAPS    = "maps";

constexpr const char *YAML_OBJECT_TYPE       = "type";
constexpr const char *YAML_OBJECT_POSITION_X = "x";
constexpr const char *YAML_OBJECT_POSITION_Y = "y";

constexpr const char *YAML_EVENT_CONDITIONS     = "if";
constexpr const char *YAML_EVENT_MESSAGE        = "message";
constexpr const char *YAML_EVENT_TITLE          = "title";
constexpr const char *YAML_EVENT_SIZE           = "size";
constexpr const char *YAML_EVENT_ATTRIBUTE      = "color";
constexpr const char *YAML_EVENT_IMAGE          = "image";
constexpr const char *YAML_EVENT_IMAGE_FILE     = "file";
constexpr const char *YAML_EVENT_ITEMS          = "items";
constexpr const char *YAML_EVENT_COMMANDS       = "do";
constexpr const char *YAML_EVENT_ITEM_LABEL     = "label";
constexpr const char *YAML_EVENT_ITEM_COMMANDS  = "do";

constexpr const char *YAML_MAP_WIDTH  = "width";
constexpr const char *YAML_MAP_HEIGHT = "height";
constexpr const char *YAML_MAP_TEXT   = "text";

constexpr const char    *DEFAULT_PARSE_ERROR       = "YAML configuration does not match the scenario specification.";
constexpr position       DEFAULT_EVENT_SIZE        = POSITION_SMALL;
constexpr attr_t         DEFAULT_EVENT_ATTRIBUTE   = PAIR(NEUTRAL_COLOR, COLOR_BLACK);
constexpr const char    *DEFAULT_PLAYER_ID         = "player";
constexpr const char    *DEFAULT_MAP_ID            = "map";
constexpr int            DEFAULT_TILE_ATTRIBUTE    = A_INVIS;

constexpr const char *RESERVED_SCENARIO_ID = "scenario";
constexpr const char *RESERVED_DIALOG_ID = "dialog";

constexpr const char *YAML_IMAGE_MOUNTAINS = "Mountains";
constexpr const char *YAML_IMAGE_COW       = "Cow";
constexpr const char *YAML_IMAGE_CENTAUR   = "Centaur";
constexpr const char *YAML_IMAGE_PIKEMAN   = "Pikeman";

constexpr const char *YAML_WINDOW_ATTRIBUTE_HOSTILE  = "Hostile";
constexpr const char *YAML_WINDOW_ATTRIBUTE_NEUTRAL  = "Neutral";
constexpr const char *YAML_WINDOW_ATTRIBUTE_FRIEND   = "Friend";

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
