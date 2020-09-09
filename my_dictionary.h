#ifndef DICTIONARY_MY_DICTIONARY_H
#define DICTIONARY_MY_DICTIONARY_H

#include <exception>
#include <type_traits>
#include <functional>
#include <vector>

template<class TKey, class TValue>
class Dictionary
{
public:
    virtual ~Dictionary() = default;

    virtual const TValue& Get(const TKey& key) const = 0;
    virtual void Set(const TKey& key, const TValue& value) = 0;
    virtual bool IsSet(const TKey& key) const = 0;
};

template<class TKey>
class NotFoundException : public std::exception
{
public:
    virtual const TKey& GetKey() const noexcept = 0;
};

template<class TKey>
        class MyNotFoundException : NotFoundException<TKey>
        {
            const TKey key;
        public:
            MyNotFoundException(TKey k)
            :key(k)
            {}

            virtual const TKey& GetKey() const noexcept {
                return key;
            }
        };


template <class T>
        using equality_comparison = decltype(std::declval<T&>() == std::declval<T&>());

template <class T>
        using comparison = decltype(std::declval<T&>() < std::declval<T&>());

template<class T>
        using hashable = decltype(std::declval<std::hash<T>>()(std::declval<T>()));

template <class T, typename = std::void_t<>>
        struct is_equality
                :std::false_type {};
template <class T>
        struct is_equality<T, std::void_t<equality_comparison<T>>>
                : std::is_same<equality_comparison<T>, bool>
                {};

template <class T, typename = std::void_t<>>
        struct is_std_hashable
                :std::false_type{};
template <class T>
        struct is_std_hashable<T, std::void_t<hashable<T>>>
                :std::true_type
                {};

template <class T, typename = std::void_t<>>
        struct is_comparable
                :std::false_type {};
template <class T>
        struct is_comparable<T, std::void_t<comparison<T>>>
                : std::is_same<comparison<T>, bool>
                {};

template <class TKey, class TValue, class Enable = void>
class MyDictionary : Dictionary<TKey, TValue>{};

/*
template <class TKey, class TValue>
class MyDictionary<TKey, TValue,//for binary tree
        typename std::enable_if<is_comparable<TKey>::value && is_equality<TValue>::value>::type>
        : Dictionary<TKey, TValue>
        {

};

template <class TKey, class TValue>
class MyDictionary<TKey, TValue,//for hash_table
        typename std::enable_if<is_std_hashable<TKey>::value && is_comparable<TValue>::value>::type>
        : Dictionary<TKey, TValue>
        {

};
*/
template <class TKey, class TValue>
class MyDictionary<TKey, TValue,//for simple pair walkthrough
        typename std::enable_if<is_equality<TKey>::value && is_equality<TValue>::value>::type>
        : Dictionary<TKey, TValue>
        {
        private:

    std::vector<std::pair<TKey,TValue>> pairs;

    std::size_t find_value(const TKey& key) const{
        for (std::size_t i = 0; i< pairs.size(); i++)
            if (pairs[i].first == key)
                return i;

        return pairs.size();
    }

        public:
    MyDictionary() = default;
    ~MyDictionary() = default;

    explicit MyDictionary(std::size_t dict_size)
    : pairs(dict_size)
    {}

    virtual const TValue& Get(const TKey& key) const{
        size_t place = find_value(key);

        if (place != pairs.size())
            return pairs[place].second;

        throw MyNotFoundException<TKey>(key);

    }
    virtual void Set(const TKey& key, const TValue& value){
        size_t place = find_value(key);
        if (place != pairs.size())
            pairs[place].second = value;
        else
            pairs.emplace_back(std::make_pair(key,value));
    }

    virtual bool IsSet(const TKey& key) const{
        return (find_value(key) != pairs.size());
    }
};

#endif //DICTIONARY_MY_DICTIONARY_H
