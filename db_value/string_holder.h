#ifndef STRING_HOLDER_H
#define STRING_HOLDER_H

#include <iostream>
#include <chrono>
#include <map>

class string_holder
{
private:
    static string_holder *_instance;
public:
    static string_holder *get_instance()
    {
        if (_instance == nullptr)
        {
            _instance = new string_holder;
        }

        return _instance;
    }

private:

    std::map<std::string, std::pair<std::string *, unsigned *>> _pool;

private:

    string_holder()
    {

    }

public:

    string_holder(string_holder const &) = delete;

    string_holder(string_holder &&) = delete;

    string_holder &operator=(string_holder const &) = delete;

    string_holder &operator=(string_holder &&) = delete;

    ~string_holder() noexcept = default;

public:

    std::string *get_string(std::string const &key)
    {
        std::string * to_return = nullptr;
        if (_pool.count(key)) { // if (_pool.contains(key)) {
            std::pair<std::string *, unsigned *> ptr_n_usages = _pool.operator[](key);
            to_return = ptr_n_usages.first;
            (*(ptr_n_usages.second))++;
        } else {
            std::pair<std::string *, unsigned *> alloc(new std::string(key), new unsigned(1));
            to_return = alloc.first;
            _pool.insert(std::make_pair(key, alloc));
        }

        return to_return;
    }

    void remove_string(std::string const &key)
    {
        if (_pool.count(key)) { // if (_pool.contains(key)) {
            std::pair<std::string *, unsigned *> ptr_n_usages = _pool.operator[](key);
            if ((*(ptr_n_usages.second)) == 1) {
                _pool.erase(key);
            } else {
                (*(ptr_n_usages.second))--;
            }
        }
    }

};

inline string_holder *string_holder::_instance = nullptr;

#endif //STRING_HOLDER_H
