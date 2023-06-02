#ifndef DB_USER_COMMUNICATION_H
#define DB_USER_COMMUNICATION_H

#include "db_key/key.h"
#include "db_value/db_value_builder.h"
#include "db/data_base.h"

#define DEFAULT_B_TREE_PARAMETER 10

void help();

#pragma region Parsing
typedef enum commands {
    _add_,
    _find_,
    _update_,
    _delete_,
    _help_,
    _exit_,
    _from_,
    _not_a_command_
} commands_;

class parse_exception final : public std::exception {
private:
    std::string _message;

public:
    explicit parse_exception(std::string message)
            : _message(std::move(message)) {

    }

    [[nodiscard]] char const *what() const noexcept override {
        return _message.c_str();
    }
};

commands_
get_command(std::string const &user_input);

std::pair<commands_, std::string>
parse_user_input(std::string const &user_input);

std::tuple<std::string, std::string, std::string>
parse_path(std::string &input_string);

std::tuple<std::string, std::string, std::string>
get_path_from_user_input(std::ifstream *input_stream, bool is_cin, bool is_path);

#pragma endregion

#pragma region Add command

void
do_add_command(data_base *db, std::string &input_str_leftover, std::ifstream * input_stream, bool is_cin);

#pragma endregion

#pragma region Find command

typedef struct time_str
{
    short YY, MM, DD;
    short hh, mm, ss;
} struct_time;

std::tuple<db_value *, std::vector<db_value *>, std::vector<db_value *>, db_value *>
do_find_command
(data_base * db, std::string &input_str_leftover, std::ifstream * input_stream, bool is_cin);

#pragma endregion

#pragma region Update command

void do_update_command(data_base * db, std::ifstream *input_stream, bool is_cin);

#pragma endregion

#pragma region Delete command

void delete_db(data_base *db);

void do_delete_command(data_base * db, std::string & path_inner, std::ifstream *input_stream, bool is_cin);

#pragma endregion

#endif //DB_USER_COMMUNICATION_H
