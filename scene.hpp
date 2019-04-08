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

#include <forward_list>
#include <string>
#include <memory>

#include "map.hpp"
#include "event.hpp"

class Object;
class Event;

using std::shared_ptr;
using std::forward_list;
using std::string;


typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

class Scenario {

    string m_file;

    int m_lines, m_cols;

    Map m_source;
    Map m_render;

    vector<Event> m_events;

    /* Polymorphism */
    forward_list<shared_ptr<Object>> m_objects;
    shared_ptr<Object> m_player;

    inline bool abroad(int x, int y)
    { return x >= m_source.width() || y >= m_source.height() || x < 0 || y < 0; }

    inline bool abroadx(int x)
    { return x >= m_source.width() || x < 0; }

    inline bool abroady(int y)
    { return y >= m_source.height() || y < 0; }

    shared_ptr<Object> get_object(const string& id);

    void render_los(const Object& viewer);

    void render_set_visible(int x, int y);
    void source_set_detected(int x, int y);

    void parse_yaml();
    void parse_yaml_objects(const yaml_node_t *node, yaml_document_t *doc);
    void parse_yaml_maps(const yaml_node_t *node, yaml_document_t *doc);
    void parse_yaml_events(const yaml_node_t *node, yaml_document_t *doc);

public:    

    Scenario(const string &f, int l, int c);

    void load(const string &f);

    int height() const { return m_source.height(); }
    int width()  const { return m_source.width(); }
    int getx()   const { return m_source.getx(); }
    int gety()   const { return m_source.gety(); }

    void set_view(int x, int y);
    void move_player(int x, int y);    
    void move_view(int x, int y)
    { set_view(m_source.getx() + x, m_source.gety() + y); }

    void turn();
    void render();

    /* For events */
    bool parse_condition(const string& cond);
    void parse_command(const string& comm);

    bool parse_conditions(const Conditions &conditions);
    void parse_commands(const Commands &commands);
};

#endif // SCENE_HPP
