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

#include <cstdio>
#include <limits>
#include <cstring>
#include <sstream>
#include <yaml.h>
#include <cmath>
#include <exception>
#include "scene.hpp"

using std::to_string;

static const char *parse_error = "YAML configuration does not match the scenario specification.";

Scenario::Scenario(const String &f, int l, int c) :
    m_lines(l),
    m_cols(c),
    m_player(new Object(0, 0, 10, cchar(), "", ""))
{
    load(f);

    set_view(m_player->getx() - m_cols/2,
             m_player->gety() - m_lines/2);

    m_source.decorate();
    update_render_map();
}

void Scenario::load(const String& f)
{
    m_file = f;
    parse_yaml();
}

void Scenario::update_render_map()
{

    m_render = m_source;
    render_los(*m_player);

    for (auto& obj : m_objects)
        m_render.at(obj->getx(), obj->gety()).replace_sc(obj->getcchar());
}

void Scenario::move_player(int x, int y)
{    
    int px = m_player->getx();
    int py = m_player->gety();

    /* Return view to player */
    set_view(px - m_cols / 2, py - m_lines / 2);

    int npx = px + x;
    int npy = py + y;

    if (abroad(npx, npy))
        return;

    if (m_player->move(x, y, m_source.at(npx, npy).c))
    {
        set_view(npx - m_cols / 2, npy - m_lines / 2);
        update_render_map();
    }
}

void Scenario::set_view(int x, int y)
{
    if (!abroadx(x) && !abroadx(x + m_cols - 1))
        m_source.setx(x);

    if (!abroady(y) && !abroady(y + m_lines - 1))
        m_source.sety(y);
}

void Scenario::render_set_visible(int x, int y)
{
    m_render.at(x, y).attr &= ~(A_INVIS | A_DIM);
    m_render.at(x, y).attr |= A_BOLD;
}

void Scenario::source_set_detected(int x, int y)
{
    m_source.at(x, y).attr &= ~A_INVIS;
    m_source.at(x, y).attr |= A_DIM;
}

void Scenario::render_los(const Object &viewer)
{
#define LIGHT(x, y)                \
    if (!abroad(x, y))             \
     {                             \
        source_set_detected(x, y); \
        render_set_visible(x, y);  \
        if (!viewer.visible(m_source.at(x, y).c)) goto next_line; \
     }

    using std::hypot;    

    int vision_range = viewer.get_vision_range();


    int px = viewer.getx();
    int py = viewer.gety();

    for (int y = py - vision_range; y < py + vision_range; ++y)
        for (int x = px - vision_range; x < px + vision_range; ++x)
        {
            if (round(hypot(x - px, y - py)) >= vision_range) continue;

            int px = viewer.getx();
            int py = viewer.gety();

            int delta_x(x - px);

            signed char const ix((delta_x > 0) - (delta_x < 0));
            delta_x = std::abs(delta_x) << 1;

            int delta_y(y - py);

            signed char const iy((delta_y > 0) - (delta_y < 0));
            delta_y = std::abs(delta_y) << 1;

            LIGHT(px, py)

            if (delta_x >= delta_y)
            {
                int error(delta_y - (delta_x >> 1));

                while (px != x)
                {
                    if ((error > 0) || (!error && (ix > 0)))
                    {
                        error -= delta_x;
                        py += iy;
                    }

                    error += delta_y;
                    px += ix;

                    LIGHT(px, py)
                }
            }
            else
            {
                int error(delta_x - (delta_y >> 1));

                while (py != y)
                {
                    if ((error > 0) || (!error && (iy > 0)))
                    {
                        error -= delta_y;
                        px += ix;
                    }

                    error += delta_x;
                    py += iy;

                    LIGHT(px, py)
                }
            }
            next_line: ;
    }
}


void Scenario::parse_yaml() {

    FILE *file = nullptr;

    yaml_parser_t parser;
    yaml_document_t document;
    yaml_node_t *node;

    std::ostringstream error;

    file = fopen(m_file.c_str(), "r");

    if(!yaml_parser_initialize(&parser))
        throw game_error("Failed to initialize parser!\n");
    else if(!file)
        throw game_error("Failed to open file!\n");

    yaml_parser_set_input_file(&parser, file);

    if (!yaml_parser_load(&parser, &document))
    {
        error << "YAML: ";

        switch (parser.error)
        {
        case YAML_MEMORY_ERROR:
            error << "Memory error: Not enough memory for parsing.\n";
            break;

        case YAML_READER_ERROR:
            if (parser.problem_value != -1) {
                error << "Reader error: "
                      << parser.problem        << ": #"
                      << parser.problem_value  << " at "
                      << parser.problem_offset <<".\n";
            }
            else {
                error << "Reader error: "
                      << parser.problem << " at "
                      << parser.problem_offset << ".\n";
            }
            break;

        case YAML_SCANNER_ERROR:
            error << "Scanner error: ";
            break;
        case YAML_PARSER_ERROR:
            error << "Parser error: ";
            break;
        case YAML_COMPOSER_ERROR:
            error << "Composer error: ";
            break;
        default:
            error << "Internal error.\n";
            break;
        }

        if (parser.error == YAML_SCANNER_ERROR ||
                parser.error == YAML_PARSER_ERROR  ||
                parser.error == YAML_COMPOSER_ERROR )
        {
            if (parser.context)
                error << parser.context << " at line "
                      << parser.context_mark.line+1
                      << ", column " << parser.context_mark.column+1 << "\n"
                      << parser.problem << " at line "
                      << parser.problem_mark.line+1 << ", column "
                      << parser.problem_mark.column+1 << ".\n";

            else
                error << parser.problem <<" at line "
                      << parser.problem_mark.line+1
                      << " , column " << parser.problem_mark.column+1
                      << ".\n";
        }

        yaml_document_delete(&document);
        yaml_parser_delete(&parser);
        fclose(file);

        throw game_error(error.str());

    }

    yaml_parser_delete(&parser);
    fclose(file);

    /* Read YAML config */
    node = yaml_document_get_root_node(&document);

    try {
        if (not (node and node->type == YAML_MAPPING_NODE)) throw game_error(parse_error);

        for (auto pair = node->data.mapping.pairs.start;
             pair < node->data.mapping.pairs.top; ++pair)
        {
            auto section = yaml_document_get_node(&document, pair->key);

            if (section->type != YAML_SCALAR_NODE) throw game_error(parse_error);

            const char *key = reinterpret_cast<const char *>(section->data.scalar.value);

            if (!strcmp(key, "objects"))
               parse_yaml_objects( yaml_document_get_node(&document, pair->value), &document);

            else if (!strcmp(key, "maps"))
               parse_yaml_maps(yaml_document_get_node(&document, pair->value), &document);

            else
                throw game_error( String("Found unknown structure \"") + key + "\".");
        }

        yaml_document_delete(&document);
        return;

    } catch (const game_error &er) {
        yaml_document_delete(&document);
        throw er;
    }
}

void Scenario::parse_yaml_objects(const yaml_node_t *node, yaml_document_t *doc)
{
    if (node->type != YAML_MAPPING_NODE) throw game_error(parse_error);

    for (auto pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; ++pair)
    {
        auto node_key = yaml_document_get_node(doc, pair->key);
        auto node_value = yaml_document_get_node(doc, pair->value);

        if (node_key->type != YAML_SCALAR_NODE)
            throw game_error(parse_error);

        const char *key = reinterpret_cast<const char *>(node_key->data.scalar.value);

        Object* new_object = Object::create_from_yaml(node_value, doc);
        m_objects.emplace_back( new_object );

        if (!strcmp(key, "player"))
            m_player = m_objects.back();
    }
}

void Scenario::parse_yaml_maps(const yaml_node_t *node, yaml_document_t *doc)
{
    if (node->type != YAML_MAPPING_NODE) throw game_error(parse_error);

    for (auto pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; ++pair)
    {
        auto node_key = yaml_document_get_node(doc, pair->key);
        auto node_value = yaml_document_get_node(doc, pair->value);

        if (node_key->type != YAML_SCALAR_NODE)
            throw game_error(parse_error);

       // const char *key = reinterpret_cast<const char *>(node_key->data.scalar.value);

        m_source = Map::create_from_yaml(node_value, doc);
    }
}
