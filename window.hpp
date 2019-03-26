#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <cstring>
#include <vector>
//#include <utility>
#include "utils.hpp"

using std::vector;

#define REQ_EXEC_ITEM (MAX_MENU_COMMAND + 1)

class Window {
private:
    PANEL  *m_pan;
    WINDOW *m_win;
    WINDOW *m_sub_t;
    WINDOW *m_sub_m;
    MENU   *m_menu;
    ITEM   **m_items;

    // Window which has top panel in the panel stack
    static Window *topw;

    int freelines, freecols;
public:

    enum Pos {
        POS_FULL,
        POS_AVER,
        POS_SMLL,
    };

    struct Menu {
        Menu() : label(), act() {}
        Menu(const String &l, const Action &a) : label(l), act(a) {}
//        Menu(const Menu &m) {
//            label = m.label;
//            act = m.act;
//        }

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

        const Pos p;
        vector<Menu> &m;
        const vector<Hook> &h;
        const Text &t;
    };

private:

    enum Pos m_pos;
    const vector<Hook> *m_hooks;

    Window(const Constructor &ct);
    ~Window();

    void _refresh() const;
    void _exechook(int key) const;

    void _menu_driver(int act) const;

    void _print(const vector<Text> &vt, int x, int y);
    void _print(const Text &t);

    int _getfreelines() const
    { return freelines; }

    int _getfreecols() const
    { return freecols; }

public:
    // prefix "a" - union "Args" as function argument

    static Window *push(const Constructor &c)
    { return (topw = new Window(c)); }

//    template <typename ...Args>
//    static Window *emplace_push(Args&&... args)
//    { return push(std::forward<Args>(args)...); }

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

    // Static methods for TOP Window in the stack

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

    static int getfreelines()
    { return topw? topw->_getfreelines() : -1; }

    static int getfreecols()
    { return topw? topw->_getfreecols() : -1; }
};

#endif // WINDOW_HPP
