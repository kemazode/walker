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

#include "scenario_constants.hpp"
#include "object.hpp"

object* object::create_from_type(const string &id, const string& type, int x, int y)
{
  if (type == DWARF_TYPE)
    return new dwarf(id, x, y);
  else
    throw game_error("Unknown object type \"" + type + "\".");
}

object* object::create_from_yaml(const string &id, const yaml_node_t *node, yaml_document_t *doc)
{
  string type;
  int x = 0;
  int y = 0;

  if (!node)
    throw game_error("Empty stucture.");
  else if (node->type != YAML_MAPPING_NODE)
    throw game_error("Invalid object stucture.");

  for (auto b = node->data.mapping.pairs.start; b < node->data.mapping.pairs.top; ++b)
    {
      auto node_key = yaml_document_get_node(doc, b->key);
      auto node_value = yaml_document_get_node(doc, b->value);

      if (node_key->type != YAML_SCALAR_NODE or node_value->type != YAML_SCALAR_NODE)
        throw game_error("Invalid object structure.");

      const char *key = reinterpret_cast<const char *>(node_key->data.scalar.value);
      const char *value = reinterpret_cast<const char *>(node_value->data.scalar.value);

      if (!strcmp(key, YAML_OBJECT_TYPE))
        type = value;

      else if (!strcmp(key, YAML_OBJECT_POSITION_X))
        x = atoi(value);

      else if (!strcmp(key, YAML_OBJECT_POSITION_Y))
        y = atoi(value);
      else
        throw game_error( string("Found unknown field \"") + key + "\" in the object structure.");
    }
  return create_from_type(id, type, x, y);
}

bool object::move(int x, int y, char path)
{
  if (m_obstacles.find(path) == string::npos)
    {
      m_x += x;
      m_y += y;
      return true;
    }
  return false;
}

bool object::visible(char path) const
{
  return m_unvisible.find(path) == string::npos;
}

dwarf::dwarf(const string &id, int x, int y)
  : object(id, x, y,
           DWARF_VISION_RANGE,
           cchar{DWARF_SYMBOL, DWARF_ATTR},
           DWARF_OBSTACLES,
           DWARF_UNVISIBLE)
{};
