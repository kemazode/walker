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

#include <unordered_map>
#include <fstream>

#include "map.hpp"
#include "utils.hpp"
#include "perlin.hpp"
#include "yaml.h"

#define F_GENERATE "Generation.txt"

using std::ofstream;
using std::unordered_map;
using std::ifstream;
using std::to_string;

/* Generation (count).txt */
static string nextgen(const string& s, int count);

/* Textures to generate */
static char textures[] = { '~', '#', '\'', '`' };

/* For non-static Map::decorate() */
static unordered_map<char, attr_t> map_attrs = {
    {'~',  PAIR(COLOR_BLUE, COLOR_BLACK)  | A_INVIS},
    {'#',  PAIR(COLOR_WHITE, COLOR_BLACK) | A_INVIS},
    {'\'', PAIR(COLOR_GREEN, COLOR_BLACK) | A_INVIS},
    {'`',  PAIR(COLOR_GREEN, COLOR_BLACK) | A_INVIS},
    {'.',  PAIR(COLOR_CYAN, COLOR_BLACK)  | A_INVIS}
};

void Map::push(const string &s)
{
    if (s.empty())
        throw game_error("Line " + to_string(m_height + 1)
                         + " is empty.");

    int slen = int(s.length());

    if (m_width == 0)
        m_width = slen;

    else if (m_width != slen)
        throw game_error("The lenght of line number " + to_string(m_height + 1) +
                         " (" + to_string(slen) +
                         ") does not match the specified length (" +
                         to_string(m_width) + ").");
    ++m_height;
    m_strs.push_back(s);
}

Map::Map(const string &id, const string &map, int w, int h) : m_id(id)
{
    if (map.empty())
        throw game_error("Map is empty.");
    else if (w == 0)
        throw game_error("Map width is zero.");
    else if (h == 0)
        throw game_error("Map height is zero.");

    m_x = m_y = 0;

    m_width = w;
    m_height = 0;

    for (string::size_type pos = 0, newline = 0; h--; pos = (newline + 1) )
    {
        newline = map.find('\n', pos);

        string &&temp = map.substr(pos, newline - pos);
        this->push(temp);              
    }

}

Map Map::create_from_yaml(const string &id, const yaml_node_t *node, yaml_document_t *doc)
{
    string map;
    int w = 0, h = 0;

    if (!node)
        throw game_error("Empty map structure.");
    else if (node->type != YAML_MAPPING_NODE)
        throw game_error("Invalid map stucture.");

    for (auto b = node->data.mapping.pairs.start; b < node->data.mapping.pairs.top; ++b)
    {
        auto node_key = yaml_document_get_node(doc, b->key);
        auto node_value = yaml_document_get_node(doc, b->value);

        if (node_key->type != YAML_SCALAR_NODE or node_value->type != YAML_SCALAR_NODE)
            throw game_error("Invalid map structure.");

        const char *key = reinterpret_cast<const char *>(node_key->data.scalar.value);
        const char *value = reinterpret_cast<const char *>(node_value->data.scalar.value);

        if (!strcmp(key, "width"))
            w = atoi(value);
        else if (!strcmp(key, "height"))
            h = atoi(value);
        else if (!strcmp(key, "text"))
            map = value;
        else
            throw game_error( string("Found unknown field \"") + key + "\" in the map structure.");
    }

    return Map(id, map, w, h);
}

void Map::decorate()
{
    char temp;
    try {
        for (auto& t : m_strs)
            for (auto& c : t)
                c.attr = map_attrs.at(temp = c.c);

    } catch (std::out_of_range &) {
        throw game_error( string("Sorry, but file has incorrect symbol '") + temp + "'.");
    }
}

void Map::generate(const string& f, int w, int h)
{
    srand(unsigned(time(nullptr)));
    perlin_set_seed(rand());

    ofstream fil;

    fil.exceptions(std::ios_base::failbit);

    try {
        fil.open(f);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                auto seed = unsigned(perlin2d(x, y, 0.05f, 10) * 10);
                fil << textures[seed % SIZE(textures)];
            }
            fil << '\n';
        }

    } catch (std::ios::failure &) {
        if (!fil.is_open()) throw game_error("Can't create file \"" + f + "\".");
        else throw game_error("Something went wrong when writing to " + f);
    }
}

string Map::generate(int w, int h)
{
    const char* home = getenv("HOME");

    if (home == nullptr)
        throw game_error("HOME environment variable not set.");

    string folder = home;
    folder = folder + '/' + F_GENERATIONS;

    ifstream fil;
    int count = 0;
    string filename = F_GENERATE;

    do {
        fil.close();
        if (count > 0)
            filename = nextgen(F_GENERATE, count);
        fil.open(folder + filename);
        ++count;
    } while (fil.is_open());

    fil.close();
    string f = folder + filename;

    generate(f, w, h);
    return f;
}

string nextgen(const string& str, int count)
{
    string s = str;
    s.insert(s.rfind('.'), " (" + to_string(count) + ")");
    return s;
}
