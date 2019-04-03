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

    enum Pos {
        POS_FULL,
        POS_AVER,
        POS_SMLL,
    };

    struct Menu {
        Menu() : label(), act() {}
        Menu(const String &l, const Action &a) : label(l), act(a) {}

        String label;
        Action act;
    };

    struct Hook {
        Hook() : key(0), act() {}
        Hook(int k, const Action &a) : key(k), act(a) {}
        int key;
        Action act;
    };

    struct Constructor {    
        Constructor(Pos p_, vector<Menu> &m_, const vector<Hook> &h_, const Text &t_) :
           p(p_), m(m_), h(h_), t(t_) {}

        /* Message replacing */
        Constructor operator|(const String &mesg)
        {
            Constructor nc(*this);
            nc.t = mesg;
            return nc;
        }

        const Pos p;
        vector<Menu> &m;
        const vector<Hook> &h;
        Text t;
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

    enum Pos m_pos;
    const vector<Hook> *m_hooks;

    Window(const Constructor &c);
    ~Window();

    void _refresh() const;
    void _exechook(int key) const;

    void _menu_driver(int act) const;

    void _print(const vector<Text> &vt, int x, int y);
    void _print(const Text &t);

public:
    // prefix "a" - union "Args" as function argument

    static Window *push(const Constructor &c)
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

    // Static methods for TOP Window in th    static int getlines()

    static bool refresh()
    { return topw? (static_cast<void>(topw->_refresh()), true) : false; }

    static bool exechook(int key)
    { return topw? (static_cast<void>(topw->_exechook(key)), true) : false; }

    static bool menu_driver(int act)
    { return topw? (static_cast<void>(topw->_menu_driver(act)), true) : false; }

    static bool print(const Text &t)
    { return topw? (static_cast<void>(topw->_print(t)), true) : false; }

    static bool print(const vector<Text> &t, int x, int y)
    { return topw? (static_cast<void>(topw->_print(t, x, y)), true) : false; }

    static void set(const Constructor &c)
    { clear(); push(c); }

    static int getx(Pos p);
    static int getcols(Pos p);
    static int getcols()
    { return topw? getcols(topw->m_pos) : -1; }

    static int gety(Pos p);
    static int getlines(Pos p);
    static int getlines()
    { return topw? getlines(topw->m_pos): -1; }

};

#endif // WINDOW_HPP
