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
#include <cstring>
#include <vector>
#include "utils.hpp"

using std::vector;

#define REQ_EXEC_ITEM (MAX_MENU_COMMAND + 1)

class Window {
public:

    struct Location {
      int x, y, lines, cols;
    };

    enum Position {
        full,
        aver,
        small,
        game,
        stat,
        log,
    };

    enum Option {
        normal     = 0,
        borderless = 1 << 0,
    };

    struct Menu {
        Menu() : label(), act() {}
        Menu(const string &l) : label(l), act() {}
        Menu(const string &l, const ActionAV &a) : label(l), act(a) {}

        string label;
        ActionAV act;
    };

    struct Hook {
        Hook() : key(0), act() {}
        Hook(int k, const ActionAV &a) : key(k), act(a) {}
        int key;
        ActionAV act;
    };

    struct Builder {
        Builder(Position p_, vector<Menu> &m_,
                    const vector<Hook> &h_, const Text &t_, const Text &title_,
                    Option o_ = normal) :
           p(p_), o(o_), m(m_), h(h_), t(t_), title(title_) {}

        /* Message replacing */
        Builder operator|(const string &mesg)
        {
            Builder nc(*this);
            nc.t = mesg;
            return nc;
        }

        const Position p;
        const Option o;
        vector<Menu> &m;
        const vector<Hook> &h;
        Text t, title;
    };

private:

    PANEL  *m_pan;
    WINDOW *m_win;
    WINDOW *m_sub_t;
    WINDOW *m_sub_m;
    MENU   *m_menu;
    ITEM   **m_items;

    // Window which has top panel in the panel stack
    static Window *topw;

    enum Position m_pos;
    const vector<Hook> *m_hooks;

    Window(const Builder &c);
    ~Window();

    void _refresh() const;
    void _hook() const;

    void _menu_driver(int act) const;

    void _print(const vector<Text> &vt, int x, int y);
    void _print(const Text &t);

public:
    // prefix "a" - union "Args" as function argument

    static Window *push(const Builder &c)
    { return (topw = new Window(c)); }

    // ~Window() changes topw to below Window * (by PANEL stack)
    static void pop()
    { delete topw; }

    static void clear()
    { while(topw) pop(); }

    static Window *top()
    { return topw; }

    static bool has(const Window *w)
    {
        const Window *temp = topw;

        while (temp)
            if (temp == w)
                return true;
            else
                temp = reinterpret_cast<const Window *>(panel_userptr(panel_below(temp->m_pan)));

        return false;
    }

    // Static methods for TOP Window in th    static int getButtons()

    static bool refresh()
    { return topw? (static_cast<void>(topw->_refresh()), true) : false; }

    static bool hook()
    { return topw? (static_cast<void>(topw->_hook()), true) : false; }

    static bool menu_driver(int act)
    { return topw? (static_cast<void>(topw->_menu_driver(act)), true) : false; }

    static bool print(const Text &t)
    { return topw? (static_cast<void>(topw->_print(t)), true) : false; }

    static bool print(const vector<Text> &t, int x, int y)
    { return topw? (static_cast<void>(topw->_print(t, x, y)), true) : false; }

    static void set(const Builder &c)
    { clear(); push(c); }

    static Location get_location(Position p);

    static Location get_location()
    { return topw? get_location(topw->m_pos): Location{0, 0, 0, 0}; }

};

#endif // WINDOW_HPP
