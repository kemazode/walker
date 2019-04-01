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

#define MYCOLOR COLOR_GREEN

#define F_SCENARIOS ".config/walker/scenarios/"
#define F_GENERATIONS ".config/walker/generations/"

using std::vector;
using String = std::string;

typedef union Args {
    Args(const void *p) : ptr(p) {}
    Args(int n)   : num(n) {}
    Args() : ptr(nullptr) {}
    int num;
    const void* ptr;
} Args;

typedef void (*Fa)(Args);
typedef void (*Fv)();

typedef struct Action {

    Action(Fa f, const Args &a) : fa(f), args_needed(true), args(a) {}

    Action(Fv f) : fv(f), args_needed(false) {}

    Action() : fa(nullptr), args_needed(false), args() {}

    bool empty() const { return fa == nullptr && fv == nullptr; }

    void exec() const {
        if (args_needed)
            fa(args);
        else
            fv();
    }

    union {
        Fa fa;
        Fv fv;
    };

    bool args_needed;
    Args args;
} Action;

struct cchar {
    cchar() : c(0), attr(0) {}
    cchar(char c_, attr_t a_) : c(c_), attr(a_) {}
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

    Text (const String &s) : Text(s.data()) {}

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

    void clear() {
        len = 0;
        delete [] text;
        text = nullptr;
    }

    ~Text() {
        delete [] text;
    }

    cchar* text;
    size_t len;
};

using std::runtime_error;

class game_error : public runtime_error {
public:
    explicit game_error(const String &err) : runtime_error(err) {}
    explicit game_error(const char *err)   : runtime_error(err) {}

    virtual ~game_error();
};

//class map_lenght_error : public game_error {
//public:
//    explicit map_lenght_error(const String &err) : game_error(err) {}
//    explicit map_lenght_error(const char *err)   : game_error(err) {}

//    virtual ~map_lenght_error();
//};

#endif // UTILS_HPP
