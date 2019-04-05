#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <string>

using std::vector;
using std::string;

#include "utils.hpp"

typedef struct yaml_node_s yaml_node_t;
typedef struct yaml_document_s yaml_document_t;

class Map {

    int m_x, m_y;

    int m_width, m_height;
    vector<Text> m_strs;

public:

  Map() : m_x(0), m_y(0), m_width(0), m_height(0), m_strs() {}
  Map(const string &map, int w, int h);

  static Map create_from_yaml(const yaml_node_t *node, yaml_document_t *doc);

  static void generate(const string &f, int w, int h);
  static string generate(int w, int h);

  inline cchar& at(int x, int y)
  { return m_strs.at( vector<Text>::size_type(y) ).at( size_t(x) ); }

  void push(const string &s);
  void decorate();

  const vector<Text>& getstrs() const
  { return m_strs; }

  vector<Text>& getstrs()
  { return m_strs; }

  int width() const
  { return m_width; }

  int height() const
  { return m_height; }

  void setx(int x)
  { m_x = x; }

  void sety(int y)
  { m_y = y; }

  int getx() const
  { return m_x; }

  int gety() const
  { return m_y; }

};

#endif // MAP_HPP
