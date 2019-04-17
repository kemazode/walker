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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "utils.hpp"
#include "base.hpp"

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

class object : public base
{
  int    m_x;
  int    m_y;
  int    m_vision_range;
  cchar  m_symbol;
  string m_obstacles;
  string m_unvisible;

protected:

    object(const string &id, int x, int y, int v, const cchar &c, const string &i, const string &u)
      : base(id),
        m_x(x),
        m_y(y),
        m_vision_range(v),
        m_symbol(c),
        m_obstacles(i),
        m_unvisible(u) {}

public:

    object(const object &)            = delete;
    object& operator=(const object &) = delete;

    static object& create_from_type(const string &id, const string& type, int x, int y);
    static object& create_from_yaml(const string &id, const yaml_node_t *node, yaml_document_t *doc);

    bool move(int x, int y, char path)
    { return movable(path)? (void(m_x += x), void(m_y += y), true) : false; }

    bool movable(char path) const
    { return m_obstacles.find(path) == string::npos; }

    bool visible(char path) const
    { return m_unvisible.find(path) == string::npos; }

    int          x()            const { return m_x; }
    int          y()            const { return m_y; }
    int          vision_range() const { return m_vision_range; }
    const cchar& symbol()       const { return m_symbol;  }

    virtual ~object() = default;
};

class dwarf : public object {
public:    
    dwarf(const string &id, int x, int y);
};

#endif // OBJECT_HPP
