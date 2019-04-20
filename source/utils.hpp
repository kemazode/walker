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

#include <string>
#include <cstring>
#include <cstdlib>
#include <ncurses.h>
#include <stdexcept>

#define SIZE(x) (sizeof(x)/sizeof(x[0]))
#define PAIR(A, B) COLOR_PAIR(B*8 + (A + 1))

#define COLOR_BLACK   0
#define COLOR_RED     1
#define COLOR_GREEN   2
#define COLOR_YELLOW  3
#define COLOR_BLUE    4
#define COLOR_MAGENTA 5
#define COLOR_CYAN    6
#define COLOR_WHITE   7

#define FRIEND_COLOR COLOR_GREEN
#define NEUTRAL_COLOR COLOR_YELLOW
#define HOSTILE_COLOR COLOR_RED

#define FILE_COUNT 2
#define FILE_SCENARIOS ".config/walker/scenarios/"
#define FILE_GENERATIONS ".config/walker/generations/"

using std::string;
using std::runtime_error;

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
  text() = default;

  text(const char *str, attr_t attr) {
    lenght = str? strlen(str) : 0;
    cstr = reinterpret_cast<cchar *>(calloc(lenght, sizeof(cchar)));
    
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
    cstr = reinterpret_cast<cchar *>(calloc(lenght, sizeof(cchar)));
    for (size_t i = 0; i < lenght; ++i)
      cstr[i] = t.cstr[i];
  }

  text& operator=(const text &t)
  {
    if (this == &t) return *this;

    lenght = t.lenght;
    free(cstr);

    cstr = reinterpret_cast<cchar *>(calloc(lenght, sizeof(cchar)));
    for (size_t i = 0; i < lenght; ++i)
      cstr[i] = t.cstr[i];
    return *this;
  }
  
  text& operator+(const char *str)
  {
    auto pos = lenght;
    lenght += strlen(str);
    cstr =  reinterpret_cast<cchar *>(realloc(cstr, lenght * sizeof(cchar)));
    for (auto i = pos; i < lenght; ++i)
      {
        cstr[i].symbol = str[i - pos];
        cstr[i].attribute = A_NORMAL;
      }
    return *this;
  }

  text& operator+(const text &str)
  {
    auto pos = lenght;
    lenght += str.lenght;
    cstr = reinterpret_cast<cchar *>(realloc(cstr, lenght * sizeof(cchar)));
    for (auto i = pos; i < lenght; ++i)
        cstr[i] = str.cstr[i - pos];
    return *this;
  }

  ~text() { free(cstr); }

  struct cchar *cstr = nullptr;
  size_t lenght = 0;
};

class game_error : public runtime_error {
public:
    explicit game_error(const string &err) : runtime_error(err) {}
    explicit game_error(const char *err)   : runtime_error(err) {}

    virtual ~game_error();
};

#endif // UTILS_HPP
