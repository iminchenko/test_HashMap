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
        T value;

        Item(int key_, T value_): key(key_), value(std::move(value_)) {}
    };

public:
    HashMap() :_size(0), _filled(0), _fixedSize(false) {};

    HashMap(const HashMap &hm) {
        *this = hm;
    }

    HashMap(HashMap &&hm) {
        *this = hm;
    }

    HashMap &operator=(const HashMap &hm) {
        if (this == &hm) {
            return *this;
        }

        std::lock_guard guard(hm._mutex);

        _size = hm._size;
        _filled = hm._filled;
        _fixedSize = hm._fixedSize;
        _data = hm._data;
        _deleted = hm._deleted;

        return *this;
    }

    HashMap &operator=(HashMap &&hm) noexcept {
        std::lock_guard guard(hm._mutex);

        _size = hm._size;
        hm._size = 0;

        _filled = hm._filled;
        hm._filled = 0;

        _fixedSize = hm._fixedSize;
        hm._fixedSize = false;

        _data = std::move(hm._data);
        _deleted = std::move(hm._deleted);

        return *this;
    }

    const T &operator[](int key) const {
        size_t id = find(key);

        if (id == NO_ID || !_data[id].has_value()) {
            throw std::logic_error("Not valid key");
        }

        return _data[id]->value;
    }

    T &operator[](int key) {
        size_t id = find(key);

        if (id == NO_ID || !_data[id].has_value()) {
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

    void erase(int key) {
        size_t id = find(key);

        if (id == NO_ID || !_data[id].has_value()) {
            throw std::logic_error("Not valid key");
        }

        _deleted[id] = true;
        _data[id].reset();

        --_size;
    }

    size_t size() const {
        return _size;
    }

    bool empty() const {
        return _size == 0;
    }

    bool contains(int key) {
        size_t id = find(key);

        return id != NO_ID && _data[id].has_value();
    }

private:
    HashMap(size_t size): _size(0), _filled(0), _data(size), _deleted(size), _fixedSize(false) {}

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

            if (!_deleted[h] && (!item.has_value() || item->key == key)) {
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
        if (_fixedSize) {
            return;
        }

        size_t newSize;

        if (_data.size() == 0) {
            newSize = 1;
        } else if (_size < _data.size() / 4) {
            newSize = std::min(_data.size() / 2, size_t(1));
        } else if (_filled == _data.size()) {
            newSize = _data.size() * 2;
        } else {
            return;
        }

        HashMap<T> newMap(newSize);

        for (size_t i = 0; i < _data.size(); ++i) {
            if (!_deleted[i]) {
                newMap.insert(_data[i]->key, _data[i]->value);
            }
        }

        *this = std::move(newMap);

        _fixedSize = false;
    }

    std::vector<std::optional<Item>> _data;
    std::vector<bool> _deleted;
    size_t _size;           // количество валидных элементов
    size_t _filled;         // количество заполненных элементов (включая deleted == true)
    std::mutex _mutex;
    bool _fixedSize;
};
