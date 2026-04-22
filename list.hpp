#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T *val;
        node *prev;
        node *next;
        node(T *v = nullptr) : val(v), prev(nullptr), next(nullptr) {}
        ~node() { if (val) { delete val; val = nullptr; } }
    };

protected:
    node *head;
    node *tail;
    size_t len;

    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        return cur;
    }
    node *erase(node *pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
    private:
        list<T> *owner;
        node *p;
        friend class list<T>;
        friend class const_iterator;
    public:
        iterator(list<T> *o = nullptr, node *np = nullptr) : owner(o), p(np) {}
        iterator operator++(int) {
            if (!owner || !p || p == owner->tail) throw invalid_iterator();
            iterator tmp = *this;
            p = p->next;
            return tmp;
        }
        iterator & operator++() {
            if (!owner || !p || p == owner->tail) throw invalid_iterator();
            p = p->next;
            return *this;
        }
        iterator operator--(int) {
            if (!owner || !p) throw invalid_iterator();
            iterator tmp = *this;
            if (p == owner->tail) {
                if (owner->len == 0) throw invalid_iterator();
                p = owner->tail->prev;
            } else {
                if (p->prev == owner->head) throw invalid_iterator();
                p = p->prev;
            }
            return tmp;
        }
        iterator & operator--() {
            if (!owner || !p) throw invalid_iterator();
            if (p == owner->tail) {
                if (owner->len == 0) throw invalid_iterator();
                p = owner->tail->prev;
            } else {
                if (p->prev == owner->head) throw invalid_iterator();
                p = p->prev;
            }
            return *this;
        }
        T & operator *() const {
            if (!owner || !p || p == owner->tail || p == owner->head) throw invalid_iterator();
            return *(p->val);
        }
        T * operator ->() const {
            if (!owner || !p || p == owner->tail || p == owner->head) throw invalid_iterator();
            return p->val;
        }
        bool operator==(const iterator &rhs) const { return owner == rhs.owner && p == rhs.p; }
        bool operator==(const const_iterator &rhs) const { return owner == rhs.owner && p == rhs.p; }
        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
    };

    class const_iterator {
    private:
        const list<T> *owner;
        node *p;
        friend class list<T>;
    public:
        const_iterator(const list<T> *o = nullptr, node *np = nullptr) : owner(o), p(np) {}
        const_iterator(const iterator &it) : owner(it.owner), p(it.p) {}
        const_iterator operator++(int) {
            if (!owner || !p || p == owner->tail) throw invalid_iterator();
            const_iterator tmp = *this;
            p = p->next;
            return tmp;
        }
        const_iterator & operator++() {
            if (!owner || !p || p == owner->tail) throw invalid_iterator();
            p = p->next;
            return *this;
        }
        const_iterator operator--(int) {
            if (!owner || !p) throw invalid_iterator();
            const_iterator tmp = *this;
            if (p == owner->tail) {
                if (owner->len == 0) throw invalid_iterator();
                p = owner->tail->prev;
            } else {
                if (p->prev == owner->head) throw invalid_iterator();
                p = p->prev;
            }
            return tmp;
        }
        const_iterator & operator--() {
            if (!owner || !p) throw invalid_iterator();
            if (p == owner->tail) {
                if (owner->len == 0) throw invalid_iterator();
                p = owner->tail->prev;
            } else {
                if (p->prev == owner->head) throw invalid_iterator();
                p = p->prev;
            }
            return *this;
        }
        const T & operator *() const {
            if (!owner || !p || p == owner->tail || p == owner->head) throw invalid_iterator();
            return *(p->val);
        }
        const T * operator ->() const {
            if (!owner || !p || p == owner->tail || p == owner->head) throw invalid_iterator();
            return p->val;
        }
        bool operator==(const const_iterator &rhs) const { return owner == rhs.owner && p == rhs.p; }
        bool operator==(const iterator &rhs) const { return owner == rhs.owner && p == rhs.p; }
        bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
    };


    list() : head(new node()), tail(new node()), len(0) {
        head->next = tail; head->prev = nullptr;
        tail->prev = head; tail->next = nullptr;
    }
    list(const list &other) : list() {
        for (node *cur = other.head->next; cur != other.tail; cur = cur->next) {
            push_back(*(cur->val));
        }
    }
    virtual ~list() { clear(); delete head; delete tail; }
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (node *cur = other.head->next; cur != other.tail; cur = cur->next) {
            push_back(*(cur->val));
        }
        return *this;
    }
    const T & front() const {
        if (len == 0) throw container_is_empty();
        return *(head->next->val);
    }
    const T & back() const {
        if (len == 0) throw container_is_empty();
        return *(tail->prev->val);
    }
    iterator begin() { return iterator(this, head->next); }
    const_iterator cbegin() const { return const_iterator(this, head->next); }
    iterator end() { return iterator(this, tail); }
    const_iterator cend() const { return const_iterator(this, tail); }
    virtual bool empty() const { return len == 0; }
    virtual size_t size() const { return len; }

    virtual void clear() {
        node *cur = head->next;
        while (cur != tail) {
            node *nx = cur->next;
            erase(cur);
            delete cur;
            cur = nx;
        }
        head->next = tail; tail->prev = head; len = 0;
    }
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.owner != this || !pos.p) throw invalid_iterator();
        node *n = new node(new T(value));
        insert(pos.p, n);
        ++len;
        return iterator(this, n);
    }
    virtual iterator erase(iterator pos) {
        if (pos.owner != this || !pos.p || pos.p == tail) throw invalid_iterator();
        node *nxt = pos.p->next;
        node *removed = erase(pos.p);
        delete removed;
        --len;
        return iterator(this, nxt);
    }
    void push_back(const T &value) { insert(end(), value); }
    void pop_back() {
        if (len == 0) throw container_is_empty();
        iterator it = end();
        --it;
        erase(it);
    }
    void push_front(const T &value) { insert(begin(), value); }
    void pop_front() {
        if (len == 0) throw container_is_empty();
        erase(begin());
    }
    void sort() {
        if (len <= 1) return;
        node **arr = new node*[len];
        size_t idx = 0;
        for (node *cur = head->next; cur != tail; cur = cur->next) arr[idx++] = cur;
        sjtu::sort<node*>(arr, arr + len, [](node* const &a, node* const &b){ return *(a->val) < *(b->val); });
        // rebuild links according to sorted order
        head->next = arr[0];
        arr[0]->prev = head;
        for (size_t i = 0; i + 1 < len; ++i) {
            arr[i]->next = arr[i+1];
            arr[i+1]->prev = arr[i];
        }
        arr[len-1]->next = tail;
        tail->prev = arr[len-1];
        delete [] arr;
    }
    void merge(list &other) {
        if (this == &other || other.len == 0) return;
        node *a = head->next;
        node *b = other.head->next;
        node *cur = head;
        while (a != tail && b != other.tail) {
            if (*(b->val) < *(a->val)) {
                node *nb = b->next;
                // splice b after cur
                b->prev->next = nb;
                nb->prev = b->prev;
                b->prev = cur;
                b->next = cur->next;
                cur->next->prev = b;
                cur->next = b;
                cur = b;
                b = nb;
            } else {
                cur = a;
                a = a->next;
            }
        }
        // append remaining b's after cur
        while (b != other.tail) {
            node *nb = b->next;
            b->prev->next = nb;
            nb->prev = b->prev;
            b->prev = cur;
            b->next = cur->next;
            cur->next->prev = b;
            cur->next = b;
            cur = b;
            b = nb;
        }
        other.head->next = other.tail;
        other.tail->prev = other.head;
        len += other.len;
        other.len = 0;
    }
    void reverse() {
        if (len <= 1) return;
        node *cur = head;
        while (cur) { node *tmp = cur->next; cur->next = cur->prev; cur->prev = tmp; cur = tmp; }
        node *tmp = head; head = tail; tail = tmp;
        head->prev = nullptr;
        tail->next = nullptr;
    }
    void unique() {
        if (len <= 1) return;
        node *cur = head->next;
        while (cur != tail) {
            node *nx = cur->next;
            while (nx != tail && *(cur->val) == *(nx->val)) {
                node *nn = nx->next;
                erase(nx);
                delete nx;
                --len;
                nx = nn;
            }
            cur = nx;
        }
    }
};

}

#endif //SJTU_LIST_HPP
