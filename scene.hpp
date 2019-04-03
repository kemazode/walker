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

using shobject = std::shared_ptr<Object>;
using std::list;

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

class Scenario {
private:
    String m_file;

    int m_lines, m_cols;

    Map m_source;
    Map m_render;

    list<shobject> m_objects;
    shobject m_player;

    inline bool abroad(int x, int y)
    { return x >= m_source.width() || y >= m_source.height() || x < 0 || y < 0; }

    inline bool abroadx(int x)
    { return x >= m_source.width() || x < 0; }

    inline bool abroady(int y)
    { return y >= m_source.height() || y < 0; }

    inline bool physic_movement_allowed(int x, int y, const Object& obj);
    void physic_light_render(const Object& viewer);

    void parse_yaml();
    void parse_yaml_objects(const yaml_node_t *node, yaml_document_t *doc);
    void parse_yaml_maps(const yaml_node_t *node, yaml_document_t *doc);
    void update_render_map();

public:    

    Scenario(const String &f, int l, int c);

    void load(const String &f);

    int height() const { return m_source.height(); }
    int width() const  { return m_source.width(); }

    int getx() const { return m_source.getx(); }
    int gety() const { return m_source.gety(); }

    void move_player(int x, int y);    

    void set_view(int x, int y);
    void move_view(int x, int y)
    { set_view(m_source.getx() + x, m_source.gety() + y); }

    const vector<Text>& get_render_map() const { return m_render.getstrs(); }
};

#endif // SCENE_HPP
