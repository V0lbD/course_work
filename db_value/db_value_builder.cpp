#include "db_value_builder.h"
#include <fstream>

db_value_builder *db_value_builder::with_description(std::string &&description) {
    _description = std::move(description);
    return this;
}

db_value_builder *db_value_builder::with_surname(std::string &&surname) {
    _surname = std::move(surname);
    return this;
}

db_value_builder *db_value_builder::with_name(std::string &&name) {
    _name = std::move(name);
    return this;
}

db_value_builder *db_value_builder::with_patronymic(std::string &&patronymic) {
    _patronymic = std::move(patronymic);
    return this;
}

db_value_builder *db_value_builder::with_email(std::string &&email) {
    _email = std::move(email);
    return this;
}

db_value_builder *db_value_builder::with_phone_number(std::string &&phone_number) {
    _phone_number = std::move(phone_number);
    return this;
}

db_value_builder *db_value_builder::with_address(std::string && address)
{
    _address = std::move(address);
    return this;
}

db_value_builder *db_value_builder::with_user_comment(std::string &&user_comment) {
    _user_comment = std::move(user_comment);
    return this;
}

db_value_builder *db_value_builder::with_date_time(std::string &&date_time) {
    _date_time = std::move(date_time);
    return this;
}

db_value* db_value_builder::build() const {
    return new db_value(_description,
                        _surname, _name, _patronymic,
                        _email, _phone_number, _address,
                        _user_comment, _date_time);
}

db_value* db_value_builder::build_from_stream(std::ifstream *input_stream, bool is_cin) {
    std::string token, delimiter = " ";
    size_t pos;
    unsigned delimiter_length = delimiter.length();

    if (is_cin) {
        std::cout << "Enter value:" << std::endl;
        std::cout << "Delivery description: >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    this->with_description(std::move(token));

    if (is_cin) {
        std::cout << "Surname Name Patronymic: >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }

    if ((pos = token.find(delimiter)) != std::string::npos) {
        this->with_surname(std::move(token.substr(0, pos)));
        token.erase(0, pos + delimiter_length);
    } else {
        throw db_value::create_exception("db_value_builder::build_from_stream:: incorrect input while building a value");
    }

    if ((pos = token.find(delimiter)) != std::string::npos) {
        this->with_name(std::move(token.substr(0, pos)));
        token.erase(0, pos + delimiter_length);
    } else {
        throw db_value::create_exception("db_value_builder::build_from_stream:: incorrect input while building a value");
    }

    if (token.empty()) {
        throw db_value::create_exception("db_value_builder::build_from_stream:: incorrect input while building a value");
    }
    this->with_patronymic(std::move(token));

    if (is_cin) {
        std::cout << "Email: >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);
    this->with_email(std::move(token));

    if (is_cin) {
        std::cout << "Phone number: >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);
    this->with_phone_number(std::move(token));

    if (is_cin) {
        std::cout << "Address: >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);
    this->with_address(std::move(token));

    if (is_cin) {
        std::cout << "User comment: >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);
    this->with_user_comment(std::move(token));

    if (is_cin) {
        std::cout << "Date time: >>";
        std::getline(std::cin, token);
    } else {
        std::getline((*input_stream), token);
    }
    check_if_string_of_value_is_empty(token);
    this->with_date_time(std::move(token));

    return (this->build());
}
