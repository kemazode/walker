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

#ifndef UI_HPP
#define UI_HPP

#include "utils.hpp"
#include "window.hpp"

using W = Window;

struct Menu {
    enum
    {
        null,
        main,
        game,
        okay,
        map_creator,
        map_sizes,
    };
};

struct Hooks {
    enum
    {
        null,
        main,
        game,
        menu
    };
};

struct Builder {
    enum
    {
        main,
        game,
        game_menu,
        okay,
        map_creator,
        map_sizes,
        error,
    };
};

// Functions for working with the scenario
static inline void scenario_move_player_x(Arg);
static inline void scenario_move_player_y(Arg);
static inline void scenario_move_view_x  (Arg);
static inline void scenario_move_view_y  (Arg);
static        void scenario_generate     (Arg);
static        void scenario_load();

static inline void w_push       (Arg);
static inline void w_set        (Arg);
static inline void w_menu_driver(Arg);

static vector<vector<W::Menu>> menus =
{
    // Menu::null
    {},

    { // Menu::main
      W::Menu("Start New Game", scenario_load),
      W::Menu("Create Map",     ActionAV(w_push, Builder::map_creator)),
      W::Menu("Scoreboard"),
      W::Menu("Options"),
      W::Menu("Exit",           W::clear),
    },

    { // Menu::game
      W::Menu("Continue",       W::pop),
      W::Menu("Exit",           ActionAV(w_set, Builder::main)),
    },

    { // Menu::okay
      W::Menu("OK",            W::pop),
    },

    { // Menu::create
      W::Menu("Generate",      ActionAV(w_push, Builder::map_sizes)),
    },

    { // Menu::map_sizes
      W::Menu("100x100", ActionAV(scenario_generate, 100)),
      W::Menu("250x250", ActionAV(scenario_generate, 250)),
      W::Menu("500x500", ActionAV(scenario_generate, 500)),
    },
};

static vector<vector<W::Hook>> hooks =
{
    // Hooks::null
    {},

    { // Hooks::main
      W::Hook(KEY_DOWN, ActionAV(w_menu_driver, REQ_DOWN_ITEM)),
      W::Hook(KEY_UP,   ActionAV(w_menu_driver, REQ_UP_ITEM)),
      W::Hook('\n',     ActionAV(w_menu_driver, REQ_EXEC_ITEM))
    },

    { // Hooks::game
      W::Hook('Q',       ActionAV(w_push, Builder::game_menu)),
      W::Hook('q',       ActionAV(w_push, Builder::game_menu)),

      // Player moving
      W::Hook(KEY_DOWN,  ActionAV(scenario_move_player_y,  1)),
      W::Hook(KEY_UP,    ActionAV(scenario_move_player_y, -1)),
      W::Hook(KEY_LEFT,  ActionAV(scenario_move_player_x, -1)),
      W::Hook(KEY_RIGHT, ActionAV(scenario_move_player_x,  1)),

      // Map moving
      W::Hook('k', ActionAV(scenario_move_view_y,  1)),
      W::Hook('i', ActionAV(scenario_move_view_y, -1)),
      W::Hook('j', ActionAV(scenario_move_view_x, -1)),
      W::Hook('l', ActionAV(scenario_move_view_x,  1)),
      W::Hook('K', ActionAV(scenario_move_view_y,  1)),
      W::Hook('I', ActionAV(scenario_move_view_y, -1)),
      W::Hook('J', ActionAV(scenario_move_view_x, -1)),
      W::Hook('L', ActionAV(scenario_move_view_x,  1)),
    },

    { // Hooks::menu
      W::Hook('q',      W::pop),
      W::Hook('Q',      W::pop),
      W::Hook(KEY_DOWN, ActionAV(w_menu_driver, REQ_DOWN_ITEM)),
      W::Hook(KEY_UP,   ActionAV(w_menu_driver, REQ_UP_ITEM)),
      W::Hook('\n',     ActionAV(w_menu_driver, REQ_EXEC_ITEM))
    },
};

static vector<W::Builder> builder =
{    
    W::Builder(W::full,  menus[Menu::main],        hooks[Hooks::main], "Welcome!",             "Menu"),
    W::Builder(W::game,  menus[Menu::null],        hooks[Hooks::game], Text(),                 "", W::borderless),
    W::Builder(W::small, menus[Menu::game],        hooks[Hooks::menu], Text(),                 "Menu"),
    W::Builder(W::small, menus[Menu::okay],        hooks[Hooks::menu], Text(),                 "Message"),
    W::Builder(W::full,  menus[Menu::map_creator], hooks[Hooks::menu], "Create your own map.", "Map Creator"),
    W::Builder(W::small, menus[Menu::map_sizes],   hooks[Hooks::menu], "Choose map size:",     "Map sizes"),
    W::Builder(W::small, menus[Menu::okay],        hooks[Hooks::menu], Text(),                 "Error"),
};

void w_set(Arg arg)
{ W::set(builder[size_t(arg)]); }

void w_push(Arg arg)
{ W::push(builder[size_t(arg)]); }

void w_menu_driver(Arg arg)
{ W::menu_driver(arg); }

#endif // UI_HPP
