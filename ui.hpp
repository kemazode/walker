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
        end,
    };
};

// Functions for working with the scenario
static inline void scenario_move_player_x(Args args);
static inline void scenario_move_player_y(Args args);
static inline void scenario_move_view_x(Args args);
static inline void scenario_move_view_y(Args args);
static        void scenario_init(Args args);
static        void scenario_generate(Args args);
static        void scenario_load();

static inline void w_push(Args args);
static inline void w_set(Args args);
static inline void w_menu_driver(Args args);

static vector<vector<W::Menu>> menus =
{
    // Menu::null
    {},

    { // Menu::main
      W::Menu("Start New Game", Action(scenario_load)),
      W::Menu("Create Map",     Action(w_push, Args(Builder::map_creator))),
      W::Menu("Scoreboard",     Action()),
      W::Menu("Options",        Action()),
      W::Menu("Exit",           Action(W::clear)),
    },

    { // Menu::game
      W::Menu("Continue",       Action(W::pop)),
      W::Menu("Exit",           Action(w_set, Args(Builder::main))),
    },

    { // Menu::okay
      W::Menu("OK",             Action(W::pop)),
    },

    { // Menu::create
      W::Menu("Generate",       Action(w_push, Args(Builder::map_sizes))),
    },

    { // Menu::map_sizes
      W::Menu("100x100", Action(scenario_generate, Args(100))),
      W::Menu("250x250", Action(scenario_generate, Args(250))),
      W::Menu("500x500", Action(scenario_generate, Args(500))),
    },
};

static vector<vector<W::Hook>> hooks =
{
    // Hooks::null
    {},

    { // Hooks::main
      W::Hook(KEY_DOWN, Action(w_menu_driver, Args(REQ_DOWN_ITEM))),
      W::Hook(KEY_UP,   Action(w_menu_driver, Args(REQ_UP_ITEM))),
      W::Hook('\n',     Action(w_menu_driver, Args(REQ_EXEC_ITEM)))
    },

    { // Hooks::game
      W::Hook('Q',       Action(w_push, Args(Builder::game_menu))),
      W::Hook('q',       Action(w_push, Args(Builder::game_menu))),

      // Player moving
      W::Hook(KEY_DOWN,  Action(scenario_move_player_y, Args(1))),
      W::Hook(KEY_UP,    Action(scenario_move_player_y, Args(-1))),
      W::Hook(KEY_LEFT,  Action(scenario_move_player_x, Args(-1))),
      W::Hook(KEY_RIGHT, Action(scenario_move_player_x, Args(1))),

      // Map moving
      W::Hook('k', Action(scenario_move_view_y, Args(1))),
      W::Hook('i', Action(scenario_move_view_y, Args(-1))),
      W::Hook('j', Action(scenario_move_view_x, Args(-1))),
      W::Hook('l', Action(scenario_move_view_x, Args(1))),
      W::Hook('K', Action(scenario_move_view_y, Args(1))),
      W::Hook('I', Action(scenario_move_view_y, Args(-1))),
      W::Hook('J', Action(scenario_move_view_x, Args(-1))),
      W::Hook('L', Action(scenario_move_view_x, Args(1))),
    },

    { // Hooks::menu
      W::Hook('q',      Action(W::pop)),
      W::Hook('Q',      Action(W::pop)),
      W::Hook(KEY_DOWN, Action(w_menu_driver, Args(REQ_DOWN_ITEM))),
      W::Hook(KEY_UP,   Action(w_menu_driver, Args(REQ_UP_ITEM))),
      W::Hook('\n',     Action(w_menu_driver, Args(REQ_EXEC_ITEM)))
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

void w_set(Args args)
{ W::set(builder[size_t(args.num)]); }

void w_push(Args args)
{ W::push(builder[size_t(args.num)]); }

void w_menu_driver(Args args)
{ W::menu_driver(args.num); }

#endif // UI_HPP
