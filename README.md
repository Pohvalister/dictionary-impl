# Dictionary implementation

# Задание:
Необходимо реализовать класс контейнера, реализующий интерфейс Dictionary и способный выбросить исключение, реализующее интерфейс NotFoundException.
При разработке допускается использовать STL. Кроме этого, внутренняя реализация ничем не ограничена.

```
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
```
---
# Релизация:
Для интерфейса были реализованы 3 класса, каждый со своими особенностями:
* TreeDictionary - класс, хранящий данные в структуре AVL-tree, для его работы необходимо, чтобы тип TKey имел овозможность сравниваться с собой на операторы "<" и "=="
* HashDictionary - класс, хранящий данные в hash-таблице, для его работы необходимы специализация std::hash<TKey> и возможность сравнения Tkey оператором "=="
* ListDictionary - медленный класс, который хранит данные в структуре связанного списка. Однако для его работы от Tkey требуется только операция сравнения "=="
