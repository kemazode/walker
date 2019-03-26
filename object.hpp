#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "utils.hpp"

class Object
{
protected:
    int m_x, m_y;
    cchar m_c;
    String m_impass;

public:
    Object(const cchar &c, const String &imp) : m_x(0), m_y(0), m_c(c), m_impass(imp) {}

    Object(int x, int y, const cchar &c, const String &imp) :
        m_x(x), m_y(y), m_c(c), m_impass(imp) {}

    virtual ~Object() {}

    // definition in .cpp
    virtual bool move(int x, int y, char path);

    int          getx() const     { return m_x; }
    int          gety() const     { return m_y; }
    const cchar& getcchar() const { return m_c; }
};

class Dwarf : public Object {
public:
    Dwarf() : Object(cchar('@', A_BOLD), "~#") {}
    Dwarf(int x, int y) : Object(x, y, cchar('@', A_BOLD), "~#") {}

    bool move(int x, int y, char path)
    { return Object::move(x, y, path); }
};

#endif // OBJECT_HPP
