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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <cstring>
#include <ncurses.h>
#include <stdexcept>
#include <cstdint>

#define SIZE(x) (sizeof(x)/sizeof(x[0]))
#define PAIR(A, B) COLOR_PAIR(B*8 + (A + 1))

#define COLOR_BLACK   	0
#define COLOR_RED	    1
#define COLOR_GREEN	    2
#define COLOR_YELLOW	3
#define COLOR_BLUE	    4
#define COLOR_MAGENTA	5
#define COLOR_CYAN	    6
#define COLOR_WHITE	    7

#define MYCOLOR COLOR_BLUE

#define F_SCENARIOS ".config/walker/scenarios/"
#define F_GENERATIONS ".config/walker/generations/"

using std::string;
using std::runtime_error;

using Arg = intptr_t;

using Fa = void(*)(Arg);
using Fv = void(*)();

template<class F, class A>
struct Action {
    F (*f)(A);
    A args;

    Action() : f(nullptr) {}
    Action(F (*f_)(A), A args_) : f(f_), args(args_) {}

    bool empty()   const { return !f; }
    F operator()() const { return f(args); }
};

template <class F>
struct Action<F, void> {
    F (*f)();

    Action() : f(nullptr) {}
    Action(F (*f_)()) : f(f_) {}

    bool empty()   const { return !f;  }
    F operator()() const { return f(); }
};

using ActionA = Action<void, Arg>;
using ActionV = Action<void, void>;

struct ActionAV {
    ActionAV() : fa() {}
    ActionAV(void (*fv_)()) : arg(0), fv(fv_) {}
    ActionAV(void (*fa_)(Arg), Arg arg_) : arg(1), fa(fa_, arg_) {}

    bool empty() const
    { return fa.empty() and fv.empty(); }

    void operator()() const
    { arg? fa() : fv(); }

    bool arg;
    union {
        ActionA fa;
        ActionV fv;
    };
};

struct cchar {
    cchar() : c(0), attr(0) {}
    cchar(char c_) : c(c_), attr(0) {}
    cchar(char c_, attr_t a_) : c(c_), attr(a_) {}

    /* s (symbol), c (color), a (attribute, not color) */
    cchar& replace_sc(const cchar &cc)
    {
        c = cc.c;
        attr &= ~COLOR_PAIR( PAIR_NUMBER(attr)    );
        attr |=  COLOR_PAIR( PAIR_NUMBER(cc.attr) );
        return *this;
    }

    char c;
    attr_t attr;
};

struct Text {

    Text(): text(nullptr), len(0) {}

    Text (const Text &t) {
        len = t.len;
        text = new cchar[len];
        for (size_t i = 0; i < len; ++i)
            text[i] = t.text[i];

    }

    Text (const string &s) : Text(s.data()) {}

    Text(const char *s, attr_t at) : Text(s) {
        for (size_t i = 0; i < len; ++i)
            text[i].attr = at;
    }

    Text(const char *s) {
        len = strlen(s);

        text = new cchar[len] {};
        for (size_t i = 0; i < len; ++i)
            text[i].c = s[i];
    }

    Text& operator=(const Text &t) {
        if (this == &t)
            return *this;
        delete [] text;

        len = t.len;

        text = new cchar[len];
        for (size_t i = 0; i < len; ++i)
            text[i] = t.text[i];

        return *this;
    }

    cchar& operator[](size_t i)
    { return this->text[i]; }

    cchar& at(size_t i)
    {
        if (i >= len) throw std::out_of_range("The value entered is out of range");
        return this->text[i];
    }

    cchar* begin()
    { return text; }

    cchar* end()
    { return text + len; }

    bool empty() const
    { return len == 0; }

    ~Text() {
        delete [] text;
    }

    cchar* text;
    size_t len;
};

class game_error : public runtime_error {
public:
    explicit game_error(const string &err) : runtime_error(err) {}
    explicit game_error(const char *err)   : runtime_error(err) {}

    virtual ~game_error();
};

#endif // UTILS_HPP
