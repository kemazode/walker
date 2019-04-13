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

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <ncurses.h>
#include <panel.h>
#include <menu.h>

#include "utils.hpp"

#define REQ_EXEC_ITEM (MAX_MENU_COMMAND + 1)

struct window;

enum position
{
    POSITION_FULL,
    POSITION_AVERAGE,
    POSITION_SMALL,
};

enum option
{
    OPTION_NORMAL     = 0,
    OPTION_BORDERLESS = 1 << 0,
};

enum format
{
  FORMAT_RIGHT,
  FORMAT_CENTER,
};

struct location
{
    int x, y, lines, cols;
};

struct item
{
  const char *label;
  struct action action;

  item() = default;
  item(const char *l)
    : label(l), action() {}
  item(const char *l, struct action &&a)
    : label(l), action(a) {}
};

struct hook
{
  int key;
  struct action action;

  hook(int k, struct action &&a)
    : key(k), action(a) {}
};

struct builder
{
  enum position position;
  struct item *items;
  struct hook *hooks;
  struct text text;
  struct text title;
  enum option options;
  enum format format;

  builder(enum position p,
          struct item *i,
          struct hook *h,
          const struct text &t,
          const struct text &e,
          enum option o = OPTION_NORMAL,
          enum format f = FORMAT_RIGHT)
    : position(p),
      items(i),
      hooks(h),
      text(t),
      title(e),
      options(o),
      format(f)
  {}
};

window *window_push(const builder &builder);
void window_pop(void);
void window_refresh(void);
void window_hook(void);
void window_menu_driver(int);
void window_set(const builder &);
void window_clear(void);

bool window_has(struct window *);
window *window_top(void);

/* For map rendering */
void window_print(const vector<text> &, int x, int y);

struct location window_get_location(enum position);

/* Get location of top window */
struct location window_get_location(void);

#endif // WINDOW_HPP
