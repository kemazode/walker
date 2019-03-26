#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <vector>
#include "utils.hpp"
#include "object.hpp"

class Scenario {
private:
    String m_file;

    int m_x, m_y, m_lines, m_cols;
    size_t m_width, m_height;

    vector<Text> m_map_source, m_map_render;

    std::list<Object> m_objects;
    std::list<Object>::pointer m_player;

    static const String m_gfile;

    inline bool physic_movement_allowed(int x, int y, const Object& obj);
    void physic_light_render(const Object& viewer);

    inline cchar& map_source(int x, int y)
    { return m_map_source.at(size_t(y))[size_t(x)]; }

    inline cchar& map_render(int x, int y)
    { return m_map_render.at(size_t(y))[size_t(x)]; }

public:    

    bool load(const String &f, Text &err);
    void clear();

    bool gen(const String &f, size_t size, Text &err);
    bool gen(size_t size, Text &err, String& f);

    size_t size() const { return m_map_source.size(); }
    int getx() const { return m_x;  }
    int gety() const { return m_y;  }    

    void move_view(int x, int y) { set_view(m_x + x, m_y + y); }
    void move_player(int x, int y);
    void set_view(int x, int y);
    void set_display(int l, int c) { m_lines = l; m_cols = c; }

    void update_render_map();
    const vector<Text>& get_render_map() const { return m_map_render; }
};

#endif // SCENE_HPP
