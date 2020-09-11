#ifndef DICTIONARY_MY_DICTIONARY_H
#define DICTIONARY_MY_DICTIONARY_H

#include <exception>
#include <type_traits>
#include <functional>
#include <vector>
#include <stack>

//dictionary interface
template<class TKey, class TValue>
class Dictionary {
public:
    virtual ~Dictionary() = default;

    virtual const TValue &Get(const TKey &key) const = 0;

    virtual void Set(const TKey &key, const TValue &value) = 0;

    virtual bool IsSet(const TKey &key) const = 0;
};

//error interface
template<class TKey>
class NotFoundException : public std::exception {
public:
    virtual const TKey &GetKey() const noexcept = 0;
};

//error implementation
template<class TKey>
class DictionaryNotFoundException : NotFoundException<TKey> {
    const TKey key;
public:
    explicit DictionaryNotFoundException(TKey k)
            : key(k) {}

    virtual const TKey &GetKey() const noexcept {
        return key;
    }
};


//defining cases of different data capabilities to get right scenario of structure working
template<class T>
using equality_comparison = decltype(std::declval<T &>() == std::declval<T &>());

template<class T>
using comparison = decltype(std::declval<T &>() < std::declval<T &>());

template<class T>
using hashable = decltype(std::declval<std::hash<T>>()(std::declval<T>()));

template<class T, typename = std::void_t<>>
struct is_equal
        : std::false_type {
};
template<class T>
struct is_equal<T, std::void_t<equality_comparison<T>>>
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

//template default dictionaries
//hash function dictionary: uses "==" and std::hash<T>
template<class TKey, class TValue, class Enable = void>
class HashDictionary : Dictionary<TKey, TValue> {
};

//binary tree dictionary: uses "==" and "<" operators
template<class TKey, class TValue, typename Enable = void>
class TreeDictionary : Dictionary<TKey, TValue> {
};

//general ineffective dictionary: uses "==" operator
template<class TKey, class TValue, typename Enable = void>
class ListDictionary : Dictionary<TKey, TValue> {
};


template<class TKey, class TValue>
class HashDictionary<TKey, TValue,
        typename std::enable_if<is_std_hashable<TKey>::value && is_equal<TKey>::value>::type
>
        : Dictionary<TKey, TValue> {
private:
    const std::size_t MAS_SIZE = 9973;
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

        amount = 0;
        for (const std::vector<std::pair<TKey, TValue>> &vec : tmp_table)
            for (const std::pair<TKey, TValue> &pair : vec)
                this->Set(pair.first, pair.second);
    }

public:
    HashDictionary()
            : table(MAS_SIZE) {
    }

    virtual ~HashDictionary() = default;

    virtual const TValue &Get(const TKey &key) const {
        std::size_t hash_val = get_place(key);

        for (int i = 0; i < table[hash_val].size(); i++)
            if (table[hash_val][i].first == key)
                return table[hash_val][i].second;

        throw DictionaryNotFoundException<TKey>(key);
    }

    virtual void Set(const TKey &key, const TValue &value) {
        amount++;
        if (amount > table.size() / PART_EMPTY)
            resize_table();

        std::size_t hash_val = get_place(key);

        for (std::pair<TKey, TValue> &data : table[hash_val])
            if (data.first == key) {
                data.second = value;
                amount--;
                return;
            }

        table[hash_val].emplace_back(key, value);
    }

    virtual bool IsSet(const TKey &key) const {
        std::size_t hash_val = get_place(key);

        for (const std::pair<TKey, TValue> &data : table[hash_val])
            if (data.first == key)
                return true;

        return false;
    }
};

template<class TKey, class TValue>
class TreeDictionary<TKey, TValue,
        typename std::enable_if<is_comparable<TKey>::value && is_equal<TKey>::value>::type
>
        : Dictionary<TKey, TValue> {

    //AVL-tree node
    struct DataNode {
        TKey key;
        TValue val;
        unsigned char high;
        DataNode *left;
        DataNode *right;

        DataNode(const TKey &k, const TValue &v)
                : key(k), val(v), left(nullptr), right(nullptr), high(1) {}
    };

    //AVL-tree balance pack
    inline unsigned char height(DataNode *pointer) {
        return pointer == nullptr ? 0 : pointer->high;
    }

    inline int height_balance(DataNode *node) {
        return height(node->right) - height(node->left);
    }

    inline void height_restore(DataNode *node) {
        unsigned char hl = height(node->left), hr = height(node->right);
        node->high = (hl > hr ? hl : hr) + 1;
    }

    DataNode *rotate_right(DataNode *node) {
        DataNode *new_top = node->left;
        node->left = new_top->right;
        new_top->right = node;
        height_restore(node);
        height_restore(new_top);
        return new_top;
    }

    DataNode *rotate_left(DataNode *node) {
        DataNode *new_top = node->right;
        node->right = new_top->left;
        new_top->left = node;
        height_restore(node);
        height_restore(new_top);
        return new_top;
    }

    DataNode *balance(DataNode *node) {
        height_restore(node);
        if (height_balance(node) == 2) {
            if (height_balance(node->right) < 0)
                node->right = rotate_right(node->right);
            return rotate_left(node);
        }
        if (height_balance(node) == -2) {
            if (height_balance(node->left) > 0)
                node->left = rotate_left(node->left);
            return rotate_right(node);
        }
        return node;
    }

    //returns top node of balanced subtree after insertion
    DataNode *insert(DataNode *pointer, const TKey &key, const TValue &value) {
        if (pointer == nullptr)
            return new DataNode(key, value);
        if (key == pointer->key)
            pointer->val = value;
        else if (key < pointer->key)
            pointer->left = insert(pointer->left, key, value);
        else
            pointer->right = insert(pointer->right, key, value);

        return balance(pointer);
    }

    DataNode *root;

    //returns pointer to data or nullptr if no
    DataNode *find_value(const TKey &key) const {
        DataNode *data_pointer = root;
        while (data_pointer != nullptr) {
            if (data_pointer->key == key)
                return data_pointer;
            if (key < data_pointer->key)
                data_pointer = data_pointer->left;
            else
                data_pointer = data_pointer->right;
        }
        return nullptr;
    }

public:
    TreeDictionary()
            : root(nullptr) {
    }

    ~TreeDictionary() {//delete tree
        std::stack<DataNode *> st;
        if (root != nullptr)
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

        throw DictionaryNotFoundException<TKey>(key);
    }

    virtual void Set(const TKey &key, const TValue &value) {
        root = insert(root, key, value);
    }

    virtual bool IsSet(const TKey &key) const {
        return find_value(key) != nullptr;
    }
};

template<class TKey, class TValue>
class ListDictionary<TKey, TValue,
        typename std::enable_if<is_equal<TKey>::value>::type
>
        : public Dictionary<TKey, TValue> {

private:
    //Linkedlist node
    struct DataNode {
        TKey key;
        TValue val;
        DataNode *next;

        DataNode(const TKey &k, const TValue &v)
                : key(k), val(v), next(nullptr) {}
    };

    DataNode *root = nullptr;

    DataNode *find_value(const TKey &key) const {
        DataNode *pointer = root;
        while (pointer != nullptr)
            if (pointer->key == key)
                return pointer;
            else
                pointer = pointer->next;

        return nullptr;
    }

public:
    ListDictionary() = default;

    ~ListDictionary() {
        std::stack<DataNode *> st;
        if (root != nullptr)
            st.push(root);
        while (st.size() != 0) {
            DataNode *pointer = st.top();
            st.pop();
            if (pointer->next != nullptr)
                st.push(pointer->next);
            delete pointer;
        }
    }

    virtual const TValue &Get(const TKey &key) const {
        DataNode *pointer = find_value(key);
        if (pointer != nullptr)
            return pointer->val;

        throw DictionaryNotFoundException<TKey>(key);
    }

    virtual void Set(const TKey &key, const TValue &value) {
        DataNode *pointer = find_value(key);
        if (pointer != nullptr)
            pointer->val = value;
        else {
            auto *new_node = new DataNode(key, value);
            new_node->next = root;
            root = new_node;
        }
    }

    virtual bool IsSet(const TKey &key) const {
        return find_value(key) != nullptr;
    }
};

#endif //DICTIONARY_MY_DICTIONARY_H
