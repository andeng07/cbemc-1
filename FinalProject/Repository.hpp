#ifndef REPOSITORY_HPP
#define REPOSITORY_HPP

template<typename T, int MAX_ITEMS>
class Repository {
public:
    Repository() : size_(0) {}

    bool add(const T& item) {
        if (size_ >= MAX_ITEMS) return false;
        data[size_++] = item;
        return true;
    }

    T* get(int index) {
        if (index < 0 || index >= size_) return nullptr;
        return &data[index];
    }

    template<typename Predicate>
    T* find(Predicate pred) {
        for (int i = 0; i < size_; ++i) {
            if (pred(data[i])) return &data[i];
        }
        return nullptr;
    }

    template<typename Predicate>
    bool update_if(Predicate pred, const T& newItem) {
        T* item = find(pred);
        if (item != nullptr) {
            *item = newItem;
            return true;
        }
        return false;
    }

    bool remove(int index) {
        if (index < 0 || index >= size_) return false;
        for (int i = index; i < size_ - 1; ++i)
            data[i] = data[i + 1];
        --size_;
        return true;
    }

    template<typename Predicate>
    bool remove_if(Predicate pred) {
        for (int i = 0; i < size_; ++i) {
            if (pred(data[i])) {
                remove(i);
                return true;
            }
        }
        return false;
    }

    int size() const { return size_; }

private:
    T data[MAX_ITEMS];
    int size_;
};

#endif
