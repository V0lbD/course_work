#ifndef DB_VALUE_BUILDER_H
#define DB_VALUE_BUILDER_H

#include "db_value.h"

class db_value_builder {
    std::string _description;
    std::string _surname, _name, _patronymic;
    std::string _email;
    std::string _phone_number;
    std::string _address;
    std::string _user_comment;
    std::string _date_time;

public:
    db_value_builder * with_description(std::string && description);
    db_value_builder * with_surname(std::string && surname);
    db_value_builder * with_name(std::string && name);
    db_value_builder * with_patronymic(std::string && patronymic);
    db_value_builder * with_email(std::string && email);
    db_value_builder * with_phone_number(std::string && phone_number);
    db_value_builder * with_address(std::string && address);
    db_value_builder * with_user_comment(std::string && user_comment);
    db_value_builder * with_date_time(std::string && date_time);

    [[nodiscard]] db_value *build() const;

private:
    static void check_if_string_of_value_is_empty(std::string const& s)
    {
        if (s.empty()) {
            throw db_value::create_exception("db_user_communication:: incorrect input while building a value");
        }
    }

public:
    db_value *build_from_stream(std::ifstream *input_stream, bool is_cin);

};

#endif //DB_VALUE_BUILDER_H
