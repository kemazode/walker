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

#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <memory>

#include "map.hpp"
#include "utils.hpp"
#include "object.hpp"

using uobject = std::unique_ptr<Object>;
using std::list;

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

class Scenario {
private:
    String m_file;

    int m_lines, m_cols;

    Map m_source;
    Map m_render;

    list<uobject> m_objects;
    uobject* m_player;

    inline bool physic_movement_allowed(int x, int y, const Object& obj);
    void physic_light_render(const Object& viewer);

    void parse_yaml();
    void parse_yaml_objects(const yaml_node_t *node, yaml_document_t *doc);
    void parse_yaml_maps(const yaml_node_t *node, yaml_document_t *doc);

public:    

    void load(const String &f);
    void clear();

    int height() const { return m_source.height(); }
    int width() const  { return m_source.width(); }

    int getx() const { return m_source.getx(); }
    int gety() const { return m_source.gety(); }

    void move_player(int x, int y);
    void set_view(int x, int y);
    void move_view(int x, int y) { set_view(m_source.getx() + x, m_source.gety() + y); }
    void set_display(int l, int c) { m_lines = l; m_cols = c; }

    void update_render_map();

    const vector<Text>& get_render_map() const { return m_render.getstrs(); }
};

#endif // SCENE_HPP
