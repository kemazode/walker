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

#include <yaml.h>

#include "scene.hpp"
#include "event.hpp"

using std::unique_ptr;

static void parse_instructions_from_yaml (const yaml_node_t *node, yaml_document_t *doc, event_instructions &cmds);
static void parse_conditions_from_yaml   (const yaml_node_t *node, yaml_document_t *doc, event_conditions &conds, size_t &size);
static void parse_items_from_yaml        (const yaml_node_t *node, yaml_document_t *doc, event_items &items);
static void parse_string_from_yaml       (const yaml_node_t *node, string &msg);
static void parse_position_from_yaml     (const yaml_node_t *node, position &p);
static void parse_image_from_yaml        (const yaml_node_t *node, yaml_document_t *doc, text &im, image_position &im_pos);
static void parse_attribute_from_yaml    (const yaml_node_t *node, attr_t &attr);

/* Dynamic alloc */
event& event::create_from_yaml(const string &id, const yaml_node_t *node, yaml_document_t *doc)
{
  auto event = new class event(id);

  if (!node)
    throw game_error("Empty event structure.");
  else if (node->type != YAML_MAPPING_NODE)
    throw game_error("Invalid event stucture.");

  for (auto b = node->data.mapping.pairs.start; b < node->data.mapping.pairs.top; ++b)
    {
      auto node_key = yaml_document_get_node(doc, b->key);
      auto node_value = yaml_document_get_node(doc, b->value);

      if (node_key->type != YAML_SCALAR_NODE)
        throw game_error("Invalid event structure.");

      const char *key = reinterpret_cast<const char *>(node_key->data.scalar.value);

      if (!strcmp(YAML_EVENT_CONDITIONS, key))
        parse_conditions_from_yaml(node_value, doc, event->m_conditions, event->m_conditions_size);

      else if (!strcmp(YAML_EVENT_TITLE, key))
        parse_string_from_yaml(node_value, event->m_title);

      else if (!strcmp(YAML_EVENT_SIZE, key))
        parse_position_from_yaml(node_value, event->m_position);

      else if (!strcmp(YAML_EVENT_MESSAGE, key))
        parse_string_from_yaml(node_value, event->m_message);

      else if (!strcmp(YAML_EVENT_IMAGE, key))
        parse_image_from_yaml(node_value, doc, event->m_image, event->m_image_pos);

      else if (!strcmp(YAML_EVENT_ITEMS, key))
        parse_items_from_yaml(node_value, doc, event->m_items);

      else if (!strcmp(YAML_EVENT_COMMANDS, key))
        parse_instructions_from_yaml(node_value, doc, event->m_instructions);

      else if (!strcmp(YAML_EVENT_ATTRIBUTE, key))
        parse_attribute_from_yaml(node_value, event->m_attribute);
      else
        throw game_error( string("Invalid field in the event structure: \"") + key + "\"");
    }

  constexpr int HOOKS_SIZE = 5;
  constexpr int HOOK_MENU = 0;
  event->m_event_hooks.reset(new hook[HOOKS_SIZE]
  {
    hook('\n',     {fun_t(window_menu_driver), REQ_EXEC_ITEM}), // HOOK_MENU
    hook('\n',     {fun_t(window_pop), 0}),
    hook(KEY_DOWN, {fun_t(window_menu_driver), REQ_DOWN_ITEM}),
    hook(KEY_UP,   {fun_t(window_menu_driver), REQ_UP_ITEM}),
    {0, {nullptr, 0}}
  }) ;

  if (!event->m_items.empty())
    {
      event->m_event_menu.reset(new item[event->m_items.size() + 1]);

      for (size_t i = 0; i < event->m_items.size(); ++i)
        event->m_event_menu.get()[i] =
        {
          event->m_items[i].label.c_str(),
          { selected, arg_t(&event->m_items[i].instructions) }
        };
      event->m_event_menu.get()[event->m_items.size()] = {};
    }
  else
    {
      event->m_event_hooks.get()[HOOK_MENU].action.function = selected;
      event->m_event_hooks.get()[HOOK_MENU].action.arg = arg_t(&event->m_instructions);
    }

  return *event;
}

void event::run()
{
  m_happened = true;

  if (m_message.empty())
    scenario_parse_instructions(m_instructions);
  else
   {
      auto &&menu_build = builder(m_position, m_event_menu.get(), m_event_hooks.get(),
                                  m_message, m_title, OPTION_NORMAL, FORMAT_CENTER,
                                  &m_image, m_image_pos, FORMAT_CENTER_RIGHT, m_attribute);
      window_push(menu_build);
    }
}

bool event::test()
{
  bool result;
  if ((result = scenario_parse_conditions(m_conditions, m_conditions_size)))
    this->run();
  return result;
}

void event::selected(arg_t instructions_ptr)
{
  auto instructions = reinterpret_cast<event_instructions *>(instructions_ptr);

  /* Execute instructions assigned to the item */
  scenario_parse_instructions(*instructions);
}

void parse_instructions_from_yaml(const yaml_node_t *node, yaml_document_t *doc, event_instructions &cmds)
{
  if (node->type != YAML_SEQUENCE_NODE)
    throw game_error("Incorrectly set \"do\" sequence in the event structure.");

  for (auto seq = node->data.sequence.items.start; seq < node->data.sequence.items.top; ++seq) {
      auto seq_value = yaml_document_get_node(doc, *seq);
      if (seq_value->type != YAML_SCALAR_NODE)
        throw game_error("Incorrectly set \"do\" sequence in the event structure.");

      const char *value = reinterpret_cast<const char *>(seq_value->data.scalar.value);
      cmds.emplace_back( value );
    }
}

void parse_conditions_from_yaml(const yaml_node_t *node, yaml_document_t *doc, event_conditions &conds, size_t &size)
{
  if (node->type != YAML_SEQUENCE_NODE)
    throw game_error("Incorrectly set \"if\" sequence in the event structure.");

  size = size_t(node->data.sequence.items.top - node->data.sequence.items.start);
  conds.reset(new event_condition[size]);

  int i = 0;
  for (auto seq = node->data.sequence.items.start; seq < node->data.sequence.items.top; ++seq, ++i) {
      auto seq_value = yaml_document_get_node(doc, *seq);

      if (seq_value->type == YAML_SEQUENCE_NODE)
        {
          parse_conditions_from_yaml(seq_value, doc, conds.get()[i].next, conds.get()[i].size);
        }
      else if (seq_value->type == YAML_SCALAR_NODE)
        {
          const char *value = reinterpret_cast<const char *>(seq_value->data.scalar.value);
          conds.get()[i].cond = value;
          conds.get()[i].next = nullptr;
        }
      else throw game_error("Incorrectly set \"if\" sequence in the event structure.");
    }
}

void parse_items_from_yaml(const yaml_node_t *node, yaml_document_t *doc, event_items &items)
{
  if (node->type != YAML_SEQUENCE_NODE)
    throw game_error("Incorrectly set \"press\" struct in the items structure.");

  for (auto b = node->data.sequence.items.start; b < node->data.sequence.items.top; ++b)
    {
      auto seq_value = yaml_document_get_node(doc, *b);

      if (seq_value->type != YAML_MAPPING_NODE)
        throw game_error("Incorrectly set \"press\" struct in the items structure.");

      items.push_back(event_item());
      auto &item = items.back();

      for (auto b = seq_value->data.mapping.pairs.start; b < seq_value->data.mapping.pairs.top; ++b)
        {
          auto node_key = yaml_document_get_node(doc, b->key);
          auto node_value = yaml_document_get_node(doc, b->value);

          if (node_key->type != YAML_SCALAR_NODE)
            throw game_error("Incorrectly set \"press\" struct in the items structure.");

          const char *key = reinterpret_cast<const char *>(node_key->data.scalar.value);

          if (!strcmp(YAML_EVENT_ITEM_LABEL, key))
            parse_string_from_yaml(node_value, item.label);

          else if (!strcmp(YAML_EVENT_ITEM_COMMANDS, key))
            parse_instructions_from_yaml(node_value, doc, item.instructions);
          else
            throw game_error( string("Invalid field in items structure: \"") + key + "\"");
        }
    }
}

static void parse_string_from_yaml(const yaml_node_t *node, string &msg)
{
  if (node->type != YAML_SCALAR_NODE)
    throw game_error("Incorrectly field in the message structure.");

  const char *value = reinterpret_cast<const char *>(node->data.scalar.value);
  msg = value;
}

static void parse_position_from_yaml(const yaml_node_t *node, position &p)
{
  if (node->type != YAML_SCALAR_NODE)
    throw game_error("Incorrectly \"position\" field in the position structure.");

  const char *value = reinterpret_cast<const char *>(node->data.scalar.value);

  if (!strcmp(value, YAML_WINDOW_SIZE_SMALL))
    p = POSITION_SMALL;

  else if (!strcmp(value, YAML_WINDOW_SIZE_AVERAGE))
    p = POSITION_AVERAGE;

  else if (!strcmp(value, YAML_WINDOW_SIZE_FULL))
    p = POSITION_FULL;
  else
    throw game_error(string("Invalid position value \"") + value + "\" in the position structure.");

}

static void parse_image_from_yaml(const yaml_node_t *node, yaml_document_t *doc, text &im, image_position &im_pos)
{
  if (node->type != YAML_MAPPING_NODE)
    throw game_error("Incorrectly \"image\" field in the event structure.");

  for (auto b = node->data.mapping.pairs.start; b < node->data.mapping.pairs.top; ++b)
    {
      auto node_key = yaml_document_get_node(doc, b->key);
      auto node_value = yaml_document_get_node(doc, b->value);

      if (node_key->type != YAML_SCALAR_NODE or node_value->type != YAML_SCALAR_NODE)
        throw game_error("Invalid image structure.");

      const char *key = reinterpret_cast<const char *>(node_key->data.scalar.value);
      const char *value = reinterpret_cast<const char *>(node_value->data.scalar.value);

      if (!strcmp(YAML_EVENT_IMAGE_FILE, key))
        im = images_find(value);

      else if (!strcmp(YAML_EVENT_IMAGE_POSITION, key))
        {
          if (!strcmp(value, YAML_IMAGE_POSITION_TOP))
            im_pos = IMAGE_POSITION_TOP;

          else if (!strcmp(value, YAML_IMAGE_POSITION_LEFT))
            im_pos = IMAGE_POSITION_LEFT;
          else
            throw game_error(string("Invalid image position value \"") + value + "\" in the image position structure.");
        }
      else
        throw game_error(string("Invalid image value \"") + value + "\" in the image structure.");
    }
}

static void parse_attribute_from_yaml (const yaml_node_t *node, attr_t &attr)
{
  if (node->type != YAML_SCALAR_NODE)
    throw game_error("Incorrectly \"color\" field in the event structure.");

  const char *value = reinterpret_cast<const char *>(node->data.scalar.value);

  if (!strcmp(value, YAML_WINDOW_ATTRIBUTE_HOSTILE))
    attr = PAIR(HOSTILE_COLOR, COLOR_BLACK);

  else if (!strcmp(value, YAML_WINDOW_ATTRIBUTE_NEUTRAL))
    attr = PAIR(NEUTRAL_COLOR, COLOR_BLACK);

  else if (!strcmp(value, YAML_WINDOW_ATTRIBUTE_FRIEND))
    attr = PAIR(FRIEND_COLOR, COLOR_BLACK);
  else
    throw game_error(string("Invalid color value \"") + value + "\" in the color structure.");
}
