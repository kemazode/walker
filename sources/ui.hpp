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

enum menu
{
  MENU_MAIN,
  MENU_GAME,
  MENU_OKAY,
  MENU_BACK,
  MENU_MAP_CREATOR,
  MENU_MAP_SIZES,
};


enum hooks
{
  HOOKS_MAIN,
  HOOKS_GAME,
  HOOKS_MENU,
  //HOOKS_EVENT_DIALOG,
};

enum desc
{
  DESC_MAIN,
  DESC_MAP_CREATOR,
  DESC_MAP_SIZES,
  DESC_CONTROL,
};

enum title
{
  TITLE_MENU,
  TITLE_MESSAGE,
  TITLE_MAP_CREATOR,
  TITLE_MAP_SIZES,
  TITLE_ERROR,
  TITLE_CONTROL
};

enum build
{
  BUILD_MAIN,
  BUILD_GAME,
  BUILD_GAME_MENU,
  BUILD_OKAY,
  BUILD_MAP_CREATOR,
  BUILD_MAP_SIZES,
  BUILD_ERROR,
  BUILD_CONTROL,
};

void init_builder(void);

extern  item *menus[];
extern  hook *hooks[];
extern builder build[];

/* For convenience */
inline void window_push(arg_t arg, const string &str)
{
  window_push(builder(build[arg].position,
                      build[arg].items,
                      build[arg].hooks,
                      str,
                      build[arg].title,
                      build[arg].options));
}

inline void window_push(arg_t arg, const char *str)
{
  window_push(builder(build[arg].position,
                      build[arg].items,
                      build[arg].hooks,
                      str,
                      build[arg].title,
                      build[arg].options));
}

inline void window_push(arg_t arg)
{ window_push(build[arg]); }

inline void window_set(arg_t arg)
{ window_set(build[arg]); }

#endif // UI_HPP
