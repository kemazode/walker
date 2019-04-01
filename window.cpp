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

#include "window.hpp"

#define LINES_BORDERS 2
#define COLS_BORDERS 2
#define SUB_WIN_Y 1
#define SUB_WIN_X 1
#define TEXT_X 1

static int text_height(const Text &t, int freecols);
static int waddtext(WINDOW *w, const Text &t);
static int waddcchar(WINDOW *w, const cchar &t);

#define mvwaddtext(win,y,x,t) \
    (wmove((win),(y),(x)) == ERR ? ERR : waddtext((win),(t)))

Window *Window::topw      = nullptr;

Window::Window(const Constructor &c)
{
    int x = 0, y = 0, lines = 0, cols = 0;
    m_pos = c.p;

    #define P(x_, y_, l_, c_) x = x_; y = y_; lines = l_; cols = c_;

    switch (m_pos) {
    case POS_FULL: P(0, 0, LINES, COLS);
        break;
    case POS_AVER: P(COLS/8, LINES/8, LINES - LINES/4, COLS - COLS/4);
        break;
    case POS_SMLL: P(COLS/4, LINES/4, LINES/2, COLS/2);
        break;
    }

    m_win = newwin(lines, cols, y, x);

    /* Set window decorations */
    wattron(m_win, PAIR(MYCOLOR, COLOR_BLACK)|A_DIM);
    box(m_win, 0, 0);
    wattroff(m_win, PAIR(MYCOLOR, COLOR_BLACK)|A_DIM);

    /* Free height (lines) space */
    freelines = lines - LINES_BORDERS;
    freecols =  cols  - COLS_BORDERS;

    int nextline = SUB_WIN_Y;

    if (!c.t.empty())
    {
        int text_h = text_height(c.t, freecols - 2*TEXT_X);

        /* Text drawing */
        m_sub_t = derwin(m_win, text_h, freecols - 2*TEXT_X, nextline, SUB_WIN_X + TEXT_X);
        mvwaddtext(m_sub_t, 0, 0, c.t);

        nextline += text_h;
    } else
        m_sub_t = nullptr;

    ++nextline;

    if (!c.m.empty())
    {        
        /* Menu subwin creating */
        int menu_h = (int(c.m.size()) < freelines - nextline)? int(c.m.size()): freelines - nextline;

        m_sub_m = derwin(m_win, menu_h, freecols, nextline, SUB_WIN_X);

        /* Create items from Menu* m */
        m_items = new ITEM *[c.m.size() + 1];

        for (size_t i = 0; i < c.m.size(); ++i)
        {
            m_items[i] = new_item(c.m[i].label.c_str(), " ");

            /* If item doesn't have any function */
            if (c.m[i].act.empty())
                item_opts_off(m_items[i], O_SELECTABLE);
            else
                m_items[i]->userptr = &c.m[i];
        }

        m_items[c.m.size()] = nullptr;

        /* Link menu & window */
        m_menu = new_menu(m_items);
        set_menu_win(m_menu, m_win);
        set_menu_sub(m_menu, m_sub_m);

        /* For menu scrolling */
        set_menu_format(m_menu, menu_h, 1);

        /* Set menu decorations */
        set_menu_mark(m_menu, " * ");
        set_menu_grey(m_menu, A_DIM);
        set_menu_fore(m_menu, PAIR(MYCOLOR, COLOR_BLACK)|A_BOLD);
        set_menu_back(m_menu, PAIR(COLOR_WHITE, COLOR_BLACK));

        /* Attaching */
        post_menu(m_menu);

    } else {
        m_menu  = nullptr;
        m_items = nullptr;
        m_sub_m = nullptr;
    }

    /* Set hooks */
    m_hooks = &c.h;

    /* Panel creation */
    m_pan = new_panel(m_win);
    set_panel_userptr(m_pan, this);
    topw = this;

    _refresh();
    update_panels();    
}

Window::~Window()
{
    /* Set the top window */
    auto pan_ptr = panel_below(topw->m_pan);


    if (pan_ptr)
        topw = reinterpret_cast<Window *>(const_cast<void *>(panel_userptr(pan_ptr)));
    else
        topw = nullptr;

    if (m_menu) {
        unpost_menu(m_menu);
        int n = item_count(m_menu);
        free_menu(m_menu);
        for (int i = 0; i < n; ++i)
            free_item(m_items[i]); 
        free_item(m_items[n]);
    }

    delete [] m_items;
    del_panel(m_pan);
    delwin(m_sub_m);
    delwin(m_sub_t);
    delwin(m_win);

    update_panels();

    refresh();
}

void Window::_refresh() const
{
    wnoutrefresh(this->m_sub_t);
    wnoutrefresh(this->m_sub_m);
    wnoutrefresh(this->m_win);
    doupdate();
}

void Window::_menu_driver(int act) const
{
    Menu *temp = static_cast<Menu *>(item_userptr(current_item(m_menu)));
    switch (act) {
    case REQ_EXEC_ITEM:
        if (temp)
            temp->act.exec();
        return;
    }

    ::menu_driver(m_menu, act);
    _refresh();
}

void Window::_exechook(int key) const
{
    for (const auto &h : *m_hooks)
        if (h.key == key)
            h.act.exec();
}

/* It reserves the entire space of the window panes to the text, if there is no menu */
void Window::_print(const vector<Text> &vt, int x, int y)
{    
    if (vt.empty() || m_sub_m) return;

    int cth = m_sub_t? getmaxy(m_sub_t) - getbegy(m_sub_t) + 1 : 0;

    int xend = x + freecols;
    int yend = y + freelines;

    if (cth != freelines) {
        if (m_sub_t)
            wresize(m_sub_t, freelines, freecols);
        else
            m_sub_t = derwin(m_win, freelines, freecols, SUB_WIN_Y, SUB_WIN_X);
    }

    wmove(m_sub_t, 0, 0);


    int h = int(vt.size());
    int w = int(vt.at(0).len);

    for (auto i = y; i < yend; ++i)
        for (auto j = x; j < xend; ++j)
        {
            if (i >= h || j >= w) {
                waddch(m_sub_t, '\n');
                break;
            }
            waddcchar(m_sub_t, vt.at(size_t(i)).text[j]);
        }

    _refresh();
}

void Window::_print(const Text &t)
{
    int cth = m_sub_t? getmaxy(m_sub_t) - getbegy(m_sub_t) + 1 : 0;
    int th = text_height(t, freecols - 2*TEXT_X);

    if (th != cth) {
        if (m_sub_m) {

            int it_c = item_count(m_menu);
            int menu_h = (it_c < freelines - th - 1)? it_c : freelines - th - 1;

            unpost_menu(m_menu);

            delwin(m_sub_m);

            m_sub_m = derwin(m_win, menu_h,
                             freecols, th + SUB_WIN_Y + 1,
                             SUB_WIN_X);

            set_menu_win(m_menu, m_win);
            set_menu_sub(m_menu, m_sub_m);
            post_menu(m_menu);
        }

        if (m_sub_t) {
            wresize(m_sub_t, th, freecols);
        } else {
            m_sub_t = derwin(m_win, th, freecols - 2*TEXT_X, SUB_WIN_Y, SUB_WIN_X + TEXT_X);
            wclear(m_sub_t);
        }
    }

    mvwaddtext(m_sub_t, 0, 0, t);
    refresh();
}

int text_height(const Text &t, int freecols)
{
    size_t old = 0;
    int text_height = 1, templen;

    if (t.empty()) return 0;

    for (size_t i = 0; i < t.len; ++i)
        if (t.text[i].c == '\n')
        {
            templen = ( int(i) - 1 - int(old) ) + 1;

            text_height +=  ((templen - 1) / freecols + 1);
            old = i + 1;
        }

    templen = ( int(t.len) - 1 - int(old) ) + 1;

    text_height += (templen - 1) / freecols;

    return text_height;
}

int waddtext(WINDOW *w, const Text &t)
{
    int rc = OK;
    for (size_t i = 0; i < t.len; ++i)
        rc = waddcchar(w, t.text[i]);
    return rc;
}

int waddcchar(WINDOW *w, const cchar &t)
{
    int rc = OK;
    wattron(w, t.attr);
    rc = waddch(w, chtype(t.c));
    wattroff(w, t.attr);
    return rc;
}
