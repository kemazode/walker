#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "utils.hpp"

using std::unordered_map;
using std::vector;
using std::string;
using variable_map = unordered_map<string, int>;

class Scenario;
union Args;

using Condition = Action<bool, void>;
using Command   = ActionAV;

using Conditions = vector<Condition>;
using Commands = vector<Command>;

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

struct Button {
    Button(const string &l, const Commands &cmds)
        : label(l), commands(cmds) {}
    string label;
    Commands commands;
};

using Buttons = vector<Button>;

class Event {
    Conditions m_conditions;
    Buttons    m_buttons;
    Commands   m_commands;
    string m_message;

    bool parse_conditions(const Conditions &conditions);
    void parse_commands(const Commands &commands);

public:
    Event(const Conditions &conds, const Buttons &butts, const Commands &cmds, const string &msg)
        : m_conditions(conds), m_buttons(butts), m_commands(cmds), m_message(msg) {}

    static Event create_from_yaml(const yaml_node_t *node, yaml_document_t *doc, Scenario &s);

    /* If the check is successful, then execute actions */
    void test();
};

#endif // EVENT_HPP
