#ifndef BASE_HPP
#define BASE_HPP

#include <string>

using std::string;

class Base {
    string m_id;
public:
    Base(const string &id) : m_id(id) {}

    const string& get_id() const
    { return m_id; }
};

#endif // BASE_HPP
