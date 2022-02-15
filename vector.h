#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <stdexcept>
#include <initializer_list>
#include <string>

template<typename E>
class Vector {
public:
    class Iterator;

    class ConstIterator;

    using value_type = E;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = Vector::Iterator;
    using const_iterator = Vector::ConstIterator;

private:
    static constexpr size_type LOAD_CONSTANT{ 5 };
    static constexpr double LOAD_FACTOR{ 1.618 };
    size_type sz;
    size_type max_sz;
    pointer values;

    inline void grow() {
        reserve(max_sz * LOAD_FACTOR + LOAD_CONSTANT);
    }

public:
    ~Vector() {
        delete[] values;
    }

    Vector() : sz{ 0 }, max_sz{ 0 }, values{ nullptr } {}

    explicit Vector(size_type n) : sz{ 0 }, max_sz{ n }, values{ n <= 0 ? nullptr : new value_type[n] } {}

    Vector(std::initializer_list<value_type> l) : Vector(l.size()) {
        for (const auto& v : l) {
            values[sz++] = v;
        }
    }

    Vector(const Vector& v) : Vector(v.max_sz) {
        for (; sz < v.sz; ++sz) {
            values[sz] = v.values[sz];
        }
    }

    Vector& operator=(const Vector& v) {
        if (v.sz > max_sz) {
            delete[] values;
            values = new value_type[v.max_sz];
            max_sz = v.max_sz;
        }

        sz = 0;
        for (; sz < v.sz; ++sz) {
            values[sz] = v.values[sz];
        }

        return *this;
    }

    [[nodiscard]] size_type size() const {
        return sz;
    }

    [[nodiscard]] bool empty() const {
        return sz <= 0;
    }

    void clear() {
        sz = 0;
    }

    void reserve(size_type n) {
        if (max_sz >= n) return;

        pointer buf{ new value_type[n] };
        for (size_type i{ 0 }; i < sz; ++i) {
            buf[i] = values[i];
        }

        delete[] values;
        values = buf;
        max_sz = n;
    }

    void shrink_to_fit() {
        if (max_sz <= sz) return;

        pointer buf{ new value_type[sz] };
        for (size_type i{ 0 }; i < sz; ++i) {
            buf[i] = values[i];
        }

        delete[] values;
        values = buf;
        max_sz = sz;
    }

    void push_back(const_reference val) {
        if (sz >= max_sz) {
            grow();
        }

        values[sz++] = val;
    }

    void pop_back() {
        if (sz <= 0) {
            throw std::out_of_range("pop_back() was called on an empty Vector");
        }

        --sz;
    }

    reference operator[](size_type index) {
        if (index >= sz || index < 0) {
            throw std::out_of_range("index " + std::to_string(index) + " out of range for size " + std::to_string(sz));
        }

        return values[index];
    }

    const_reference operator[](size_type index) const {
        if (index >= sz || index < 0) {
            throw std::out_of_range("index " + std::to_string(index) + " out of range for size " + std::to_string(sz));
        }

        return values[index];
    }

    [[nodiscard]] size_type capacity() const {
        return max_sz;
    }

    [[nodiscard]] iterator begin() {
        return iterator{ values };
    }

    [[nodiscard]] iterator end() {
        return iterator{ values + sz };
    }

    [[nodiscard]] const_iterator begin() const {
        return const_iterator{ values };
    }

    [[nodiscard]] const_iterator end() const {
        return const_iterator{ values + sz };
    }

    iterator insert(const_iterator pos, const_reference val) {
        auto diff = pos - begin();
        if (diff < 0) throw std::out_of_range("iterator out of range");
        size_type current{ static_cast<size_type>(diff) };
        if (current > sz) throw std::out_of_range("iterator out of range");

        if (sz >= max_sz) grow();

        for (size_type i{ sz }; i-- > current;) {
            values[i + 1] = values[i];
        }

        values[current] = val;
        ++sz;
        return iterator{ values + current };
    }

    iterator erase(const_iterator pos) {
        auto diff = pos - begin();
        if (diff < 0) throw std::out_of_range("iterator out of range");
        size_type current{ static_cast<size_type>(diff) };
        if (current >= sz) throw std::out_of_range("iterator out of range");


        for (size_type i{ current }; i < sz - 1; ++i) {
            values[i] = values[i + 1];
        }

        --sz;
        return iterator{ values + current };
    }

    std::ostream& print(std::ostream& o) const {
        o << '[';
        bool first = true;
        for (size_type i{ 0 }; i < sz; ++i) {
            if (!first) {
                o << ", ";
            } else {
                first = false;
            }

            o << values[i];
        }
        o << ']';

        return o;
    }

    class Iterator {
    public:
        using value_type = Vector::value_type;
        using reference = Vector::reference;
        using pointer = Vector::pointer;
        using difference_type = Vector::difference_type;
        using iterator_category = std::forward_iterator_tag;
    private:
        pointer ptr;
    public:
        Iterator() : ptr{ nullptr } {}

        explicit Iterator(pointer _ptr) : ptr{ _ptr } {}

        reference operator*() const {
            return *ptr;
        }

        pointer operator->() const {
            return ptr;
        }

        bool operator==(const const_iterator& rop) const {
            return static_cast<const_iterator>(*this) == rop;
        }

        bool operator!=(const const_iterator& rop) const {
            return static_cast<const_iterator>(*this) != rop;
        }

        iterator& operator++() {
            ++ptr;
            return *this;
        }

        iterator operator++(int) {
            return iterator{ ptr++ };
        }

        operator const_iterator() const {
            return const_iterator{ ptr };
        }
    };

    class ConstIterator {
    public:
        using value_type = Vector::value_type;
        using reference = Vector::const_reference;
        using pointer = Vector::const_pointer;
        using difference_type = Vector::difference_type;
        using iterator_category = std::forward_iterator_tag;
    private:
        pointer ptr;
    public:
        ConstIterator() : ptr{ nullptr } {}

        explicit ConstIterator(pointer _ptr) : ptr{ _ptr } {}

        reference operator*() const {
            return *ptr;
        }

        pointer operator->() const {
            return ptr;
        }

        bool operator==(const const_iterator& rop) const {
            return ptr == rop.ptr;
        }

        bool operator!=(const const_iterator& rop) const {
            return ptr != rop.ptr;
        }

        const_iterator& operator++() {
            ++ptr;
            return *this;
        }

        const_iterator operator++(int) {
            return const_iterator{ ptr++ };
        }

        Vector::difference_type operator-(const const_iterator& rop) const {
            return ptr - rop.ptr;
        }
    };
};

template<typename E>
std::ostream& operator<<(std::ostream& o, const Vector<E>& v) {
    return v.print(o);
}

#endif
