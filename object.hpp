#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "utils.hpp"

class Object {
private:
    int m_x = 0, m_y = 0;    
    Text::cchar m_c;

    String pathless;
public:
    Object() : m_x(0), m_y(0), m_c(), pathless(nullptr) {}
    Object(int x, int y, const Text::cchar& c) : m_x(x), m_y(y), m_c(c),

        pathless("#~") {}

    void move(int x, int y) { m_x += x; m_y += y; }

    const String&      getpathless() const { return pathless; }
    int                getx() const        { return m_x; }
    int                gety() const        { return m_y; }
    const Text::cchar& getcchar() const    { return m_c; }
};

#endif // OBJECT_HPP
