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
using std::vector;

typedef intptr_t arg_t;
typedef void (*fun_t)(arg_t);

struct action
{  
  fun_t function;
  arg_t arg;

  bool empty() const
  { return function == nullptr; }
  void operator()() const
  { function(arg); }
};

struct cchar
{
    char symbol;
    attr_t attribute;
};

struct text
{
  text(const char *str, attr_t attr) {
    lenght = str? strlen(str) : 0;
    cstr = new cchar[lenght];
    
    for (size_t i = 0; i < lenght; ++i) {
        cstr[i].symbol    = str[i];
        cstr[i].attribute = attr;
      }
  }

  text(const char *str)
    : text(str, A_NORMAL) {}

  text(const string &str)
    : text(str.c_str()) {}

  text(const text &t)
  {
    lenght = t.lenght;
    cstr = new cchar[lenght];
    for (size_t i = 0; i < lenght; ++i)
      cstr[i] = t.cstr[i];
  }

  text& operator=(const text &t)
  {
    if (this == &t) return *this;

    lenght = t.lenght;
    delete [] cstr;

    cstr = new cchar[lenght];
    for (size_t i = 0; i < lenght; ++i)
      cstr[i] = t.cstr[i];
    return *this;
  }
  
  ~text()
  { delete [] cstr; }

  struct cchar *cstr;
  size_t lenght;
};

class game_error : public runtime_error {
public:
    explicit game_error(const string &err) : runtime_error(err) {}
    explicit game_error(const char *err)   : runtime_error(err) {}

    virtual ~game_error();
};

#endif // UTILS_HPP
