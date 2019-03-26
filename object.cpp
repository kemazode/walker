#include "object.hpp"

bool Object::move(int x, int y, char path)
{
    if (m_impass.find(path) == m_impass.npos) {
        m_x += x; m_y += y;
        return true;
    }
    return false;
}
