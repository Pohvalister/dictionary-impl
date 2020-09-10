#ifndef DICTIONARY_MY_DICTIONARY_H
#define DICTIONARY_MY_DICTIONARY_H

#include <exception>
#include <type_traits>
#include <functional>
#include <vector>
#include <stack>

template<class TKey, class TValue>
class Dictionary {
public:
    virtual ~Dictionary() = default;

    virtual const TValue &Get(const TKey &key) const = 0;

    virtual void Set(const TKey &key, const TValue &value) = 0;

    virtual bool IsSet(const TKey &key) const = 0;
};

template<class TKey>
class NotFoundException : public std::exception {
public:
    virtual const TKey &GetKey() const noexcept = 0;
};

template<class TKey>
class MyNotFoundException : NotFoundException<TKey> {
    const TKey key;
public:
    MyNotFoundException(TKey k)
            : key(k) {}

    virtual const TKey &GetKey() const noexcept {
        return key;
    }
};


//defining cases of different data capabilities to get different scenarios of structure working
template<class T>
using equality_comparison = decltype(std::declval<T &>() == std::declval<T &>());

template<class T>
using comparison = decltype(std::declval<T &>() < std::declval<T &>());

template<class T>
using hashable = decltype(std::declval<std::hash<T>>()(std::declval<T>()));

template<class T, typename = std::void_t<>>
struct is_equality
        : std::false_type {
};
template<class T>
struct is_equality<T, std::void_t<equality_comparison<T>>>
        : std::is_same<equality_comparison<T>, bool> {
};

template<class T, typename = std::void_t<>>
struct is_std_hashable
        : std::false_type {
};
template<class T>
struct is_std_hashable<T, std::void_t<hashable<T>>>
        : std::true_type {
};

template<class T, typename = std::void_t<>>
struct is_comparable
        : std::false_type {
};
template<class T>
struct is_comparable<T, std::void_t<comparison<T>>>
        : std::is_same<comparison<T>, bool> {
};

//default template dictionary
template<class TKey, class TValue, class Enable_equ = void>
class MyDictionary: Dictionary<TKey, TValue> {
};


//hash function dictionary: uses "==" and std::hash<T>
template<class TKey, class TValue>
class MyDictionary<TKey, TValue,//for hash_table
        typename std::enable_if<is_std_hashable<TKey>::value && is_equality<TKey>::value>::type
        >
        : Dictionary<TKey, TValue> {
private:
    const std::size_t MAS_SIZE = 1023;
    const std::size_t SIZE_MULTIPLIER = 3;
    const std::size_t PART_EMPTY = 4;
    int amount = 0;

    std::vector<std::vector<std::pair<TKey, TValue>>> table;

    inline std::size_t get_place(const TKey &key) const {
        return (std::hash<TKey>{}(key)) % table.size();
    }

    void resize_table() {
        std::size_t new_size = table.size() * SIZE_MULTIPLIER;
        std::vector<std::vector<std::pair<TKey, TValue>>> tmp_table(new_size);
        std::swap(table, tmp_table);

        for (std::vector<std::pair<TKey, TValue>> vec : tmp_table)
            for (std::pair<TKey, TValue> pair : vec)
                this->Set(pair.first, pair.second);
    }

public:
    MyDictionary()
            : table(MAS_SIZE) {
        std::cout<<"hash\n";
    }

    MyDictionary(std::size_t table_size, std::size_t f)
            : MAS_SIZE(table_size), table(table_size), PART_EMPTY(f) {}

    virtual ~MyDictionary() = default;

    virtual const TValue &Get(const TKey &key) const {
        std::size_t hash_val = get_place(key);

        for (int i = 0; i < table[hash_val].size(); i++)
            if (table[hash_val][i].first == key)
                return table[hash_val][i].second;

        throw MyNotFoundException<TKey>(key);
    }

    virtual void Set(const TKey &key, const TValue &value) {
        amount++;
        if (amount > table.size() / PART_EMPTY) {
            resize_table();
        }

        std::size_t hash_val = get_place(key);

        for (std::pair<TKey, TValue> data : table[hash_val]) {
            if (data.first == key)
                data.second = value;
            return;
        }
        table[hash_val].emplace_back(std::make_pair(key, value));
    }

    virtual bool IsSet(const TKey &key) const {
        std::size_t hash_val = get_place(key);

        for (std::pair<TKey, TValue> data : table[hash_val]) {
            if (data.first == key)
                return true;
        }

        return false;
    }

};

//binary tree dictionary: using "==" and "<" operators
template<class TKey, class TValue>
class MyDictionary<TKey, TValue,//for binary tree
        typename std::enable_if<is_comparable<TKey>::value && (!is_std_hashable<TKey>::value) && is_equality<TKey>::value>::type
>
        : Dictionary<TKey, TValue> {
    //Binary-tree node
    struct DataNode {
        TKey key;
        TValue val;
        DataNode *left;
        DataNode *right;

        DataNode(const TKey &k, const TValue &v)
                : key(k), val(v), left(nullptr), right(nullptr) {}
    };

    DataNode *root;

    DataNode *find_value(const TKey &key) const {
        DataNode *data_pointer = root;
        while (data_pointer != nullptr) {
            if (data_pointer->key == key)
                return data_pointer;
            if (data_pointer->key < key)
                data_pointer = data_pointer->left;
            else
                data_pointer = data_pointer->right;
        }
        return nullptr;
    }

public:
    MyDictionary()
            : root(nullptr) {
        std::cout<<"heap\n";
    }

    ~MyDictionary() {
        std::stack<DataNode *> st;
        st.push(root);
        while (st.size() != 0) {
            DataNode *pointer = st.top();
            st.pop();
            if (pointer->left != nullptr)
                st.push(pointer->left);
            if (pointer->right != nullptr)
                st.push(pointer->right);
            delete pointer;
        }
    };

    virtual const TValue &Get(const TKey &key) const {
        DataNode *data = find_value(key);
        if (data != nullptr)
            return data->val;

        throw MyNotFoundException<TKey>(key);
    }

    virtual void Set(const TKey &key, const TValue &value) {
        if (root == nullptr) {
            root = new DataNode(key, value);
            return;
        }
        DataNode *data_pointer = root;
        while (true) {
            if (data_pointer->key == key) {
                data_pointer->val = value;
                return;
            }
            if (data_pointer->key < key) {
                if (data_pointer->left == nullptr) {
                    data_pointer->left = new DataNode(key, value);
                    return;
                }
                data_pointer = data_pointer->left;
            } else {
                if (data_pointer->right == nullptr) {
                    data_pointer->right = new DataNode(key, value);
                    return;
                }
                data_pointer = data_pointer->right;
            }
        }
    }

    virtual bool IsSet(const TKey &key) const {
        return find_value(key) != nullptr;
    }
};

//general ineffective dictionary: uses "==" operator
template<class TKey, class TValue>
class MyDictionary<TKey, TValue,
        typename std::enable_if<!(is_comparable<TKey>::value) && (!is_std_hashable<TKey>::value) && is_equality<TKey>::value>::type
>
        : Dictionary<TKey, TValue> {

private:
    std::vector<std::pair<TKey, TValue>> pairs;

    std::size_t find_value(const TKey &key) const {
        for (std::size_t i = 0; i < pairs.size(); i++)
            if (pairs[i].first == key)
                return i;

        return pairs.size();
    }

public:
    MyDictionary(){
        std::cout<<"basic\n";
    }
    // = default;

    ~MyDictionary() = default;

    virtual const TValue &Get(const TKey &key) const {
        size_t place = find_value(key);
        if (place != pairs.size())
            return pairs[place].second;

        throw MyNotFoundException<TKey>(key);
    }

    virtual void Set(const TKey &key, const TValue &value) {
        size_t place = find_value(key);
        if (place != pairs.size())
            pairs[place].second = value;
        else
            pairs.emplace_back(std::make_pair(key, value));
    }

    virtual bool IsSet(const TKey &key) const {
        return (find_value(key) != pairs.size());
    }
};


#endif //DICTIONARY_MY_DICTIONARY_H
