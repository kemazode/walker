#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <cstring>
#include "utils.hpp"

#define REQ_EXEC_ITEM (MAX_MENU_COMMAND + 1)

struct Hook {
    int key;
    Action act;
};

struct Menu {   
    Menu() : label(), act() {}
    Menu(const String &l, const Action &a) : label(l), act(a) {}
    String label;
    Action act;
};

class Window {
private:
    PANEL  *m_pan;
    WINDOW *m_win;
    WINDOW *m_sub_t;
    WINDOW *m_sub_m;
    MENU   *m_menu;
    ITEM   **m_items;

    const Hook  *m_hooks;
    size_t  m_hooks_size;

    // Window which has top panel in the panel stack
    static Window *topw;

    int freelines, freecols;
public:
    struct Constructor {

        Constructor() : x(0), y(0), lines(0), cols(0),
            m(nullptr), msize(0), h(nullptr), hsize(0),
            t(nullptr) {}

        Constructor(int x_, int y_, int lines_, int cols_,
                    Menu *m_,       size_t msize_,
                    const Hook *h_, size_t hsize_,
                    const Text &t_) :
            x(x_), y(y_), lines(lines_), cols(cols_),
            m(m_), msize(msize_),
            h(h_), hsize(hsize_), t(&t_) {}

        int x, y, lines, cols;
        Menu *m;
        size_t msize;
        const Hook *h;
        size_t hsize;
        const Text *t;
    };

private: // Only dynamically alloc

    Window(const Constructor &ct = Constructor());
    ~Window();

    void _refresh() const;
    void _exechook(int key) const;

    void _menu_driver(int act) const;
    void _amenu_driver(const Args args) const { menu_driver(args.num); }

    void _print(const Text &t, int x, int y, int size);
    void _print(const Text &t);
    void _aprint(const Args args) { print(*((const Text *) args.ptr)); }

    int _getfreelines() { return freelines; }
    int _getfreecols() { return freecols; }

public:

    // prefix "a" - union "Args" as function argument

    static void apush(const Args args)
    { topw = new Window( *(reinterpret_cast<Constructor *>(const_cast<void *>(args.ptr))) ); }

    static Window* push(const Constructor &c)
    { return (topw = new Window(c)); }

    static void pop()  {
        delete topw;
    }

    // ~Window() changes topw to below Window * (by PANEL stack)
    static void clear()  { while(topw) delete topw; }
    static Window *top() { return topw; }
    static bool has(const Window *w)
    {
        const Window *temp = topw;

        while (temp) {
            if (temp == w)
                return true;
            temp = reinterpret_cast<const Window *>(panel_userptr(panel_below(temp->m_pan)));
        }

        return false;
    }

    // Static methods for TOP Window in the stack
    static bool refresh()
    { return topw? (static_cast<void>(topw->_refresh()), true) : false; }

    static bool exechook(int key)
    { return topw? (static_cast<void>(topw->_exechook(key)), true) : false; }

    static bool amenu_driver(const Args args)
    { return topw? (static_cast<void>(topw->_amenu_driver(args)), true) : false; }

    static bool menu_driver(int act)
    { return topw? (static_cast<void>(topw->_menu_driver(act)), true) : false; }

    static bool aprint(const Args args)
    { return topw? (static_cast<void>(topw->_aprint(args)), true) : false; }

    static bool print(const Text &t)
    { return topw? (static_cast<void>(topw->_print(t)), true) : false; }

    static bool print(const Text &t, int x, int y, int size)
    { return topw? (static_cast<void>(topw->_print(t, x, y, size)), true) : false; }

    static int getfreelines()
    { return topw? topw->_getfreelines() : -1; }

    static int getfreecols()
    { return topw? topw->_getfreecols() : -1; }
};

#endif // WINDOW_HPP
