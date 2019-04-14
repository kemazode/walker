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

#include <cstdlib>
#include <cmath>

#include "window.hpp"
#include "utils.hpp"

#define LINES_BORDERS_WIDTH 1
#define COLS_BORDERS_WIDTH 1

#define LINES_BORDERS (LINES_BORDERS_WIDTH*2)
#define COLS_BORDERS (COLS_BORDERS_WIDTH*2)

#define TEXT_BORDER_INDENT_X 1
#define TEXT_BORDER_INDENT_Y 1

#define TITLE_SEPARATION " "
#define ITEM_DESCRIPTION " "
#define ITEM_SELECT " >>---> "

static int text_height(const struct text *t, int freecols);
static int waddtext(WINDOW *w, const struct text *t, format f);
static int waddcchar(WINDOW *w, const struct cchar *t);

static int items_count(struct item *);
static int hooks_count(struct hook *);

inline int mvwaddtext(WINDOW *w, int x, int y, const text *t, format f)
{ return wmove(w,y,x) == ERR ? ERR : waddtext(w,t,f); }

struct window {
  PANEL  *panel;
  WINDOW *window;
  WINDOW *sub_window_text;
  WINDOW *sub_window_menu;
  WINDOW *sub_window_image;
  WINDOW *sub_window_desc;
  MENU   *menu;
  ITEM   **items;

  int items_c;
  int hooks_c;

  enum position position;
  struct hook *hooks;
};

static struct window *top_window = nullptr;

window *window_push(const struct builder &builder)
{
  struct window *new_w = new window;
  struct location loc_w = window_get_location(builder.position);

  new_w->position = builder.position;
  new_w->window   = newwin(loc_w.lines + LINES_BORDERS,
                           loc_w.cols + COLS_BORDERS,
                           loc_w.y,
                           loc_w.x);

  /* Set window decorations */
  wattron(new_w->window, PAIR(MYCOLOR, COLOR_BLACK));

  if ( !(builder.options & OPTION_BORDERLESS))
       box(new_w->window, 0, 0);

  /* Window title */
  if (builder.title.lenght)
    {
      wattron(new_w->window, A_REVERSE);
      wmove(new_w->window, 0, loc_w.cols/2 - int(builder.title.lenght/2));
      waddstr(new_w->window, TITLE_SEPARATION);
      waddtext(new_w->window, &builder.title, FORMAT_RIGHT);
      waddstr(new_w->window, TITLE_SEPARATION);
      wattroff(new_w->window, A_REVERSE);
    }

  wattroff(new_w->window, PAIR(MYCOLOR, COLOR_BLACK));

  int nextline = LINES_BORDERS_WIDTH + TEXT_BORDER_INDENT_Y;
  int sub_window_width = loc_w.cols - 2*TEXT_BORDER_INDENT_X;

  if (builder.image)
    {
      int text_h = text_height(builder.image, sub_window_width);

      if (builder.image_pos == IMAGE_POSITION_TOP) {

          /*
          +-----------------------------------+
          |                                   |
          | +-------------------------------+ |
          | |                               | |
          | |             image             | |
          | |                               | |
          | |                               | |
          | +-------------------------------+ |
          | _________________________________ |
          |                                   |
          |               title               |
          |                                   |
          |    subscription...                |
          |                                   |
          |    - item                         |
          |                                   |
          +-----------------------------------+
          */
          /* "--nextline" because image hasn't top border */
          --nextline;

          new_w->sub_window_image = derwin(new_w->window,
                                           text_h,
                                           sub_window_width,
                                           nextline,
                                           COLS_BORDERS_WIDTH + TEXT_BORDER_INDENT_X);
          nextline += (text_h + 1);
          mvwaddtext(new_w->sub_window_image, 0, 0, builder.image, builder.image_format);

          if (!(builder.image_option & OPTION_BORDERLESS))
            wborder(new_w->sub_window_image, ' ', ' ', ' ', '_', ' ', ' ', ' ', ' ');
          else /* Because image hasn't bottom border */
            --nextline;
        }
      else if (builder.image_pos == IMAGE_POSITION_LEFT)
        {
          /* Determine the width of the image */
          int image_width = int(builder.image->lenght) + 1;
          for (int i = 0; i < int(builder.image->lenght); ++i)
            if (builder.image->cstr[i].symbol == '\n')
              {
                image_width = i;
                break;
              }
          ++image_width;

          loc_w.cols  /= 2;

          /*  Recompute the standard width of the subwindow */
          sub_window_width = loc_w.cols - 2*TEXT_BORDER_INDENT_X;

          /*
          +-----------------+-----------------+
          |                 |                 |
          |      title      |   +---------+   |
          |                 |   |         |   |
          |  subscription.  |   |  image  |   |
          |                 |   |         |   |
          |  - item         |   |         |   |
          |                 |   |         |   |
          |  - item         |   |         |   |
          |                 |   +---------+   |
          |                 |                 |
          +-----------------+-----------------+
          */

          int image_x = loc_w.cols + (loc_w.cols - image_width)/2 - COLS_BORDERS_WIDTH;
          int image_y = (loc_w.lines - text_h)/2 + LINES_BORDERS_WIDTH;

          /* Set to center */
          new_w->sub_window_image = derwin(new_w->window,
                                           text_h,
                                           image_width + COLS_BORDERS,
                                           image_y,
                                           image_x);

          mvwaddtext(new_w->sub_window_image, 0, 0, builder.image, builder.image_format);
          //box(new_w->sub_window_image, 0,0 );
        }
    }
  else
    new_w->sub_window_image = nullptr;

  if (builder.text.lenght)
    {
      int text_h = text_height(&builder.text, sub_window_width);

      /* Text drawing */
      new_w->sub_window_text = derwin(new_w->window,
                                      text_h,
                                      sub_window_width,
                                      nextline,
                                      COLS_BORDERS_WIDTH + TEXT_BORDER_INDENT_X);
      nextline += (text_h + 1);
      mvwaddtext(new_w->sub_window_text, 0, 0, &builder.text, builder.text_format);      
    } else
    new_w->sub_window_text = nullptr;

  if (builder.items)
    {
      /* Menu subwin creating */
      new_w->items_c = items_count(builder.items);

      int menu_h = (new_w->items_c < loc_w.lines - nextline)?
            new_w->items_c : loc_w.lines - nextline;

      new_w->sub_window_menu = derwin(new_w->window,
                                      menu_h,
                                      sub_window_width,
                                      nextline,
                                      COLS_BORDERS_WIDTH + TEXT_BORDER_INDENT_X);


      new_w->sub_window_desc = derwin(new_w->window,
                                      1,
                                      sub_window_width,
                                      getmaxy(new_w->window) - LINES_BORDERS,
                                      COLS_BORDERS_WIDTH + TEXT_BORDER_INDENT_X);

      new_w->items = new ITEM *[size_t(new_w->items_c) + 1];

      for (int i = 0; i < new_w->items_c; ++i)
        {
          new_w->items[i] = new_item(builder.items[i].label, ITEM_DESCRIPTION);

          /* If item doesn't have any function */
          if (builder.items[i].action.function == nullptr)
            item_opts_off(new_w->items[i], O_SELECTABLE);
          else
            set_item_userptr(new_w->items[i], &builder.items[i]);
        }

      new_w->items[new_w->items_c] = nullptr;

      /* Link menu & window */
      new_w->menu = new_menu(new_w->items);
      set_menu_win(new_w->menu, new_w->window);
      set_menu_sub(new_w->menu, new_w->sub_window_menu);

      /* For menu scrolling */
      set_menu_format(new_w->menu, menu_h, 1);

      /* Set menu decorations */
      set_menu_mark(new_w->menu, ITEM_SELECT);
      set_menu_grey(new_w->menu, A_DIM);
      set_menu_fore(new_w->menu, PAIR(MYCOLOR, COLOR_BLACK)|A_BOLD);
      set_menu_back(new_w->menu, PAIR(COLOR_WHITE, COLOR_BLACK));

      /* Attaching */
      post_menu(new_w->menu);

      /* Print description */
      auto item = reinterpret_cast<struct item *>(item_userptr(current_item(new_w->menu)));
      if (item->description)
        mvwaddstr(new_w->sub_window_desc, 0, 0, item->description);

    } else {
      new_w->menu  = nullptr;
      new_w->items = nullptr;
      new_w->sub_window_menu = nullptr;
      new_w->sub_window_desc = nullptr;
    }

  /* Set hooks */
  new_w->hooks   = builder.hooks;
  new_w->hooks_c = hooks_count(new_w->hooks);

  /* Panel creation */
  new_w->panel = new_panel(new_w->window);
  set_panel_userptr(new_w->panel, new_w);
  top_window = new_w;

  window_refresh();
  update_panels();

  return top_window;
}

void window_pop()
{
  /* Set the top window */
  if (!top_window) return;

  PANEL *panel_b = panel_below(top_window->panel);
  struct window *new_top_window = nullptr;

  if (panel_b)
    /* set_panel_userptr takes userprt as "const void *",
         * so if we want to get userptr for changing we need to reset const qualifier.
         */
    new_top_window = reinterpret_cast<struct window *>(const_cast<void *>(panel_userptr(panel_b)));

  if (top_window->menu) {
      unpost_menu(top_window->menu);
      int n = item_count(top_window->menu);
      free_menu(top_window->menu);
      for (int i = 0; i < n; ++i)
        free_item(top_window->items[i]);
      free_item(top_window->items[n]);
    }

  delete [] top_window->items;
  del_panel(top_window->panel);
  delwin(top_window->sub_window_menu);
  delwin(top_window->sub_window_text);
  delwin(top_window->sub_window_image);
  delwin(top_window->sub_window_desc);
  delwin(top_window->window);
  delete top_window;

  top_window = new_top_window;

  update_panels();
  window_refresh();
}

void window_refresh()
{
  if (!top_window) return;

  wnoutrefresh(top_window->sub_window_text);
  wnoutrefresh(top_window->sub_window_menu);
  wnoutrefresh(top_window->sub_window_image);
  wnoutrefresh(top_window->sub_window_desc);
  wnoutrefresh(top_window->window);
  doupdate();
}

void window_menu_driver(int req)
{
  if (!top_window) return;

  switch (req) {
    case REQ_EXEC_ITEM:
      auto item = reinterpret_cast<struct item *>(item_userptr(current_item(top_window->menu)));
      if (item) item->action();
      return;
    }

  menu_driver(top_window->menu, req);

  auto item = reinterpret_cast<struct item *>(item_userptr(current_item(top_window->menu)));
  wclear(top_window->sub_window_desc);
  if (item->description)
    mvwaddstr(top_window->sub_window_desc, 0, 0, item->description);

  window_refresh();
}

void window_hook()
{
  if (!top_window) return;

  struct window *cur = top_window;

  int key = getch();

  /* "cur == top_window &&" нужен в случае, если вызов какой-то команды удалит окно/стек окон,
     * или создаст новое, при этом указатель может повиснет на освобожденной структуре*/
  for (int i = 0; cur == top_window && i < cur->hooks_c; ++i)
    if (cur->hooks[i].key == key) {
        cur->hooks[i].action();
        return;
      }
}

void window_print(const vector<text> &vec, int x, int y)
{
  if (vec.empty() || !top_window) return;
  int text_h =
      top_window->sub_window_text? getmaxy(top_window->sub_window_text) -
                                   getbegy(top_window->sub_window_text) + 1 : 0;
  struct location loc_w = window_get_location();
  int xend = x + loc_w.cols;
  int yend = y + loc_w.lines;

  if (text_h != loc_w.lines)
    {
      if (top_window->sub_window_text)
        wresize(top_window->sub_window_text, loc_w.lines, loc_w.cols);
      else
        top_window->sub_window_text = derwin(top_window->window, loc_w.lines, loc_w.cols, LINES_BORDERS_WIDTH, COLS_BORDERS_WIDTH);
    }

  wmove(top_window->sub_window_text, 0, 0);

  int h = int(vec.size());
  int w = int(vec.at(0).lenght);

  /* Double array is limited to NULL "strcut text *" */
  for (int i = y; i < yend; ++i)
    for (int j = x; j < xend; ++j)
      {
        if (i >= h || j >= w)
          {
            waddch(top_window->sub_window_text, '\n');
            break;
          }
        waddcchar(top_window->sub_window_text, &vec.at(size_t(i)).cstr[j]);
      }

  window_refresh();
}

void window_set(const struct builder &builder)
{
  window_clear();
  window_push(builder);
}

void window_clear(void)
{
  while(top_window)
    window_pop();
}

bool window_has(struct window *window)
{
  const struct window *temp = top_window;

  while (temp)
    if (temp == window) return true;
    else temp = reinterpret_cast<const struct window *>(panel_userptr(panel_below(temp->panel)));

  return false;
}

struct window *window_top(void)
{ return top_window; }

int text_height(const struct text *t, int freecols)
{
  if (!t) return 0;

  size_t old = 0;
  int text_height = 1, templen;

  for (size_t i = 0; i < t->lenght; ++i)
    if (t->cstr[i].symbol == '\n')
      {
        templen = int(i) - int(old);

        text_height +=  ((templen - 1) / freecols + 1);
        old = i + 1;
      }

  templen = int(t->lenght) - int(old);

  text_height += (templen - 1) / freecols;

  return text_height;
}

int waddtext(WINDOW *w, const struct text *t, format f)
{    
  int rc = OK;

  if (f == FORMAT_CENTER)
    {
      int width = getmaxx(w) - getcurx(w);
      bool indent_exists = false;
      int next_line = int(t->lenght) - 1;

      for (int i = 0; i < int(t->lenght); ++i)
        {
          /* When newline */
          if (getmaxx(w) - getcurx(w) == width)
            {
              indent_exists = false;

              next_line = int(t->lenght) - i;
              for (int n = i; n < int(t->lenght); ++n)
                if (t->cstr[n].symbol == '\n')
                  {
                    next_line = n - i;
                    break;
                  }
            }

          if (next_line <= width && !indent_exists)
            {
              indent_exists = true;

              int indent_lenght = (width - next_line)/2;
              for (int i = 0; i <= indent_lenght; ++i)
                rc = waddch(w, ' ');
            }

          rc = waddcchar(w, &t->cstr[i]);
        }
    }
  else if (f == FORMAT_CENTER_RIGHT)
    {
      int width = getmaxx(w) - getcurx(w);
      bool indent_exists = false;
      int next_newline = int(t->lenght) - 1;

      for (int n = 0; n < int(t->lenght); ++n)
        if (t->cstr[n].symbol == '\n')
          {
            next_newline = n ;
            break;
          }

      for (int i = 0; i < int(t->lenght); ++i)
        {
          if (getmaxx(w) - getcurx(w) == width)
            indent_exists = false;

          /* When newline */
          if (next_newline <= width && !indent_exists)
            {
              indent_exists = true;

              int indent_lenght = (width - next_newline)/2;
              for (int i = 0; i <= indent_lenght; ++i)
                rc = waddch(w, ' ');
            }
          rc = waddcchar(w, &t->cstr[i]);
        }
    }
  else if (f == FORMAT_RIGHT)
    {
      for (int i = 0; i < int(t->lenght); ++i)
        rc = waddcchar(w, &t->cstr[i]);
    }
  return rc;
}

int waddcchar(WINDOW *w, const struct cchar *t)
{
  int rc = OK;

  if (t->attribute & A_INVIS)
    return rc = waddch(w, ' ');

  wattron(w, t->attribute);
  rc = waddch(w, chtype(t->symbol));
  wattroff(w, t->attribute);
  return rc;
}

struct location window_get_location(enum position p)
{
  switch (p) {
    case POSITION_FULL:  return location{0, 0, LINES - LINES_BORDERS, COLS - COLS_BORDERS};
    case POSITION_AVERAGE:  return location{COLS/8, LINES/8, LINES - LINES/4 - LINES_BORDERS, COLS - COLS/4 - COLS_BORDERS};
    case POSITION_SMALL: return location{COLS/4, LINES/4, LINES/2 - LINES_BORDERS, COLS/2 - COLS_BORDERS};
    }
  return location{0, 0, 0, 0};
}

struct location window_get_location(void)
{
  return window_get_location(top_window->position);
}

static int items_count(struct item *i)
{
  if (!i) return 0;
  int count = 0;
  while (i->label ||
         i->action.arg != 0 ||
         i->action.function != nullptr)
    { ++count; ++i;}

  return count;
}

static int hooks_count(struct hook *h)
{
  if (!h) return 0;
  int count = 0;
  while (h->key != 0 ||
         h->action.arg != 0 ||
         h->action.function != nullptr)
    { ++count; ++h; }
  return count;
}
