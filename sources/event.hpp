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

#include "scenario_constants.hpp"
#include "images.hpp"
#include "utils.hpp"
#include "ui.hpp"
#include "base.hpp"

using std::unique_ptr;
using std::vector;
using std::string;

using event_instructions = vector<string>;

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

struct event_condition;
using event_conditions = unique_ptr<event_condition[]>;

struct event_condition {
  string cond;
  size_t size;
  event_conditions next;
};

struct event_item {
    string label;
    event_instructions instructions;
};

using event_items = vector<event_item>;

class event : public base
{
    event_conditions   m_conditions;
    size_t             m_conditions_size;
    event_items        m_items;
    event_instructions m_instructions;
    unique_ptr<item[]> m_event_menu = nullptr;
    unique_ptr<hook[]> m_event_hooks;
    string             m_message;
    attr_t             m_attribute = DEFAULT_EVENT_ATTRIBUTE;
    string             m_title;
    position           m_position  = DEFAULT_EVENT_SIZE;
    text               m_image;
    image_position     m_image_pos = DEFAULT_IMAGE_POSITION;
    bool               m_happened  = false;
    int                m_count     = 0;

    event(const string &id) : base(id) {}

public:

    event(event &&) = default;
    event(const event &) = delete;
    event& operator=(const event &) = delete;

    /* If the check is successful, then execute actions */
    void run();
    void inc() { ++m_count; }
    bool test();
    bool happened(int count = -1)
    { return count == -1? m_happened : m_happened * (m_count == count); }

    static event& create_from_yaml(const string &id, const yaml_node_t *node, yaml_document_t *doc);
    static void selected(arg_t instructions_ptr);
};

#endif // EVENT_HPP
