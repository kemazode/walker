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

#include <cstdlib>
#include <cmath>

#include "window.hpp"
#include "utils.hpp"

#define DESCRIPTION_HORIZONTAL_INTEND 1
#define HORIZONTAL_INTEND 2
#define VERTICAL_INTEND 2

#define TITLE_SEPARATION " "
#define ITEM_DESCRIPTION " "
#define ITEM_SELECT " >>---> "

static int text_height(const struct text *t, int freecols);
static int waddtext(WINDOW *w, const struct text *t, format f);
static int waddcchar(WINDOW *w, const struct cchar *t);

static int items_count(const item *);
static int hooks_count(const hook *);

static int put_image(struct window *, const struct text *, int& nextline);
static int put_text(struct window *, const struct text &, enum format, int& nextline);
static int put_menu(struct window *, const struct item *, attr_t attribute, int& nextline);

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
  new_w->window   = newwin(loc_w.lines + (HORIZONTAL_INTEND * 2),
                           loc_w.cols + (VERTICAL_INTEND * 2),
                           loc_w.y,
                           loc_w.x);

  int nextline = HORIZONTAL_INTEND;
  //  int sub_window_width = loc_w.cols - 2*TEXT_INDENT_X;

  if (builder.image && builder.image->lenght)
      put_image(new_w, builder.image, nextline);
  else
      new_w->sub_window_image = nullptr;

  //int description_height = text_height(&builder.text, sub_window_width);
  if (builder.text.lenght)
      put_text(new_w, builder.text, builder.text_format, nextline);
  else
      new_w->sub_window_text = nullptr;

  if (builder.items)
      put_menu(new_w, builder.items, builder.attribute, nextline);
  else {
      new_w->menu  = nullptr;
      new_w->items = nullptr;
      new_w->sub_window_menu = nullptr;
      new_w->sub_window_desc = nullptr;
  }

  /* Set window decorations */
  wattron(new_w->window, builder.attribute);

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

  wattroff(new_w->window, builder.attribute);

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
  if (item && item->description)
    mvwaddstr(top_window->sub_window_desc, 0, 0, item->description);

  window_refresh();
}

void window_hook()
{
  if (!top_window) return;

  struct hook *hks = top_window->hooks;
  int hks_c = top_window->hooks_c;

  int key = getch();

  /* Так как struct window хранит только указатель
   * на hooks, то после удаления окна можно продолжать выполнять
   * команды определенные хуками */
  for (int i = 0; i < hks_c; ++i)
    if (hks[i].key == key)
        hks[i].action();
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
        top_window->sub_window_text = derwin(top_window->window, loc_w.lines, loc_w.cols, HORIZONTAL_INTEND, VERTICAL_INTEND);
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
{ /* Put this text indent as a second border */
  switch (p) {
    case POSITION_FULL:
      return location{0, 0, LINES - (HORIZONTAL_INTEND * 2), COLS - (VERTICAL_INTEND * 2)};
    case POSITION_AVERAGE:
      return location{COLS/8, LINES/8, LINES - LINES/4 - (HORIZONTAL_INTEND * 2), COLS - COLS/4 - (VERTICAL_INTEND * 2)};
    case POSITION_SMALL:
      return location{COLS/4, LINES/4, LINES/2 - (HORIZONTAL_INTEND * 2), COLS/2 - (VERTICAL_INTEND * 2)};
    }
  return location{0, 0, 0, 0};
}

struct location window_get_location(void)
{
  return window_get_location(top_window->position);
}

static int items_count(const struct item *i)
{
  if (!i) return 0;
  int count = 0;
  while (i->label ||
         i->action.arg != 0 ||
         i->action.function != nullptr)
    { ++count; ++i;}

  return count;
}

static int hooks_count(const struct hook *h)
{
  if (!h) return 0;
  int count = 0;
  while (h->key != 0 ||
         h->action.arg != 0 ||
         h->action.function != nullptr)
    { ++count; ++h; }
  return count;
}


static int put_image(struct window *win, const struct text *img, int& nextline)
{
    struct location loc = window_get_location(win->position);

    // compute image height
    int image_height = 0;
    for (int i = 0; i < int(img->lenght); ++i)
        if (img->cstr[i].symbol == '\n') ++image_height;

    win->sub_window_image = derwin(win->window,
                                     image_height,
                                     loc.cols,
                                     nextline,
                                     VERTICAL_INTEND);

    nextline += image_height + 1 /* Indent */;
    return mvwaddtext(win->sub_window_image, 0, 0, img, FORMAT_CENTER_RIGHT);
}

static int put_text(struct window *win, const struct text &text, enum format text_format, int& nextline)
{
    struct location loc = window_get_location(win->position);
    int height = text_height(&text, loc.cols);

    /* Text drawing */
    win->sub_window_text = derwin(win->window,
                                    height,
                                    loc.cols,
                                    nextline,
                                    VERTICAL_INTEND);
    nextline += (height + 1);
    return mvwaddtext(win->sub_window_text, 0, 0, &text, text_format);
}

static int put_menu(struct window *win, const struct item *items, attr_t attribute, int& nextline)
{
    struct location loc = window_get_location(win->position);
    win->items_c = items_count(items);

    int menu_h = (win->items_c < loc.lines - nextline)?
                win->items_c : loc.lines - nextline;

    win->sub_window_menu = derwin(win->window,
                                    menu_h,
                                    loc.cols,
                                    nextline,
                                    VERTICAL_INTEND);

    win->sub_window_desc = derwin(win->window,
                                    1,
                                    loc.cols,
                                    getmaxy(win->window) - (DESCRIPTION_HORIZONTAL_INTEND * 2),
                                    VERTICAL_INTEND);

    win->items = new ITEM *[size_t(win->items_c) + 1];

    for (int i = 0; i < win->items_c; ++i)
    {
        win->items[i] = new_item(items[i].label, ITEM_DESCRIPTION);

        /* If item doesn't have any function */
        if (items[i].action.function == nullptr)
            item_opts_off(win->items[i], O_SELECTABLE);
        else
            set_item_userptr(win->items[i], const_cast<struct item *>(&items[i]));
    }

    win->items[win->items_c] = nullptr;

    /* Link menu & window */
    win->menu = new_menu(win->items);
    set_menu_win(win->menu, win->window);
    set_menu_sub(win->menu, win->sub_window_menu);

    /* For menu scrolling */
    set_menu_format(win->menu, menu_h, 1);

    /* Set menu decorations */
    set_menu_mark(win->menu, ITEM_SELECT);
    set_menu_grey(win->menu, A_DIM);
    set_menu_fore(win->menu, attribute|A_BOLD);
    set_menu_back(win->menu, PAIR(COLOR_WHITE, COLOR_BLACK));

    /* Attaching */
    post_menu(win->menu);

    /* Print description */
    auto item = reinterpret_cast<struct item *>(item_userptr(current_item(win->menu)));
    if (item->description)
        mvwaddstr(win->sub_window_desc, 0, 0, item->description);

    return 0;
}
