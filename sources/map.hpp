#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <string>

using std::vector;
using std::string;

#include "utils.hpp"
#include "base.hpp"

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

class map  : public base {
    int m_x, m_y;

    int m_width, m_height;
    vector<text> m_lines;

public:

  map(const string &id, const string &map, int w, int h);

  static map create_from_yaml(const string &id, const yaml_node_t *node, yaml_document_t *doc);

  static void generate(const string &f, int w, int h);
  static string generate(int w, int h);

  inline cchar& at(int x, int y)
  { return m_lines.at( vector<text>::size_type(y) ).cstr[x]; }

  void push(const string &s);
  void decorate();

  const vector<text>& get_map() const
  { return m_lines; }

  vector<text>& get_map()
  { return m_lines; }

  int width() const
  { return m_width; }

  int height() const
  { return m_height; }

  void setx(int x)
  { m_x = x; }

  void sety(int y)
  { m_y = y; }

  int x() const
  { return m_x; }

  int y() const
  { return m_y; }

};

#endif // MAP_HPP
