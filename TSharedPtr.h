//
// Created by akvasha on 06.03.19.
//

#ifndef YT_TSHAREDPTR_H
#define YT_TSHAREDPTR_H

template <class T>
class TSharedPtr {
private:
    int *refCounter;
    T *Storage;

public:
    TSharedPtr () : refCounter(nullptr), Storage(nullptr) {}

    explicit TSharedPtr(T *p) : refCounter(new int(1)), Storage(p) {}

    TSharedPtr(const TSharedPtr &p) : refCounter(nullptr), Storage(nullptr) {
        if (p.isValid()) {
            refCounter = p.refCounter;
            Storage = p.Storage;
            (*refCounter)++;
        }
    }

    void free() {
        /*if (isValid()) {
            (*refCounter)--;
            if (*refCounter == 0) {
                delete refCounter;
                delete Storage;
            }
            refCounter = nullptr;
            Storage = nullptr;
        }*/
        if (Storage && --(*refCounter) == 0) {
            delete Storage;
            delete refCounter;
        } else if (!Storage) {
            delete refCounter;
        }
        Storage = nullptr;
        refCounter = nullptr;
    }

    ~TSharedPtr() {
        free();
    }

    bool isValid() const {
        return refCounter != nullptr;
    }

    int use_count() const {
        if (isValid()) {
            return *refCounter;
        } else {
            return 0;
        }
    }

    TSharedPtr &operator=(const TSharedPtr &p) {
        free();
        Storage = p.Storage;
        refCounter = p.refCounter;
        if (isValid()) {
            (*refCounter)++;
        }
        return *this;
    }

    T *operator->() const {
        return Storage;
    };

    T &operator*() const {
        return *Storage;
    };
};

template <class T, class... Targs>
TSharedPtr<T> MakeShared(Targs&&... targs) {
    return TSharedPtr<T>(new T(std::forward<Targs>(targs)...));
}

#endif //YT_TSHAREDPTR_H
