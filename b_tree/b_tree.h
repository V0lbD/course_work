#ifndef B_TREE_H
#define B_TREE_H

#include <stack>

#include "associative_container.h"
#include "../logger/logger.h"
#include "../logger/logger_holder.h"
#include "../allocator/memory_base_class.h"
#include "../allocator/memory_holder.h"

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class b_tree
        : public associative_container<tkey, tvalue>,
          protected logger_holder,
          protected memory_holder
{
friend struct b_node;
protected:
    unsigned _order_of_tree;
    memory * _allocator;
    logger * _logger;

public:
    struct b_node
    {
        unsigned _key_count;
        tkey * _key_array;
        tvalue * _value_array;
        bool _is_leaf;
        b_node ** _sub_trees;
        b_tree<tkey, tvalue, tkey_comparer> * _target_tree;

#pragma region insertion
        unsigned find_key(tkey const & key)
        {
            unsigned to_return = 0;
            tkey_comparer comparer;
            while (to_return < _key_count && comparer(_key_array[to_return], key) < 0) {
                ++to_return;
            }
            return to_return;
        }

        void split_child(unsigned index, tkey const &key, b_node * child)
        {
            _target_tree->debug_with_guard("b_node::split_child method started");
            tkey_comparer comparer;
            if (comparer(child->_key_array[_target_tree->_order_of_tree - 1], key) == 0) {
                _target_tree->warning_with_guard("b_node::split_child equal keys detected")
                        ->trace_with_guard("b_node::split_child method finished");
                throw b_tree<tkey, tvalue, tkey_comparer>::insert_exception("b_node::split_child equal keys detected");
            }

            // создание и инициализация нового поддерева
            b_node * brother_to_child = new b_node(_target_tree);
            brother_to_child->_is_leaf = child->_is_leaf;
            unsigned count_of_keys_in_brother_to_child = _target_tree->_order_of_tree - 1;
            brother_to_child->_key_count = count_of_keys_in_brother_to_child;

            // переносим ключи, значения и поддеревья из заполненного поддерева в его нового брата
            memmove(brother_to_child->_key_array, (child->_key_array + _target_tree->_order_of_tree), sizeof(tkey) * count_of_keys_in_brother_to_child);
            memmove(brother_to_child->_value_array, (child->_value_array + _target_tree->_order_of_tree), sizeof(tvalue) * count_of_keys_in_brother_to_child);

            if (!(child->_is_leaf)) {
                unsigned count_of_sub_nodes_in_brother_to_child = _target_tree->_order_of_tree;

                memmove(brother_to_child->_sub_trees, (child->_sub_trees + _target_tree->_order_of_tree), sizeof(b_node *) * count_of_sub_nodes_in_brother_to_child);
            }

            child->_key_count = count_of_keys_in_brother_to_child;

            // переносим поддеревья текущего узла, чтобы вставить туда новое поддерево
            memmove((_sub_trees + index + 2), (_sub_trees + index + 1), sizeof(b_node *) * (_key_count - index));

            // вставляем новое поддерево в список поддеревьев текущего узла
            _sub_trees[index + 1] = brother_to_child;

            // переносим ключи и значения текущего узла, чтобы вставить ключ-значение нового элемента
            memmove((_key_array + index + 1), (_key_array + index), sizeof(tkey) * (_key_count - index));
            _key_array[index] = child->_key_array[_target_tree->_order_of_tree - 1];

            memmove((_value_array + index + 1), (_value_array + index), sizeof(tvalue) * (_key_count - index));
            _value_array[index] = std::move(child->_value_array[_target_tree->_order_of_tree - 1]);

            _key_count++;

            _target_tree->debug_with_guard("b_node::split_child method finished");
        }

        void insert_in_non_full(tkey const & key, tvalue && value)
        {
            int index_of_newly_inserted = _key_count - 1;
            tkey_comparer comparer;
            int comparison_result = comparer(_key_array[index_of_newly_inserted], key);

            if (comparison_result == 0) {
                _target_tree->warning_with_guard("b_tree::b_node::insert_in_non_full a key to insert is not unique");
                throw b_tree<tkey, tvalue, tkey_comparer>::insert_exception("b_tree::b_node::insert_in_non_full a key to insert is not unique");
            }

            if (_is_leaf) {
                // перемещаем ключи и значения в списках текущего узла и находим индекс для вставки нового элемента
                _target_tree->debug_with_guard("b_node::insert_in_non_full inserting to leaf");
                unsigned found_index = find_key(key);
                if (comparer(_key_array[found_index], key) == 0) {
                    _target_tree->warning_with_guard("b_tree::b_node::insert_in_non_full a key to insert is not unique");
                    throw b_tree<tkey, tvalue, tkey_comparer>::insert_exception("b_tree::b_node::insert_in_non_full a key to insert is not unique");
                }

                if (found_index != _key_count) {
                    memmove((_key_array + found_index + 1), (_key_array + found_index), sizeof(tkey) * (_key_count - found_index));
                    memmove((_value_array + found_index + 1), (_value_array + found_index), sizeof(tvalue) * (_key_count - found_index));
                }

                // вставляем новый элемент
                _key_array[found_index] = key;
                _value_array[found_index] = std::move(value);
                _key_count++;
            }
            else {
                while (index_of_newly_inserted >= 0 &&
                      (comparison_result = comparer(_key_array[index_of_newly_inserted], key)) > 0) {
                    index_of_newly_inserted--;
                }

                if (comparison_result == 0) {
                    _target_tree->warning_with_guard("b_tree::b_node::insert_in_non_full a key to insert is not unique");
                    throw b_tree<tkey, tvalue, tkey_comparer>::insert_exception("b_tree::b_node::insert_in_non_full a key to insert is not unique");
                }

                unsigned index_of_subtree_to_insert_to = index_of_newly_inserted + 1;

                if (_sub_trees[index_of_subtree_to_insert_to]->_key_count == (2 * _target_tree->_order_of_tree - 1)) {
                    _target_tree->debug_with_guard("b_node::insert_in_non_full splitting child");
                    split_child(index_of_subtree_to_insert_to, key, _sub_trees[index_of_subtree_to_insert_to]);

                    if (comparer(_key_array[index_of_subtree_to_insert_to], key) < 0) {
                        index_of_subtree_to_insert_to++;
                    }
                }

                _sub_trees[index_of_subtree_to_insert_to]->insert_in_non_full(key, std::move(value));
            }
        }
#pragma endregion

#pragma region deletion
        void delete_from_leaf(unsigned index)
        {
            if (index != _key_count - 1) {
                memmove((_key_array + index), (_key_array + index + 1), sizeof(tkey) * (_key_count - index - 1));
                memmove((_value_array + index), (_value_array + index + 1), sizeof(tvalue) * (_key_count - index - 1));
            }

            _key_count--;
        }

        void delete_from_inner_node(unsigned index)
        {
            // нашли ключ, который удаляем из дерева
            tkey key_to_delete = _key_array[index];

            unsigned min_count_of_nodes = _target_tree->_order_of_tree - 1;

            // Количество ключей в левом поддереве != минимальному, т.е. t-1
            if (_sub_trees[index]->_key_count > min_count_of_nodes) {
                _target_tree->trace_with_guard("b_node::delete_from_inner_node swapping element to delete with left sub node");
                // спускаемся в самое правое поддерево левого поддерева
                b_node *left_sub_node = _sub_trees[index];
                while (!(left_sub_node->_is_leaf)) {
                    left_sub_node = left_sub_node->_sub_trees[left_sub_node->_key_count];
                }

                // заменяем удаляемый на элемент из самого правого поддерева левого поддерева
                _key_array[index] = left_sub_node->_key_array[left_sub_node->_key_count - 1];
                _value_array[index] = std::move(left_sub_node->_value_array[left_sub_node->_key_count - 1]);

                // вызываем удаление ключа из дерева для ключа, на который мы заменили удаляемый
                _sub_trees[index]->delete_key_value_pair(_key_array[index], false);
            }
            else if (_sub_trees[index + 1]->_key_count > min_count_of_nodes) {
                _target_tree->trace_with_guard("b_node::delete_from_inner_node swapping element to delete with right sub node");
                b_node *right_subtree_node = _sub_trees[index + 1];
                while (!(right_subtree_node->_is_leaf)) {
                    right_subtree_node = right_subtree_node->_sub_trees[0];
                }

                _key_array[index] = right_subtree_node->_key_array[0];
                _value_array[index] = right_subtree_node->_value_array[0];
                _sub_trees[index + 1]->delete_key_value_pair(right_subtree_node->_key_array[0], false);
            }
            else {
                _target_tree->trace_with_guard("b_node::delete_from_inner_node merging nodes where divider is an element to delete");
                merge_sub_nodes(index);
                _sub_trees[index]->delete_key_value_pair(key_to_delete, true);
            }
        }

        void borrow_from_prev(unsigned index)
        {
            b_node *child = _sub_trees[index];
            b_node *prev_to_child = _sub_trees[index - 1];

            // переместим все ключи, значения и поддеревья в дочернем узле на 1 индекс
            memmove((child->_key_array + 1), (child->_key_array), sizeof(tkey) * child->_key_count);
            memmove((child->_value_array + 1), (child->_value_array), sizeof(tvalue) * child->_key_count);

            // вставим ключ из родительского узла в дочерний узел
            child->_key_array[0] = _key_array[index - 1];
            child->_value_array[0] = std::move(_value_array[index - 1]);
            child->_key_count++;

            if (!(child->_is_leaf)) {
                memmove((child->_sub_trees + 1), (child->_sub_trees), sizeof(b_node *) * (child->_key_count));

                child->_sub_trees[0] = prev_to_child->_sub_trees[prev_to_child->_key_count];
            }

            // вставим ключ из предыдущего узла в родительский узел
            _key_array[index - 1] = prev_to_child->_key_array[prev_to_child->_key_count - 1];
            _value_array[index - 1] = std::move(prev_to_child->_value_array[prev_to_child->_key_count - 1]);
            prev_to_child->_key_count--;
        }

        void borrow_from_next(unsigned index)
        {
            b_node *child = _sub_trees[index];
            b_node *next_to_child = _sub_trees[index + 1];

            // вставим ключ из родительского узла в дочерний узел
            child->_key_array[child->_key_count] = _key_array[index];
            child->_value_array[child->_key_count] = std::move(_value_array[index]);
            child->_key_count++;

            if (!(child->_is_leaf)) {
                child->_sub_trees[child->_key_count] = next_to_child->_sub_trees[0];

                memmove(next_to_child->_sub_trees, (next_to_child->_sub_trees + 1), sizeof(b_node *) * next_to_child->_key_count);
            }

            // вставим ключ из следующего узла в родительский узел
            _key_array[index] = next_to_child->_key_array[0];
            _value_array[index] = std::move(next_to_child->_value_array[0]);
            next_to_child->_key_count--;

            memmove(next_to_child->_key_array, (next_to_child->_key_array + 1), sizeof(tkey) * next_to_child->_key_count);
            memmove(next_to_child->_value_array, (next_to_child->_value_array + 1), sizeof(tvalue) * next_to_child->_key_count);
        }

        void merge_sub_nodes(unsigned index)
        {
            b_node * child = _sub_trees[index];
            b_node * next_to_child = _sub_trees[index + 1];

            unsigned order_of_target_tree = _target_tree->_order_of_tree;

            // перемещаем ключ-разделитель из родительского узла в дочерний
            child->_key_array[order_of_target_tree - 1] = _key_array[index];
            child->_value_array[order_of_target_tree - 1] = std::move(_value_array[index]);
            _key_count--;

            // переносим все ключи-значения из next_to_child в child
            memmove((child->_key_array + order_of_target_tree), next_to_child->_key_array, sizeof(tkey) * next_to_child->_key_count);

            memmove((child->_value_array + order_of_target_tree), next_to_child->_value_array, sizeof(tvalue) * next_to_child->_key_count);

            child->_key_count += next_to_child->_key_count + 1;

            if (!(child->_is_leaf)) {
                memmove((child->_sub_trees + order_of_target_tree), next_to_child->_sub_trees, sizeof(b_node *) * (next_to_child->_key_count + 1));
            }

            // переносим ключи-значения и указатели на поддеревья в родительском элементе
            memmove((_key_array + index), (_key_array + index + 1), sizeof(tkey) * (_key_count - index));
            memmove((_value_array + index), (_value_array + index + 1), sizeof(tvalue) * (_key_count - index));
            memmove((_sub_trees + index + 1), (_sub_trees + index + 2), sizeof(b_node *) * (_key_count - index));

            next_to_child->~b_node();
        }

        void fill_sub_node(unsigned index)
        {
            if (index != 0 && _sub_trees[index - 1]->_key_count >= _target_tree->_order_of_tree) {
                _target_tree->trace_with_guard("b_node::fill_sub_node borrowing an key-value pair from previous sub node");
                borrow_from_prev(index);
            }
            else if (index != _key_count && _sub_trees[index + 1]->_key_count >= _target_tree->_order_of_tree) {
                _target_tree->trace_with_guard("b_node::fill_sub_node borrowing an key-value pair from next sub node");
                borrow_from_next(index);
            }
            else {
                _target_tree->trace_with_guard("b_node::fill_sub_node merging sub nodes");
                if (index != _key_count) {
                    merge_sub_nodes(index);
                }
                else
                    merge_sub_nodes(index - 1);
            }
        }

        tvalue delete_key_value_pair(tkey const & key, bool deleting_initial)
        {
            unsigned found_index = find_key(key);

            tkey_comparer comparer;

            if (found_index < _key_count && comparer(_key_array[found_index], key) == 0) {
                // ключ, который надо удалить, в этом узле
                tvalue to_return;
                if (deleting_initial) {
                    to_return = std::move(_value_array[found_index]);
                }
                if (_is_leaf) {
                    this->_target_tree->debug_with_guard("b_node::delete_key_value_pair key-value to delete is inside a leaf node");
                    delete_from_leaf(found_index);
                }
                else {
                    this->_target_tree->debug_with_guard("b_node::delete_key_value_pair key-value to delete is inside an inner node");
                    delete_from_inner_node(found_index);
                }
                if (deleting_initial) {
                    return to_return;
                }
            }
            else {
                // ключ, который надо удалить, не в этом узле, продолжаем поиск
                if (_is_leaf) {
                    _target_tree->warning_with_guard("removing_template_method::remove::b_node::delete_key_value_pair no key in tree")
                            ->trace_with_guard("b_node::delete_key_value_pair method finished");
                    throw b_tree<tkey, tvalue, tkey_comparer>::remove_exception("removing_template_method::remove::b_node::delete_key_value_pair no key in tree");
                }
                bool flag = (found_index == _key_count);

                // Минимальное количество ключей в некорневом узле -- t-1. Мы собираемся удалить ключ из узла, поэтому заполним поддерево[found_index], с помощью элементов из его брата
                if (_sub_trees[found_index]->_key_count < _target_tree->_order_of_tree) {
                    fill_sub_node(found_index);
                }

                if (flag && found_index > _key_count) {
                    return _sub_trees[found_index - 1]->delete_key_value_pair(key, deleting_initial);
                } else {
                    return _sub_trees[found_index]->delete_key_value_pair(key, deleting_initial);
                }
            }
        }
#pragma endregion
        class b_node_exception final : public std::exception {
        private:
            std::string _message;

        public:
            explicit b_node_exception(std::string message)
                    : _message(std::move(message)) {

            }

            [[nodiscard]] char const *what() const noexcept override {
                return _message.c_str();
            }
        };

#pragma region b node rule 5
        explicit b_node(b_tree<tkey, tvalue, tkey_comparer> * target_tree)
        : _target_tree(target_tree)
        {
            _key_count = 0;
            _is_leaf = true;

            try {
                _key_array = reinterpret_cast<tkey *>
                        (_target_tree->allocate_with_guard(sizeof(tkey) * (2 * _target_tree->_order_of_tree - 1)));
                _value_array = reinterpret_cast<tvalue * >
                        (_target_tree->allocate_with_guard(sizeof(tvalue) * (2 * _target_tree->_order_of_tree - 1)));
                _sub_trees = reinterpret_cast<b_node **>
                        (_target_tree->allocate_with_guard(sizeof(b_node *) * 2 * _target_tree->_order_of_tree));
            }
            catch (memory::memory_exception const &ex)
            {
                throw b_node_exception("b_node constructor:: allocating memory error for b_node properties");
            }
            catch (std::bad_alloc const &ex)
            {
                throw b_node_exception("b_node constructor:: allocating memory error for b_node properties");
            }
        }

        // конструктор копирования
        b_node(b_node const & obj)
        : b_node(obj._target_tree)
        {
            memcpy(_key_array, obj._key_array, sizeof(tkey) * obj._key_count);
            memcpy(_value_array, obj._value_array, sizeof(tvalue) * obj._key_count);
            memcpy(_sub_trees, obj._sub_trees, sizeof(b_node **) * (obj._key_count + 1));
        }

        // конструктор перемещения
        b_node(b_node && obj) noexcept
        : b_node(obj._target_tree)
        {
            memmove(_key_array, obj._key_array, sizeof(tkey) * obj._key_count);
            memmove(_value_array, obj._value_array, sizeof(tvalue) * obj._key_count);
            memmove(_sub_trees, obj._sub_trees, sizeof(b_node **) * (obj._key_count + 1));

            _target_tree->deallocate_with_guard(obj._key_array);
            _target_tree->deallocate_with_guard(obj._value_array);
            _target_tree->deallocate_with_guard(obj._sub_trees);

            obj._key_array = nullptr;
            obj._value_array = nullptr;
            obj._sub_trees = nullptr;
        }

        // оператор присваивания копированием
        b_node &operator=(b_node const & obj) = delete;

        // оператор присваивания перемещением
        b_node &operator=(b_node && obj) = delete;

        virtual ~b_node()
        {
            if (_key_array != nullptr) {
                _target_tree->deallocate_with_guard(_key_array);
            }
            if (_key_array != nullptr) {
                _target_tree->deallocate_with_guard(_value_array);
            }
            if (_sub_trees != nullptr) {
                _target_tree->deallocate_with_guard(_sub_trees);
            }
        }

#pragma endregion
    };

protected:
    b_node * _root;

public:
#pragma region exceptions
    class insert_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit insert_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class find_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit find_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class remove_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit remove_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    class iterator_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit iterator_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };
#pragma endregion

#pragma region iterator
public:
    class iterator final
    {
        // iterates: first -- node, then its sub-nodes
        friend class b_tree<tkey, tvalue, tkey_comparer>;

    private:
        b_node *_current_node;
        // parent node and index of sub-node
        std::stack<std::pair<b_node *, unsigned> > _path;

    public:
        explicit iterator(b_node *current_node)
        {
            this->_current_node = current_node;
        }

        bool operator==(iterator const &other) const
        {
            return (_current_node == other._current_node);
        }

        bool operator!=(iterator const &other) const
        {
            return (_current_node != other._current_node);
        }

        iterator& operator++()
        {
            if (_current_node == nullptr) {
                throw iterator_exception("b_tree_incorrect::iterator iterator is out of range");
            }

            unsigned index_of_last_sub_node_in_parent_node = 0;

            if (_current_node->_is_leaf) {
                if (_path.empty()) {
                    _current_node = nullptr;
                    return *this;
                }

                while (!(_path.empty())) {
                    auto parent_and_index_of_this_sub_node = _path.top();
                    b_node * parent_node = parent_and_index_of_this_sub_node.first;
                    unsigned index_of_this_sub_node = parent_and_index_of_this_sub_node.second;
                    _path.pop();

                    index_of_last_sub_node_in_parent_node = parent_node->_key_count;
                    if (index_of_this_sub_node != index_of_last_sub_node_in_parent_node) {
                        // go to the next sub-node, if current is not the last
                        index_of_this_sub_node++;
                        _path.push(std::pair<b_node *, unsigned >(parent_node, index_of_this_sub_node));
                        _current_node = parent_node->_sub_trees[index_of_this_sub_node];
                        return *this;
                    }
                    // current sub-node is the last, thus go to the grandparent
                }

                if (_path.empty()) {
                    _current_node = nullptr;
//                    return *this;
                }
            }
            else {
                // current node is an inner node, so we need to go to its first sub-node
                _path.push(std::pair<b_node *, unsigned >(_current_node, 0));
                _current_node = _current_node->_sub_trees[0];
            }

            return *this;
        }

        iterator operator++(int not_used)
        {
            auto result = *this;
            ++*this;
            return *this;
        }

        std::tuple<unsigned int, const tkey*, const tvalue*, unsigned int, bool > operator*() const
        {
            return std::tuple<unsigned int, const tkey*, const tvalue*, unsigned int, bool >(_path.size(), _current_node->_key_array, _current_node->_value_array, _current_node->_key_count, _current_node->_is_leaf);
        }

    };

public:
    iterator begin_iter() const noexcept
    {
        return b_tree<tkey, tvalue, tkey_comparer>::iterator(_root);
    }

    iterator end_iter() const noexcept
    {
        return b_tree<tkey, tvalue, tkey_comparer>::iterator(nullptr);
    }

#pragma endregion

protected:
#pragma region template methods
    class template_method_basics:
            protected logger_holder,
            protected memory_holder
    {
        friend class b_tree<tkey, tvalue, tkey_comparer>;

    protected:
        b_tree<tkey, tvalue, tkey_comparer> * _target_tree;

        std::pair<b_node *, unsigned> find_inner(tkey const &key, b_node * current_node)
        {
            tkey_comparer comparer;
            int comparison_result;
            unsigned i = 0;
            while (i < current_node->_key_count && (comparison_result = comparer(key, current_node->_key_array[i])) > 0) {
                i++;
            }

            if (comparison_result == 0) {
                return {current_node, i};
            }

            if (current_node->_is_leaf) {
                return { nullptr, 0 };
            }

            return find_inner(key, current_node->_sub_trees[i]);
        }

    private:
        [[nodiscard]] logger *get_logger() const noexcept override
        {
            return _target_tree->_logger;
        }

        [[nodiscard]] memory *get_memory() const noexcept override
        {
            return _target_tree->_allocator;
        }

    public:
        explicit template_method_basics(b_tree<tkey, tvalue, tkey_comparer> *target_tree)
                : _target_tree(target_tree)
        {

        }

        ~template_method_basics() override = default;
    };

    class finding_template_method
            : public template_method_basics
    {
    public:
        tvalue const &find(tkey const & key) {
            this->trace_with_guard("finding_template_method::find method started");

            if (this->_target_tree->_root == nullptr) {
                throw b_tree<tkey, tvalue, tkey_comparer>::find_exception("finding_template_method::find trying to find a value in an empty tree");
            }

            std::pair<b_node *, unsigned> find_inner_result = this->find_inner(key, this->_target_tree->_root);
            b_node * node = find_inner_result.first;
            unsigned found_key_index = find_inner_result.second;

            if (node == nullptr) {
                this->warning_with_guard("finding_template_method::find key-value pair not found")
                        ->trace_with_guard("finding_template_method::find method finished");
                throw b_tree<tkey, tvalue, tkey_comparer>::find_exception("finding_template_method::find key-value pair not found");
            }

            this->trace_with_guard("finding_template_method::find method finished");
            return node->_value_array[found_key_index];
        }

    public:
        explicit finding_template_method(b_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : template_method_basics(target_tree)
        {

        }

        ~finding_template_method() override = default;
    };

    class insertion_template_method
            : public template_method_basics
    {
    public:
        void insert(tkey const &key, tvalue &&value)
        {
            this->trace_with_guard("insertion_template_method::insert method started");
            if (this->_target_tree->_root == nullptr) {
                this->debug_with_guard("insertion_template_method::insert inserting to root for the first time");
                this->_target_tree->_root = new b_node(this->_target_tree);
                this->_target_tree->_root->_key_array[0] = std::move(key);
                this->_target_tree->_root->_value_array[0] = std::move(value);
                this->_target_tree->_root->_key_count = 1;
            } else {
                if (this->_target_tree->_root->_key_count == (2 * this->_target_tree->_order_of_tree) - 1) {
                    // корень дерева заполнен
                    this->debug_with_guard("insertion_template_method::insert splitting a root");
                    b_node * new_node = new b_node(this->_target_tree);
                    new_node->_is_leaf = false;

                    new_node->_sub_trees[0] = this->_target_tree->_root;

                    new_node->split_child(0, key, this->_target_tree->_root);

                    this->_target_tree->_root = new_node;

                    unsigned i = 0;

                    tkey_comparer comparer;
                    int comparison_result = comparer(new_node->_key_array[0], key);
                    if (comparison_result < 0) {
                        i++;
                    } else if (comparison_result == 0) {
                        this->warning_with_guard("insertion_template_method::insert a key to insert is not unique")
                        ->trace_with_guard("insertion_template_method::insert method finished");
                        throw b_tree<tkey, tvalue, tkey_comparer>::insert_exception("insertion_template_method::insert a key to insert is not unique");
                    }

                    new_node->_sub_trees[i]->insert_in_non_full(key, std::move(value));
                } else {
                    this->_target_tree->_root->insert_in_non_full(key, std::move(value));
                }
            }
            this->trace_with_guard("insertion_template_method::insert method finished");
        }

    public:
        explicit insertion_template_method(b_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : template_method_basics(target_tree)
                {

                }

        ~insertion_template_method() override = default;
    };

    class removing_template_method
            : public template_method_basics
    {

    public:
    tvalue remove(tkey const &key)
    {
        this->trace_with_guard("removing_template_method::remove method started");
        if (this->_target_tree->_root == nullptr) {
            this->warning_with_guard("removing_template_method::remove trying to delete from an empty tree")
                    ->trace_with_guard("removing_template_method::remove method finished");
            throw b_tree<tkey, tvalue, tkey_comparer>::remove_exception("removing_template_method::remove trying to delete from an empty tree");
        }

        tvalue to_return_value = this->_target_tree->_root->delete_key_value_pair(key, true);

        if (this->_target_tree->_root->_key_count == 0) {
            this->debug_with_guard("removing_template_method::remove new root after merging");
            b_node *tmp = this->_target_tree->_root;
            if (tmp->_is_leaf) {
                tmp->~b_node();
                this->_target_tree->_root = nullptr;
            } else {
                this->_target_tree->_root = this->_target_tree->_root->_sub_trees[0];
                delete tmp;
            }
        }

        this->trace_with_guard("removing_template_method::remove method finished");
        return to_return_value;
    }

    public:
        explicit removing_template_method(b_tree<tkey, tvalue, tkey_comparer> *target_tree)
        : template_method_basics(target_tree)
                {

                }

        ~removing_template_method() override = default;
    };

#pragma endregion

protected:
    insertion_template_method * _insertion;
    finding_template_method * _finding;
    removing_template_method * _removing;

#pragma region rule 5
private:
    b_tree(insertion_template_method * insertion, finding_template_method * finding, removing_template_method * removing, unsigned order_of_tree, logger *logger, memory *allocator)
    : _insertion(insertion), _finding(finding), _removing(removing), _order_of_tree(order_of_tree), _logger(logger), _allocator(allocator), _root(nullptr)
    {

    }

public:
    b_tree(unsigned order_of_tree, logger * logger = nullptr, memory * allocator = nullptr)
    : b_tree(new insertion_template_method(this), new finding_template_method(this), new removing_template_method(this), order_of_tree, logger, allocator)
    {

    }

    void clear_up_nodes(b_node * curr)
    {
        if (curr == nullptr) { return;}

        if (!(curr->_is_leaf)) {
            unsigned i;
            for (i = 0; i < curr->_key_count; i++) {
                clear_up_nodes(curr->_sub_trees[i]);
            }
        }
        curr->~b_node();
    }

    ~b_tree() override
    {
        clear_up_nodes(_root);

        delete _insertion;
        delete _finding;
        delete _removing;
    }
#pragma endregion

#pragma region logger and memory holders contracts
private:
    [[nodiscard]] logger *get_logger() const noexcept override
    {
        return _logger;
    }

    [[nodiscard]] memory *get_memory() const noexcept override
    {
        return _allocator;
    }
#pragma endregion

#pragma region associative container contract
public:
    void insert(tkey const &key, tvalue &&value) override
    {
        _insertion->insert(key, std::move(value));
    }

    tvalue const &get(tkey const &key) override
    {
        return _finding->find(key);
    }

    tvalue remove(tkey const &key) override
    {
        return _removing->remove(key);
    }
#pragma endregion

};

#endif //B_TREE_H