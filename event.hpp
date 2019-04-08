#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "utils.hpp"
#include "ui.hpp"

using std::unordered_map;
using std::vector;
using std::string;

class Scenario;

using Condition = string;
using Command   = string;

using Conditions = vector<Condition>;
using Commands = vector<Command>;

//using condition_f =  bool (*)(const string &tupl);
//using command_f   =  void (*)(const string &tupl);

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

class Event;

struct Button {
    Button() {}

    Button(const string &id_)
        : id(id_) {}

    Button(const string &id_,
           const string &label_,
           const Commands &commands_)
        : id(id_),
          label(label_),
          commands(commands_) {}

    string id, label;
    Commands commands;

    /* Pointer to base event */
    Event *event = nullptr;
};

using Buttons = vector<Button>;

class Event {
    string m_id;

    Conditions  m_conditions;
    Buttons     m_buttons;
    Commands    m_commands;
    string      m_message;
    string      m_title;
    W::Position m_position;

    Scenario &m_scenario;

public:
    Event(const string &id,
          const Conditions &conds,
          const Buttons &butts,
          const Commands &cmds,
          const string &msg,
          const string &title,
          const W::Position& pos,
          Scenario &scene)
        : m_id(id),
          m_conditions(conds),
          m_buttons(butts),
          m_commands(cmds),
          m_message(msg),
          m_scenario(scene),
          m_title(title),
          m_position(pos)
    { for (auto &b : m_buttons) b.event = this; }

    static Event create_from_yaml(const string &id, const yaml_node_t *node, yaml_document_t *doc, Scenario &scene);
    static void push_button(Arg button_ptr);

    /* If the check is successful, then execute actions */
    void test();
};

#endif // EVENT_HPP
