#ifndef KEY_H
#define KEY_H

#include <iostream>
#include <fstream>

class key
{
    int _user_id;
    int _delivery_id;

    friend class key_comparer;
    friend std::ostream &operator<<(std::ostream &out, const key &_key_);

public:
    class create_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit create_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

public:
    key(std::ifstream* input_stream, bool is_cin)
    {
        std::string token, delimiter = " ";
        std::string user_id, delivery_id;
        size_t pos;
        unsigned delimiter_length = delimiter.length();

        if (is_cin) {
            std::cout << "Enter key:\nuser id delivery id: >>";
            std::getline(std::cin, token);
        } else {
            std::getline((*input_stream), token);
        }

        if ((pos = token.find(delimiter)) != std::string::npos) {
            user_id = token.substr(0, pos);
            token.erase(0, pos + delimiter_length);
        } else {
            throw key::create_exception("key constructor: incorrect input for key");
        }

        if (token.empty()) {
            throw key::create_exception("key constructor: incorrect input for key");
        }

        delivery_id = token;

        try {
            this->_user_id = stoi(user_id);
            this->_delivery_id = stoi(delivery_id);
        }
        catch (std::invalid_argument const &) {
            throw key::create_exception("key constructor: incorrect input for key(s)");
        }
    }

    key() = default;

    key(int user_id, int delivery_id)
    : _delivery_id(delivery_id), _user_id(user_id)
    {

    }

    // copy constructor
    key(key const &obj)
    : key(obj._user_id, obj._delivery_id)
    {

    }

    // move constructor
    key(key &&obj) noexcept
    : key(obj._user_id, obj._delivery_id)
    {
        obj._user_id = 0;
        obj._delivery_id = 0;
    }

    // copy assignment (оператор присваивания)
    key &operator=(key const &obj)
    {
        if (this == &obj)
        {
            return *this;
        }

        _user_id = obj._user_id;
        _delivery_id = obj._delivery_id;

        return *this;
    }

    // move assignment (оператор присваивания перемещением)
    key &operator=(key &&obj) noexcept
    {
        if (this == &obj)
        {
            return *this;
        }

        _user_id = obj._user_id;
        obj._user_id = 0;

        _delivery_id = obj._delivery_id;
        obj._delivery_id = 0;

        return *this;
    }

    ~key() = default;
};

inline std::ostream &operator<<(std::ostream &out, const key &_key_)
{
    out << _key_._user_id << " " << _key_._delivery_id << std::endl;
    return out;
}

class key_comparer
{
public:
    int operator()(key x, key y) {
        int user_id_comparison = x._user_id - y._user_id;
        if (user_id_comparison == 0) {
            return x._delivery_id - y._delivery_id;
        } else {
            return user_id_comparison;
        }
    }
};

#endif //KEY_H
