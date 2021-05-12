#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <iterator>
#include <cmath>

#define INIT_CAP 8

template <class T, class Alloc=std::allocator<T>>
class cycle {
public:

    // constructors

    // default
    explicit cycle(const Alloc& alloc = Alloc())
            : alloc_(alloc)
            , start_(0)
            , size_(0)
            , capacity_(INIT_CAP)
            , buffer_(alloc_.allocate(capacity_)) {}

    // fill
    cycle(size_t count, const T& value, const Alloc& alloc = Alloc())
            : alloc_(alloc)
            , start_(0)
            , size_(count)
            , capacity_(get_cap(size_))
            , buffer_(alloc_.allocate(capacity_)) {
        for (int i = 0; i < count; ++i) {
            buffer_[i] = value;
        }
    }

    // size
    explicit cycle(size_t count, const Alloc& alloc = Alloc())
            : cycle(count, T(), alloc) {} // todo default insertable shit

    // iterator
    template<class InputIt>
    cycle(InputIt sit, InputIt eit, const Alloc& alloc = Alloc())
            : cycle(alloc) {
        for (InputIt it = sit; it != eit; ++it) {
            // todo
        }
    }

    // copy with same allocator
    cycle(const cycle& other)
            : alloc_(std::allocator_traits<Alloc>::select_on_container_copy_construction(other.alloc_))
            , start_(other.start_)
            , size_(other.size_)
            , capacity_(other.capacity_)
            , buffer_(alloc_.allocate(capacity_)) {
        std::copy(other.buffer_, other.buffer_+size_, buffer_);
    }

    // copy
    cycle(const cycle& other, const Alloc& alloc)
            : alloc_(alloc)
            , start_(other.start_)
            , size_(other.size_)
            , capacity_(other.capacity_)
            , buffer_(alloc_.allocate(capacity_)) {
        std::copy(other.buffer_, other.buffer_+size_, buffer_);
    }

    // todo move constructor

    // initializer list
    cycle(std::initializer_list<T> ilist, const Alloc& alloc = Alloc())
            : alloc_(alloc)
            , start_(0)
            , size_(ilist.size())
            , capacity_(get_cap(size_))
            , buffer_(alloc.allocate(capacity_)) {
        std::copy(ilist.begin(), ilist.end(), buffer_); // todo maybe this should be move?
    }

    // destructor

    ~cycle() {
        alloc_.deallocate(buffer_);
    }

    // assignment

    // copy
    cycle& operator=(const cycle& other) {
        if (this == &other) {
            return *this;
        }
        alloc_.deallocate(buffer_);
        bool propagate = std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value;
        if (propagate) {
            alloc_ = other.alloc_;
        }
        start_ = other.start_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        buffer_ = alloc_.allocate(capacity_);
        std::copy(other.buffer_, other.buffer_+size_, buffer_);
        return *this;
    }

    // todo move assignment

    // initializer list
    cycle& operator=(std::initializer_list<T> ilist) {
        alloc_.deallocate(buffer_);
        start_ = 0;
        size_ = ilist.size();
        capacity_ = get_cap(size_);
        buffer_ = alloc_.allocate(capacity_);
        std::copy(ilist.begin(), ilist.end(), buffer_); // todo maybe this should be move?
        return *this;
    }

    // todo maybe assign function

    // direct element access

    T& operator[](size_t pos) {
        return buffer_[get_index(pos)];
    }

    const T& operator[](size_t pos) const {
        return buffer_[get_index(pos)];
    }

    T& at(size_t pos) {
        if (pos < size_) {
            return (*this)[pos];
        }
        throw std::out_of_range("index out of range");
    }

    const T& at(size_t pos) const {
        if (pos < size_) {
            return (*this)[pos];
        }
        throw std::out_of_range("index out of range");
    }

    T& front() {
        return buffer_[start_];
    }

    const T& front() const {
        return buffer_[start_];
    }

    T& back() {
        return buffer_[get_index(size_-1)];
    }

    const T& back() const {
        return buffer_[get_index(size_-1)];
    }

    // iterators

    struct Iter { // todo totally wrong, need checks for same iter and some other stuff, probably should rewrite entirely
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = long long; // todo maybe this is wrong?
        using value_type = T;
        using pointer = T*;
        using reference = T&;
    public:

        friend void swap(Iter& iter1, Iter& iter2) {
            size_t index1 = iter1.pos_;
            iter1.pos_ = iter2.pos_;
            iter2.pos_ = index1;
        }

        reference operator*() {
            return cyc_[pos_];
        }

        Iter& operator+=(difference_type n) {
            pos_ += n;
            return *this;
        }

        Iter& operator++() {
            pos_++;
            return *this;
        }

        Iter operator++(int) {
            Iter tmp = *this;
            ++(*this);
            return tmp;
        }

        friend Iter operator+(Iter iter, difference_type n) {
            iter+=n;
            return iter;
        }

        friend Iter operator+(difference_type n, const Iter& iter) {
            return iter+n;
        }

        Iter& operator-=(difference_type n) {
            pos_ -= n;
            return *this;
        }

        Iter& operator--() {
            pos_--;
            return *this;
        }

        Iter operator--(int) {
            Iter tmp = *this;
            --(*this);
            return tmp;
        }

        friend Iter operator-(Iter iter, difference_type n) {
            iter-=n;
            return iter;
        }

        difference_type operator-(const Iter& other) const {
            return pos_ - other.pos_;
        }

        reference operator[](difference_type n) const {
            return *((*this) + n);
        }


        bool operator<(const Iter &other) const {
            return pos_ < other.pos_;
        }

        bool operator>(const Iter &other) const {
            return other < *this;
        }

        bool operator<=(const Iter &other) const {
            return !(other < *this);
        }

        bool operator>=(const Iter &other) const {
            return !(*this < other);
        }

        bool operator==(const Iter& other) const {
            return pos_ == other.pos_;
        }

        bool operator!=(const Iter& other) const {
            return !((*this) == other);
        }

        pointer operator->() {
            return cyc_.pointer(pos_);
        }

        size_t pos() const {
            return pos_;
        }

        Iter(cycle& cyc, size_t pos)
        : cyc_(cyc)
        , pos_(pos) {}

    private:
        cycle& cyc_;
        size_t pos_;
    };

    Iter begin() {
        return Iter(*this, 0);
    }

    Iter end() {
        return Iter(*this, size_);
    }

    // size stuff

    bool empty() const {
        return size_ == 0;
    }

    size_t size() const {
        return size_;
    }

    static size_t max_size() {
        return std::numeric_limits<size_t>::max();
    }

    void reserve(size_t size) {
        size_t new_cap = get_cap(size);
        if (new_cap > capacity_) {
            realloc(new_cap);
        }
    }

    size_t capacity() const {
        return capacity_;
    }

    void shrink_to_fit() {
        size_t new_cap = get_cap(size_);
        if (new_cap < capacity_) {
            realloc(new_cap);
        }
    }

    // modification

    void clear() {
        size_ = 0;
    }

    Iter insert(Iter pos, const T& value) {
        if (size_ == capacity_) {
            realloc(capacity_*2);
        }
        if (pos == end()) {
            buffer_[get_index(size_)] = value;
        } else if (pos == begin()) {
            buffer_[get_index(-1)] = value;
            if (start_ == 0) {
                start_ = capacity_-1;
            } else {
                start_--;
            }
        } else {
            for (int i = size_-1; i >= pos.pos(); --i) {
                buffer_[get_index(i+1)] = buffer_[get_index(i)];
            }
            buffer_[pos.pos()] = value;
        }
        size_++;
    }

    // misc

    Alloc get_allocator() const {
        return alloc_;
    }

protected:

    static size_t get_cap(size_t size) {
        int pow = std::ceil(std::log2(size));
        return (size_t) std::ceil(std::pow(2, pow));
        // todo optimize maybe
    }

    size_t get_index(size_t pos) {
        return ((start_+pos)%capacity_ + capacity_)%capacity_;
    }

    T* pointer(size_t pos) {
        return buffer_ + get_index(pos);
    }

    void realloc(size_t cap) {
        T *new_buffer = alloc_.allocate(cap);
        for (int i = 0; i < size_; ++i) {
            new_buffer[i] = (*this)[i];
        }
        alloc_.deallocate(buffer_);
        buffer_ = new_buffer;
        capacity_ = cap;
        start_ = 0;
    }

    Alloc alloc_;
    size_t start_;
    size_t size_;
    size_t capacity_;
    T *buffer_;
};

int main() {

}
