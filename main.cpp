#include "db_user_communication.h"

void do_from_file_command(data_base * db, std::string const & filename);

void db_test(data_base * db, std::ifstream *input_stream, bool is_cin) {
    if (is_cin) {
        help();
    }
    bool not_exited = true;
    std::string user_input;

    while (not_exited) {
        if (is_cin) {
            std::cout << ">>";
            std::getline(std::cin, user_input);
        } else {
            std::getline((*input_stream), user_input);
        }

        auto command_and_leftover_string = parse_user_input(user_input);
        commands_ command = command_and_leftover_string.first;
        std::string leftover = command_and_leftover_string.second;

        switch (command) {
            case commands_::_add_:
                try {
                    do_add_command(db, leftover, input_stream, is_cin);
                }
                catch (parse_exception const & except) {
                    if (is_cin) {
                        std::cout << except.what() << std::endl;
                    }
                }
                catch (db_value::create_exception const & except) {
                    if (is_cin) {
                        std::cout << except.what() << std::endl;
                    }
                }
                catch (key::create_exception const & except) {
                    if (is_cin) {
                        std::cout << except.what() << std::endl;
                    }
                }
                catch (handler::order_exception const & except) {
                    if (is_cin) {
                        std::cout << except.what() << std::endl;
                    }
                }
                catch (data_base::db_insert_exception const & except) {
                    if (is_cin) {
                        std::cout << except.what() << std::endl;
                    }
                }
                catch (data_base::db_find_exception const & except) {
                    if (is_cin) {
                        std::cout << except.what() << std::endl;
                    }
                }
                break;
            case commands_::_find_:
                try {
                    std::tuple<db_value *, std::vector<db_value *>, std::vector<db_value *>, db_value *> found
                            = do_find_command(db, leftover, input_stream, is_cin);

                    db_value * found_with_time = std::get<0>(found);
                    std::vector<db_value *> db_value_vector_with_time = std::get<1>(found);
                    std::vector<db_value *> db_value_vector_simply = std::get<2>(found);
                    db_value * simpy_found = std::get<3>(found);

                    if (found_with_time != nullptr) {
                        if (is_cin) {
                            std::cout << (*found_with_time) << std::endl;
                        }
                        delete found_with_time;
                    }
                    else if (simpy_found != nullptr) {
                        if (is_cin) {
                            std::cout << (*simpy_found) << std::endl;
                        }
                    }
                    else if (!(db_value_vector_simply.empty())) {
                        if (is_cin) {
                            unsigned i, size_of_vector = db_value_vector_simply.size();
                            for (i = 0; i < size_of_vector; i++) {
                                std::cout << "----- " << i + 1 << " value " << "-----" << std::endl;
                                std::cout << (*(db_value_vector_simply[i])) << std::endl;
                            }
                        }
                    } else if (!(db_value_vector_with_time.empty())) {
                        if (is_cin) {
                            unsigned i, size_of_vector = db_value_vector_with_time.size();
                            for (i = 0; i < size_of_vector; i++) {
                                std::cout << "----- " << i + 1 << " value " << "-----" << std::endl;
                                if ((db_value_vector_with_time[i]) == nullptr) {
                                    std::cout << "value does not exist on that time" << std::endl;
                                } else {
                                    std::cout << (*(db_value_vector_with_time[i])) << std::endl;
                                    delete db_value_vector_with_time[i];
                                }
                            }
                        }
                    }
                    else {
                        if (is_cin) {
                            std::cout << "No values were found" << std::endl;
                        }
                    }
                }
                catch (parse_exception const & exception) {
                    if (is_cin) {
                        std::cout << exception.what() << std::endl;
                    }
                }
                catch (key::create_exception const & exception)
                {
                    if (is_cin) {
                        std::cout << exception.what() << std::endl;
                    }
                }
                catch (data_base::db_find_exception const & exception) {
                    if (is_cin) {
                        std::cout << exception.what() << std::endl;
                    }
                }
                break;
            case commands_::_update_:
                try {
                    do_update_command(db, input_stream, is_cin);
                }
                catch (key::create_exception const & exception) {
                    if (is_cin) {
                        std::cout << exception.what() << std::endl;
                    }
                }
                catch (parse_exception const & exception) {
                    if (is_cin) {
                        std::cout << exception.what() << std::endl;
                    }
                }
                catch (handler::order_exception const & exception) {
                    if (is_cin) {
                        std::cout << exception.what() << std::endl;
                    }
                }
                catch (data_base::db_find_exception const & exception) {
                    if (is_cin) {
                        std::cout << exception.what() << std::endl;
                    }
                }
                break;
            case commands_::_delete_:
                try {
                    do_delete_command(db, leftover, input_stream, is_cin);
                }
                catch (parse_exception const & exception) {
                    if (is_cin) {
                        std::cout << exception.what() << std::endl;
                    }
                }
                catch (data_base::db_find_exception const & exception) {
                    if (is_cin) {
                        std::cout << exception.what() << std::endl;
                    }
                }
                catch (handler::order_exception const & exception) {
                    if (is_cin) {
                        std::cout << exception.what() << std::endl;
                    }
                }
                catch (data_base::db_remove_exception const & exception) {
                    if (is_cin) {
                        std::cout << exception.what() << std::endl;
                    }
                }
                break;
            case commands_::_help_:
                help();
                break;
            case commands_::_exit_:
                if (is_cin) {
                    delete_db(db);
                    std::cout << "Exited successfully!" << std::endl;
                }
                not_exited = false;
                break;
            case commands_::_from_:
                do_from_file_command(db, leftover);
                break;
            default:
                if (is_cin) {
                    std::cout << "Wrong command passed, try again!" << std::endl;
                }
                break;
        }
    }
}

void do_from_file_command(data_base * db, std::string const & filename)
{
    auto * from_file = new std::ifstream(filename);
    if (!(*from_file)) {
        std::cout << "Could not open a file " << filename << std::endl;
    } else {
        db_test(db, from_file, false);
        from_file->close();
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "You should point a path to file with execution commands as a command line argument for proper work of program" << std::endl;
        return 0;
    }

    auto * file = new std::ifstream(argv[1]);
    if ( !(*file) ) {
        std::cout << "Could not open a file " << argv[1] << std::endl;
        return 0;
    }

    logger_builder * loggerBuilder = new logger_builder_impl();
    logger * logg = loggerBuilder->with_stream("trace.txt", logger::severity::trace)
                                 ->with_stream("warning.txt", logger::severity::warning)
                                 ->build();
    delete loggerBuilder;

    data_base db(10, logg);

    db_test(&db, file, false);
    file->close();

    std::cout << "Hello! Would you like to get some closer interaction with my program? Print y for yes an n for no\n>>";
    std::string answer_to_important_question;
    std::getline(std::cin, answer_to_important_question);

    if (answer_to_important_question == "y") {
        db_test(&db, nullptr, true);
    } else {
        delete_db(&db);
    }

    return 0;
}
