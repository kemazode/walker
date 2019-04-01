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

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

class Object
{
protected:
    int m_x, m_y;
    cchar m_c;

    String m_impass;
public:
    Object(const cchar &c, const String &imp) : m_x(0), m_y(0), m_c(c), m_impass(imp) {}

    Object(int x, int y, const cchar &c, const String &imp) :
        m_x(x), m_y(y), m_c(c), m_impass(imp) {}

    static Object* create(const String& obj, int x, int y);
    static Object* create_from_yaml(const yaml_node_t *node, yaml_document_t *doc);

    virtual ~Object() {}

    virtual bool move(int x, int y, char path);

    int          getx() const     { return m_x; }
    int          gety() const     { return m_y; }
    const cchar& getcchar() const { return m_c; }
};

class Dwarf : public Object {
public:
    Dwarf() : Object(cchar('@', A_BOLD), "~#") {}
    Dwarf(int x, int y) : Object(x, y, cchar('@', A_BOLD), "~#") {}

    bool move(int x, int y, char path)
    { return Object::move(x, y, path); }
};

#endif // OBJECT_HPP
