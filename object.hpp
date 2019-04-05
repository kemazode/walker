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

#include <string>
using std::string;

#include "utils.hpp"

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

class Object
{
protected:
    int m_x, m_y;
    int m_vision_range;

    cchar m_c;

    string m_obstacles;
    string m_unvisible;
public:

    Object(int x, int y, int visr, const cchar &c, const string &imp, const string &unvis) :
        m_x(x), m_y(y), m_vision_range(visr),
        m_c(c),
        m_obstacles(imp),
        m_unvisible(unvis) {}

    static Object* create(const string& obj, int x, int y);
    static Object* create_from_yaml(const yaml_node_t *node, yaml_document_t *doc);

    virtual ~Object() {}

    virtual bool move(int x, int y, char path);
    virtual bool visible(char path) const;

    int              getx() const { return m_x; }
    int              gety() const { return m_y; }
    int  get_vision_range() const { return m_vision_range; }
    const cchar& getcchar() const { return m_c; }
};

class Dwarf : public Object {
public:    
    Dwarf(int x, int y) : Object(x, y, 10, cchar('@', A_BOLD), "~#", "#") {}
};

#endif // OBJECT_HPP
