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
#include <forward_list>

#include "utils.hpp"
#include "ui.hpp"
#include "base.hpp"

using std::forward_list;
using std::vector;
using std::string;

class Scenario;

using Condition = string;
using Command   = string;
using Conditions = vector<Condition>;
using Commands = vector<Command>;

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

class Event;

struct Button {

    Button() {}

    Button(const string &label_,
           const Commands &commands_)
        : label(label_),
          commands(commands_) {}

    string label;
    Commands commands;

    /* Pointer to base event */
    Event *event = nullptr;
};

using Buttons = vector<Button>;

class Event : public Base {

    Conditions  m_conditions;
    Buttons     m_buttons;
    Commands    m_commands;
    string      m_message;
    string      m_title;
    W::Position m_position;

    Scenario &m_scenario;

public:
    Event(const string &id, Scenario &scene) :
        Base(id),
        m_scenario(scene)
    {}

    Event(const string &id,
          const Conditions &conds,
          const Buttons &butts,
          const Commands &cmds,
          const string &msg,
          const string &title,
          const W::Position& pos,
          Scenario &scene)
        : Base(id),
          m_conditions(conds),
          m_buttons(butts),
          m_commands(cmds),
          m_message(msg),
          m_scenario(scene),
          m_title(title),
          m_position(pos)
    {}


    static Event* create_from_yaml(const string &id, const yaml_node_t *node, yaml_document_t *doc, Scenario &scene);
    static void push_button(Arg button_ptr);

    /* If the check is successful, then execute actions */
    void test();
};

#endif // EVENT_HPP
