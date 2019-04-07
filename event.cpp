#include <yaml.h>

#include "scene.hpp"
#include "event.hpp"
#include "window.hpp"

using W = Window;

Event Event::create_from_yaml(const yaml_node_t *node, yaml_document_t *doc, Scenario &s)
{
    Conditions conds;
    Buttons    butts;
    Commands   cmds;
    string     msg;

    if (!node)
        throw game_error("Empty map structure.");
    else if (node->type != YAML_MAPPING_NODE)
        throw game_error("Invalid map stucture.");

    for (auto b = node->data.mapping.pairs.start; b < node->data.mapping.pairs.top; ++b)
    {
        auto node_key = yaml_document_get_node(doc, b->key);
        auto node_value = yaml_document_get_node(doc, b->value);

        if (node_key->type != YAML_SCALAR_NODE)
            throw game_error("Invalid map structure.");

    }

    return Event(conds, butts, cmds, msg);
}

void Event::test()
{
    if (parse_conditions(m_conditions))
        if (m_message.empty()) {
            parse_commands(m_commands);
        } else {
            /* raise window */
            //W::Position p = W::small;
           // vector<W::Menu> menu;

            //W::Builder builder()
        }
}

bool Event::parse_conditions(const vector<Condition> &conditions)
{
    for (auto &f : conditions)
        if (!f()) return false;
    return true;
}

void Event::parse_commands(const vector<Command> &commands)
{
    for (auto &f : commands) f();
}
