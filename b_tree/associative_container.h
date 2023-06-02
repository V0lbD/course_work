#ifndef ASSOCIATIVE_CONTAINER_H
#define ASSOCIATIVE_CONTAINER_H

template<typename tkey, typename tvalue>
class associative_container
{
public:

    virtual ~associative_container() = default;

    virtual void insert(tkey const &key, tvalue &&value) = 0;

    virtual tvalue const &get(tkey const &key) = 0;

    virtual tvalue remove(tkey const &key) = 0;
};

#endif //ASSOCIATIVE_CONTAINER_H
