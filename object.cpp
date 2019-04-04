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

#include "object.hpp"
#include "yaml.h"

Object* Object::create(const String& obj, int x, int y)
{
    if (obj == "Dwarf")
        return new Dwarf(x, y);
    else
        throw game_error("Unknown object type \"" + obj + "\".");
}

Object* Object::create_from_yaml(const yaml_node_t *node, yaml_document_t *doc)
{
    String type;
    int x = 0, y = 0;

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

        if (!strcmp(key, "type"))
            type = value;
        else if (!strcmp(key, "x"))
            x = atoi(value);
        else if (!strcmp(key, "y"))
            y = atoi(value);
        else
            throw game_error( String("Found unknown field \"") + key + "\" in the object structure.");
    }

    return create(type, x, y);
}

bool Object::move(int x, int y, char path)
{
    if (m_obstacles.find(path) == String::npos)
    {
        m_x += x;
        m_y += y;
        return true;
    } else
        return false;
}

bool Object::visible(char path) const
{
    if (m_unvisible.find(path) == String::npos)
        return true;
    else
        return false;
}

