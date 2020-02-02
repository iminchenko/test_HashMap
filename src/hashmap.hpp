#pragma once

#include <vector>
#include <optional>
#include <utility>
#include <mutex>
#include <stdexcept>

template <class T>
class HashMap {
    static constexpr size_t NO_ID = static_cast<size_t>(-1);
    static constexpr size_t HASH_INCREMENT = 3571;

    struct Item {
        int key;
        bool deleted;
        T value;

        Item(int key, T value)
        :key(key), deleted(false), value(std::move(value)) {}
    };

public:
    HashMap() :_size(0), _filled(0) {};

    HashMap(const HashMap &hm) {
        *this = hm;
    }

    HashMap(HashMap &&hm) {
        *this = hm;
    }

    HashMap &operator=(const HashMap &hm) {
        std::lock_guard guard(hm._mutex);

        _size = hm._size;
        _filled = hm._filled;
        _data = hm._data;

        return *this;
    }

    HashMap &operator=(HashMap &&hm) {
        std::lock_guard guard(hm._mutex);

        _size = hm._size;
        hm._size = 0;

        _filled = hm._filled;
        hm._filled = 0;

        _data = std::move(hm._data);

        return *this;
    }

    const T &operator[](int key) const {
        size_t id = find(key);

        if (id == NO_ID || _data[id]->key != key) {
            throw std::logic_error("Not valid key");
        }

        return _data[id]->value;
    }

    T &operator[](int key) {
        size_t id = find(key);

        if (id == NO_ID || _data[id]->key != key) {
            insert(key, T());
            id = find(key);
        }

        return _data[id]->value;
    }

    void insert(int key, const T &value) {
        std::lock_guard guard(_mutex);

        if (_filled == _data.size()) {
            resize();
        }

        size_t id = find(key);

        auto &item = _data[id];

        if (item.has_value()) {
            item->value = value;
        } else {
            item.emplace(key, value);
        }

        ++_size;
        ++_filled;
    }

    size_t size() const {
        return _size;
    }

    bool empty() const {
        return _size == 0;
    }

private:
    size_t hash(int key) const {
        return key % _data.size();
    }

    size_t nextHash(size_t key) const {
        return (key + HASH_INCREMENT) % _data.size();
    }

    // поиск id элемента с заданным ключом
    // если элемента с такми ключом key не найдено, возвращается первый
    // подходящий незанятый индекс
    // если подходящего индекса не найдено, возвращяется NO_ID
    size_t find(int key) const {
        if (_data.empty()) {
            return NO_ID;
        }

        // количество проверенных элементов
        size_t count = 0;

        size_t h = hash(key);

        do {
            const auto &item = _data[h];

            if (!item.has_value() || (!item->deleted && item->key == key)) {
                return h;
            }

            h = nextHash(h);

            ++count;
        } while (count < _data.size());

        // возможно только в случае заполнения всей хэш таблицы
        return NO_ID;
    }

    // изменение размера
    // при нулевом размере массива расширение до 1
    // если валидных элементов < 1/4, уменьшение размера в 2 раза
    // если map заполнен, увеличение в 2 раза
    void resize() {
        if (_data.size() == 0) {
            _data.resize(1);
            return;
        }

        HashMap<T> newMap;

        if (_size < _data.size() / 4) {
            newMap._data.resize(_data.size() / 4);
        } else if (_filled == _data.size()) {
            newMap._data.resize(_data.size() * 2);
        } else {
            return;
        }

        for (const auto &i : _data) {
            if (!i->deleted) {
                newMap.insert(i->key, i->value);
            }
        }

        _size = newMap._size;
        _filled = newMap._filled;
        _data = std::move(newMap._data);
    }

    std::vector<std::optional<Item>> _data;
    size_t _size;           // количество валидных элементов
    size_t _filled;         // количество заполненных элементов (включая deleted == true)
    std::mutex _mutex;
};
