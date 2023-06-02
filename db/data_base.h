#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <iostream>
#include <tuple>
#include <vector>

#include "../allocator/memory_holder.h"
#include "../allocator_from_global_heap/memory_from_global_heap.h"

#include "../logger/logger_holder.h"

#include "../b_tree/associative_container.h"
#include "../b_tree/b_tree.h"

#include "../chain_of_resp_and_command/handler.h"
#include "../db_key/key.h"

class data_base final :
        private memory_holder,
        private logger_holder
{
private:
    class string_comparer
    {
    public:
        int operator()(std::string * x, std::string * y) {
            return (*x).compare((*y));
        }
    };

private:
    /* дерево пулов
     *     дерево схем
     *          дерево коллекций
     *               коллекция
     * */
    b_tree<std::string *,
        associative_container<std::string *,
            associative_container<std::string *,
                associative_container<key, db_value *> *
                                 > *
                             > *, string_comparer
                         > * _database;
    logger * _logger;
    memory* _allocator;
    std::map<std::string, memory *> _all_trees_allocators;

public:
    typedef enum allocator_types {
        global,
        for_inner_use_sorted_list,
        sorted_list_best,
        sorted_list_worst,
        sorted_list_first,
        for_inner_use_descriptors,
        descriptors_best,
        descriptors_worst,
        descriptors_first,
        buddy_system,
        not_an_allocator
    } allocator_types_;

#pragma region exceptions
public:
    class db_insert_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_insert_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class db_find_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_find_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class db_remove_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit db_remove_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };
#pragma endregion

#pragma region Find structure
private:
    [[nodiscard]] associative_container<std::string *, associative_container<std::string *, associative_container<key, db_value *> *> *> *
    find_data_pool
    (std::string const & pool_name) const;

    [[nodiscard]] associative_container<std::string *, associative_container<key, db_value *> *> *
    find_data_scheme
    (std::string const & pool_name, std::string const & scheme_name) const;

    [[nodiscard]] associative_container<key, db_value *> *
    find_data_collection
    (std::string const & pool_name, std::string const & scheme_name, std::string const & collection_name) const;
#pragma endregion

#pragma region Collection-related functions
#pragma region Insertion in collection
public:
    void add_to_collection
    (std::string const & pool_name, std::string const & scheme_name, std::string const & collection_name,
     const key& _key, db_value * value) const;

#pragma endregion

#pragma region Updating a collection value
public:
    void update_in_collection
    (std::string const & pool_name, std::string const & scheme_name, std::string const & collection_name,
     const key& _key, std::map<db_value_fields, unsigned char *> upd_dict) const;
#pragma endregion

#pragma region Finding among collection
public:
    [[nodiscard]] db_value * find_among_collection
    (std::string const & pool_name, std::string const & scheme_name, std::string const & collection_name,
     const key& _key) const;

    [[nodiscard]] db_value * find_with_time
    (std::string const & pool_name, std::string const & scheme_name, std::string const & collection_name,
     db_value * found_value, const key& _key, uint64_t time_parameter) const;

    [[nodiscard]] std::vector<db_value *> find_in_range
    (std::string const & pool_name, std::string const & scheme_name, std::string const & collection_name,
     key min_key, key max_key) const;
#pragma endregion

#pragma region Deletion from collection
public:
    void delete_from_collection
    (std::string const & pool_name, std::string const & scheme_name, std::string const & collection_name,
     const key& key) const;
#pragma endregion
#pragma endregion

#pragma region Structure functions
#pragma region Inserting in structure of data base
    // adding a collection: no string is empty
    // adding a scheme: collection string is empty
private:
    memory *
    get_new_allocator_for_inner_trees
    (std::string const & pool_name, std::string const & scheme_name, std::string const & collection_name,
     allocator_types_ allocator_type, size_t allocator_pool_size);

public:
    void add_to_structure
    (std::string const & pool_name, std::string const & scheme_name, std::string const & collection_name,
     unsigned tree_parameter, allocator_types_ allocator_type, size_t allocator_pool_size);
#pragma endregion

#pragma region Deletion from structure of data base
private:
    void delete_collection
            (const std::string & collection_name, associative_container<std::string *,
                    associative_container<key, db_value *> *> * parent_scheme);

    void delete_scheme
            (const std::string & scheme_name, associative_container<std::string *,
                    associative_container<std::string *, associative_container<key, db_value *> *> *> * parent_pool);

    void delete_pool(const std::string & pool_name);

public:
    void delete_from_structure
    (std::string const & pool_name, std::string const & scheme_name, std::string const & collection_name);
#pragma endregion
#pragma endregion

#pragma region logger_holder and memory_holder contract
    [[nodiscard]] logger *get_logger() const noexcept override
    {
        return this->_logger;
    }

    [[nodiscard]] memory *get_memory() const noexcept override
    {
        return this->_allocator;
    }
#pragma endregion

#pragma region rool 5
public:
    explicit data_base(unsigned order_of_tree, logger * this_db_logger = nullptr, memory * this_db_allocator = nullptr)
    : _logger(this_db_logger), _allocator(this_db_allocator)
    {
        _database = new b_tree<std::string *,
                                    associative_container<std::string *,
                                        associative_container<std::string *,
                                            associative_container<key, db_value *> *> *> *
                                                                     , string_comparer>(order_of_tree, this_db_logger, this_db_allocator);
    }

    ~data_base() override;

    // copy constructor
    data_base(data_base const &obj) = delete;

    // move constructor
    data_base(data_base &&obj) noexcept = delete;

    // copy assignment (оператор присваивания)
    data_base &operator=(data_base const &obj) = delete;

    // move assignment (оператор присваивания перемещением)
    data_base &operator=(data_base &&obj) noexcept = delete;

#pragma endregion

    std::vector<db_value *>
    find_dataset_with_time(const std::string &pool_name, const std::string &scheme_name,
                           const std::string &collection_name,
                           key min_key, key max_key, uint64_t time_parameter);
};

#endif //DATA_BASE_H
