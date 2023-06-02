#ifndef DB_VALUE_H
#define DB_VALUE_H

#include "string_holder.h"

typedef enum fields {
    _description_,
    _surname_,
    _name_,
    _patronymic_,
    _email_,
    _phone_number_,
    _address_,
    _user_comment_,
    _date_time_
} db_value_fields;

class handler;

class db_value {
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

private:
    std::string * _description;
    std::string * _surname, * _name, * _patronymic;
    std::string * _email;
    std::string * _phone_number;
    std::string * _address;
    std::string * _user_comment;
    std::string * _date_time;

    uint64_t _timestamp;

    handler * _chain_of_resp;
    handler * _last_handler;

    friend class db_value_builder;
    friend class update_command;
    friend class add_command;
    friend class add_handler;

public:
    handler * get_first_handler();
    handler * get_last_handler();
    void add_new_handler(handler * handler_);

public:
    [[nodiscard]] uint64_t get_timestamp() const
    {
        return _timestamp;
    }

    friend std::ostream &operator<<(std::ostream &out, const db_value &value);

    db_value * make_a_copy()
    {
        auto * copy = new db_value(
                (*(this->_description)),
                (*(this->_surname)), (*(this->_name)), (*(this->_patronymic)),
                (*(this->_email)), (*(this->_phone_number)), *this->_address,
                (*(this->_user_comment)), *this->_date_time);
        copy->_timestamp = this->_timestamp;
        copy->_chain_of_resp = nullptr;
        copy->_last_handler = nullptr;
        return copy;
    }

private:
    static std::string * get_ptr_from_string_holder(std::string const & s)
    {
        return string_holder::get_instance()->get_string(s);
    }

    static void remove_string_from_string_holder(std::string const & s)
    {
        string_holder::get_instance()->remove_string(s);
    }

    db_value(std::string const & description,
             std::string const & surname, std::string const & name, std::string const & patronymic,
             std::string const & email, std::string const & phone_number, std::string const & address,
             std::string const & user_comment, std::string const & date_time)
    {
        _description = get_ptr_from_string_holder(description);
        _surname = get_ptr_from_string_holder(surname);
        _name = get_ptr_from_string_holder(name);
        _patronymic = get_ptr_from_string_holder(patronymic);
        _email = get_ptr_from_string_holder(email);
        _phone_number = get_ptr_from_string_holder(phone_number);
        _address = get_ptr_from_string_holder(address);
        _user_comment = get_ptr_from_string_holder(user_comment);
        _date_time = get_ptr_from_string_holder(date_time);

        _timestamp = std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::system_clock::now().time_since_epoch()).count();
        _chain_of_resp = nullptr;
        _last_handler = nullptr;
    }

public:
    ~db_value();
};

inline std::ostream &operator<<(std::ostream &out, const db_value &value) {
    out << (*(value._description)) << std::endl;
    out << (*(value._surname)) << " " << (*(value._name)) << " " << (*(value._patronymic)) << std::endl;
    out << (*(value._email)) << std::endl;
    out << (*(value._phone_number)) << std::endl;
    out << (*(value._address)) << std::endl;
    out << (*(value._user_comment)) << std::endl;
    out << (*(value._date_time)) << std::endl;
    return out;
}

#endif //DB_VALUE_H
