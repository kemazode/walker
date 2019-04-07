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
void scenario_move_player_x(Arg);
void scenario_move_player_y(Arg);
void scenario_move_view_x  (Arg);
void scenario_move_view_y  (Arg);
void scenario_generate     (Arg);
void scenario_load            ();

extern vector<W::Builder>      builder;
extern vector<vector<W::Menu>> menus;
extern vector<vector<W::Hook>> hooks;

#endif // UI_HPP
