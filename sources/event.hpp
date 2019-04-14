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

#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>
#include <vector>
#include <memory>
#include <forward_list>
#include <variant>

#include "images.hpp"
#include "utils.hpp"
#include "ui.hpp"
#include "base.hpp"

using std::unique_ptr;
using std::forward_list;
using std::vector;
using std::string;
using std::variant;

using Commands = vector<string>;

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

class scenario;
class Event;

struct Condition;
using Conditions = unique_ptr<Condition[]>;

struct Condition {
  string cond;
  size_t size;
  Conditions next;
};

struct Item {
    string label;
    Commands commands;
};

using Items = vector<Item>;

class Event : public Base {

    Conditions     m_conditions;
    size_t         m_conditions_size;
    Items          m_items;
    Commands       m_commands;
    string         m_message;
    string         m_title;
    position       m_position;
    text           m_image;
    image_position m_image_pos;

    bool m_happened = false;

    Event(const string &id) : Base(id) {}

public:

    /* If the check is successful, then execute actions */
    void test();
    bool happened() { return m_happened; }

    static Event* create_from_yaml(const string &id, const yaml_node_t *node, yaml_document_t *doc);
    static void selected(arg_t commands_ptr);
};

#endif // EVENT_HPP
