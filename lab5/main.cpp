#include <iostream>
#include <memory>
#include <algorithm>
#include <cmath>
#include <type_traits>

#define INIT_CAP 8

template<class T, class Alloc, class InputIt>
std::pair<T*, int> read_iter(InputIt sit, InputIt eit, Alloc alloc) {
    int capacity = INIT_CAP;
    int i = 0;
    T* buffer = std::allocator_traits<Alloc>::allocate(alloc, capacity);
    while (true) {
        for (; i < capacity; ++i) {
            if (sit == eit) {
                return std::make_pair(buffer, i);
            }
            buffer[i] = *(sit++);
        }
        T* new_buffer = std::allocator_traits<Alloc>::allocate(alloc, capacity*2);
        std::move(buffer, buffer+capacity, new_buffer);
        std::allocator_traits<Alloc>::deallocate(alloc, buffer, capacity);
        capacity*=2;
        buffer = new_buffer;
    }
}

template<class Cycle, bool IS_CONST>
class cycle_iter;

template<class T, class Alloc=std::allocator<T>>
class cycle {
public:
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = int;
    using difference_type = int;
    using refrence = T&;
    using const_reference = const T&;
    using pointer = T*;
    using iterator = cycle_iter<cycle<T, Alloc>, false>;
    using const_iterator = cycle_iter<const cycle<T, Alloc>, true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    Alloc get_allocator() const {
        return alloc_;
    }

    cycle(const Alloc& alloc = Alloc())
            : alloc_(alloc)
            , capacity_(INIT_CAP)
            , size_(0)
            , start_(0)
            , buffer_(allocate(capacity_)) {}

    cycle(int count, const T& value, const Alloc& alloc = Alloc())
            : alloc_(alloc)
            , capacity_(get_cap(count))
            , size_(count)
            , start_(0)
            , buffer_(allocate(capacity_)) {
        std::fill(buffer_, buffer_+count, value);
    }

    explicit cycle(int count, const Alloc& alloc = Alloc())
            : alloc_(alloc)
            , capacity_(get_cap(count))
            , size_(count)
            , start_(0)
            , buffer_(allocate(capacity_)) {
        for (int i = 0; i < size_; ++i) {
            std::allocator_traits<Alloc>::construct(alloc_, buffer_+i);
        }
    }

    template<class InputIt>
    cycle(InputIt sit, InputIt eit, const Alloc& alloc = Alloc())
            : cycle(alloc) {
        insert(begin(), sit, eit);
    }

    cycle(const cycle& other)
            : alloc_(std::allocator_traits<Alloc>::select_on_container_copy_construction(other.get_allocator()))
            , capacity_(other.capacity_)
            , size_(other.size_)
            , start_(other.start_)
            , buffer_(allocate(capacity_)) {
        std::copy(other.buffer_, other.buffer_+capacity_, buffer_);
    }

    cycle(const cycle& other, const Alloc& alloc)
            : alloc_(alloc)
            , capacity_(other.capacity_)
            , size_(other.size_)
            , start_(other.start_)
            , buffer_(allocate(capacity_)) {
        std::copy(other.buffer_, other.buffer_+capacity_, buffer_);
    }

    cycle(cycle&& other)
            : alloc_(std::move(other.alloc_))
            , capacity_(capacity_)
            , size_(other.size_)
            , start_(other.start_)
            , buffer_(allocate(capacity_)) {
        std::move(other.buffer_, other.buffer_+capacity_, buffer_);
    }

    cycle(cycle&& other, const Alloc& alloc = Alloc())
            : alloc_(alloc)
            , capacity_(other.capacity_)
            , size_(other.size_)
            , start_(other.start_)
            , buffer_(allocate(capacity_)) {
        std::move(other.buffer_, other.buffer_+capacity_, buffer_);
    }

    cycle(std::initializer_list<T> init, const Alloc& alloc = Alloc())
            : alloc_(alloc)
            , capacity_(get_cap(init.size()))
            , size_(init.size())
            , start_(0)
            , buffer_(allocate(capacity_)) {
        std::copy(init.begin(), init.end(), buffer_);
    }

    cycle& operator=(const cycle& other) {
        if (this == &other) {
            return *this;
        }
        deallocate(buffer_, capacity_);
        if (std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value) {
            alloc_ = other.alloc_;
        }
        capacity_ = other.capacity_;
        size_ = other.size_;
        start_ = other.start_;
        buffer_ = allocate(capacity_);
        std::copy(other.buffer_, other.buffer_+capacity_, buffer_);
        return *this;
    }

    cycle& operator=(cycle&& other) {
        if (this == &other) {
            return *this;
        }
        deallocate(buffer_, capacity_);
        if (std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value) {
            alloc_ = other.alloc_;
        }
        capacity_ = other.capacity_;
        size_ = other.size_;
        start_ = other.start_;
        buffer_ = allocate(capacity_);
        std::move(other.buffer_, other.buffer_+capacity_, buffer_);
        return *this;
    }

    cycle& operator=(std::initializer_list<T> init) {
        allocate(buffer_);
        capacity_ = get_cap(init.size());
        size_ = init.size();
        start_ = 0;
        buffer_ = allocate(capacity_);
        std::copy(init.begin(), init.end(), buffer_);
        return *this;
    }

    ~cycle() {
        clear();
        deallocate(buffer_, capacity_);
    }

    T& operator[](int i) {
        return buffer_[index(i)];
    }

    const T& operator[](int i) const {
        return buffer_[index(i)];
    }

    T& at(int i) {
        if (0 <= i && i < size_) {
            return (*this)[i];
        }
        throw std::out_of_range("out of range");
    }

    const T& at(int i) const {
        if (0 <= i && i < size_) {
            return (*this)[i];
        }
        throw std::out_of_range("out of range");
    }

    T& front() {
        return buffer_[start_];
    }

    const T& front() const {
        return buffer_[start_];
    }

    T& back() {
        return buffer_[index(size_-1)];
    }

    const T& back() const {
        return buffer_[index(size_-1)];
    }

    iterator begin() {
        return iter(0);
    }

    const_iterator begin() const {
        return const_iter(0);
    }

    const_iterator cbegin() const {
        return begin();
    }

    iterator end() {
        return iter(size_);
    }

    const_iterator end() const {
        return const_iter(size_);
    }

    const_iterator cend() const {
        return end();
    }

    reverse_iterator rbegin() {
        return std::make_reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return std::make_reverse_iterator(cend());
    }

    const_reverse_iterator crbegin() const {
        return rbegin();
    }

    reverse_iterator rend() {
        return std::make_reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return std::make_reverse_iterator(cbegin());
    }

    const_reverse_iterator crend() const {
        return rend();
    }

    bool empty() const {
        return size_!=0;
    }

    int size() const {
        return size_;
    }

    int max_size() const {
        return std::numeric_limits<int>::max();
    }

    void reserve(int size) {
        int new_cap = get_cap(size);
        if (new_cap > capacity_) {
            realloc(new_cap);
        }
    }

    int capacity() const {
        return capacity_;
    }

    void shrink_to_fit() {
        int new_cap = get_cap(size_);
        if (new_cap < get_cap(size_)) {
            realloc(new_cap);
        }
    }

    void clear() {
        for (int i = 0; i < size_; ++i) {
            std::allocator_traits<Alloc>::destroy(alloc_, &buffer_[index(i)]);
        }
        size_ = 0;
    }

    iterator insert(const_iterator pos, int count, const T& value) {
        iterator it = iter(pos.i_);
        insert_prep(it, count);
        std::fill(it, it + count, value);
        size_ += count;
        return it;
    }

    iterator insert(const_iterator pos, const T& value) {
        return insert(pos, 1, value);
    }

    iterator insert(const_iterator pos, T&& value) {
        iterator it = iter(pos.i_);
        insert_prep(it, 1);
        *it = std::move(value);
        size_++;
        return it;
    }

    template<class InputIt>
    iterator insert(const_iterator pos, InputIt sit, InputIt eit) {
        iterator it = iter(pos.i_);
        auto [minibuf, count, minicap] = read_iter(sit, eit, alloc_);
        insert_prep(it, count);
        std::move(minibuf, minibuf+count, it);
        deallocate(minibuf, minicap);
        size_ += count;
        return it;
    }

    iterator insert(const_iterator pos, std::initializer_list<T> init) {
        iterator it = iter(pos.i_);
        insert_prep(it, init.size());
        std::copy(init.begin(), init.end(), it);
        size_ += init.size();
        return it;
    }

    template<class... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        iterator it = iter(pos.i_);
        insert_prep(it, 1);
        std::allocator_traits<Alloc>::construct(alloc_, &(*it), std::forward<Args>(args)...);
        size_++;
        return it;
    }

    iterator erase(const_iterator pos) {
        iterator it = iter(pos.i_);
        std::allocator_traits<Alloc>::destroy(alloc_, &buffer_[index(pos.i_)]);
        erase_post(it, 1);
        size_--;
        return it;
    }

    iterator erase(const_iterator sit, const_iterator eit) {
        iterator it = sit;
        for (int i = sit.i_; i < eit.i_; ++i) {
            std::allocator_traits<Alloc>::destroy(alloc_, &buffer_[index(i)]);
        }
        erase_post(it, eit-sit);
        size_ -= eit.i_ - sit.i_;
        return it;
    }

    void push_back(const T& value) {
        insert(end(), value);
    }

    void push_back(T&& value) {
        insert(end(), value);
    }

    template<class... Args>
    T& emplace_back(Args&&... args) {
        return *emplace(end(), std::forward<Args>(args)...);
    }

    void pop_back() {
        erase(end()-1);
    }

    void resize(int count) {
        if (count < size_) {
            erase(const_iter(count), end());
        } else {
            reserve(count);
            for (int i = size_; i < size_ + count; ++i) {
                std::allocator_traits<Alloc>::construct(alloc_, &buffer_[index(i)]);
            }
            size_ = count;
        }
    }

    void resize(int count, const T& value) {
        if (count < size_) {
            erase(const_iter(count), end());
        } else {
            insert(end(), count, value);
        }
    }

    void swap(cycle& other) {
        if (std::allocator_traits<Alloc>::propagate_on_container_swap::value) {
            std::swap(alloc_, other.alloc_);
        }
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        std::swap(start_, other.start_);
        std::swap(buffer_, other.buffer_);
    }

protected:
    Alloc alloc_;
    int capacity_;
    int size_;
    int start_;
    T* buffer_;

    T* allocate(int n) {
        return std::allocator_traits<Alloc>::allocate(alloc_, n);
    }

    void deallocate(T* buf, int n) {
        std::allocator_traits<Alloc>::deallocate(alloc_, buf, n);
    }

    static int get_cap(int size) {
        return static_cast<int>(std::ceil(std::pow(2, std::ceil(std::log2(size)))));
    }

    int index(int i) const {
        return (capacity_ + (start_+i)%capacity_)%capacity_;
    }

    void realloc(int new_cap) {
        T* new_buffer = allocate(new_cap);
        std::move(begin(), end(), new_buffer);
        deallocate(buffer_, capacity_);
        buffer_ = new_buffer;
        capacity_ = new_cap;
        start_ = 0;
    }

    iterator iter(int i) {
        return iterator(*this, i);
    }

    const_iterator const_iter(int i) const {
        return const_iterator(*this, i);
    }

    void insert_prep(iterator it, int count) {
        reserve(size_+count);
        if (it.i_ == 0 && size_ != 0) {
            start_ = index(-count);
        } else {
            std::move_backward(it, end(), end() + count);
        }
    }

    void erase_post(iterator it, int count) {
        if (it.i_ == 0) {
            start_ = index(count);
        } else {
            std::move(it+count, end(), it);
        }
    }
};

template<class Cycle, bool IS_CONST>
class cycle_iter {
public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = int;
    using value_type = typename std::conditional<IS_CONST, const typename Cycle::value_type, typename Cycle::value_type>::type;
    using T = value_type;
    using pointer = T*;
    using reference = T&;

    Cycle& cyc_;
    int i_;

    cycle_iter(Cycle& cyc, int i)
            : cyc_(cyc)
            , i_(i) {}

    cycle_iter(const cycle_iter& it)
            : cyc_(it.cyc_)
            , i_(it.i_) {}
    cycle_iter& operator=(const cycle_iter& it) {
        cyc_ = it.cyc_;
        i_ = it.i_;
        return *this;
    }

    template<class PrevCycle, bool WAS_CONST, class = std::enable_if_t<IS_CONST && !WAS_CONST>>
    cycle_iter(const cycle_iter<PrevCycle, WAS_CONST>& it)
            : cyc_(it.cyc_)
            , i_(it.i_) {}

    template<class PrevCycle, bool WAS_CONST, class = std::enable_if_t<IS_CONST && !WAS_CONST>>
    cycle_iter& operator=(const cycle_iter<PrevCycle, WAS_CONST>& it) {
        cyc_ = it.cyc_;
        i_ = it.i_;
        return *this;
    }

    T& operator*() const {
        return cyc_[i_];
    }

    T* operator->() const {
        return &cyc_[i_];
    }

    cycle_iter& operator++() {
        i_++;
        return *this;
    }

    cycle_iter operator++(int) {
        cycle_iter tmp = *this;
        ++(*this);
        return tmp;
    }

    cycle_iter& operator+=(int n) {
        i_+=n;
        return *this;
    }

    friend cycle_iter operator+(cycle_iter it, int n) {
        it+=n;
        return it;
    }

    friend cycle_iter operator+(int n, const cycle_iter& it) {
        return it+n;
    }

    cycle_iter& operator--() {
        i_--;
        return *this;
    }

    cycle_iter operator--(int) {
        cycle_iter tmp = *this;
        --(*this);
        return tmp;
    }

    cycle_iter& operator-=(int n) {
        i_-=n;
        return *this;
    }

    friend cycle_iter operator-(cycle_iter it, int n) {
        it-=n;
        return it;
    }

    friend int operator-(const cycle_iter& a, const cycle_iter& b) {
        return a.i_ - b.i_;
    }

    T& operator[](int n) const {
        return cyc_[i_+n];
    }

    bool operator==(const cycle_iter& other) const {
        return i_ == other.i_;
    }

    bool operator!=(const cycle_iter& other) const {
        return !(*this == other);
    }

    friend bool operator<(const cycle_iter &a, const cycle_iter &b) {
        return a.i_ < b.i_;
    }

    friend bool operator>(const cycle_iter &a, const cycle_iter &b) {
        return b < a;
    }

    friend bool operator<=(const cycle_iter &a, const cycle_iter &b) {
        return !(b < a);
    }

    friend bool operator>=(const cycle_iter &a, const cycle_iter &b) {
        return !(a < b);
    }
};

int main() {
    cycle<int> a {4,6,8,8};
    a.insert(a.begin(), {2,4});
    a.push_back(10);

    for (int x : a) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;

    for(auto it = a.crbegin(); it != a.crend(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;

    std::cout << std::all_of(a.cbegin(), a.cend(), [](int x) {
        return x%2 == 0;
    }) << std::endl;

    std::cout << *(++std::find(a.begin(), a.end(), 6)) << std::endl;

    std::cout << *std::adjacent_find(a.begin(), a.end()) << std::endl;

    std::cout << std::max_element(a.begin(), a.end()) - a.begin() << std::endl;

    std::cout << std::binary_search(a.begin(), a.end(), 6) << std::endl;

    cycle<int>::iterator it = a.begin();
    cycle<int>::const_iterator cit = it;
//    cycle<int>::iterator it2 (a.cbegin()); // doesn't work
}
