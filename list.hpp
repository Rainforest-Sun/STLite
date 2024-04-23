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
        /**
         * add data members and constructors & destructor
         */
        T *data;
        node *prev, *next;
        node(){
            data = nullptr;
            prev = nullptr;
            next = nullptr;
        }
        node(const T& value){
            data = new T(value);
            prev = nullptr;
            next = nullptr;
        }
        node(const node& nodes){
            data = new T(*(nodes.data));
            prev = nodes.prev;
            next = nodes.next;
        }
        ~node(){
            if (data) delete data;
        }
    };

protected:
    /**
     * add data members for linked list as protected members
     */
    node *head, *tail;
    size_t cur_len;
    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        node *tmp = pos->prev;
        cur->prev = tmp;
        cur->next = pos;
        tmp->next = cur;
        pos->prev = cur;
        ++cur_len;
        return cur;
    }
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        if (!cur_len) throw container_is_empty();
        node *p = pos->prev;
        node *n = pos->next;
        p->next = n;
        n->prev = p;
        pos->prev = nullptr;
        pos->next = nullptr;
        --cur_len;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
    public:
        node *pnode;
        list *lis;
        iterator(){}
        iterator(node *_pnode,list* _lis):pnode(_pnode),lis(_lis){}
        iterator(const iterator& other):pnode(other.pnode),lis(other.lis){}
        /**
         * iter++
         */
        iterator operator++(int) {
            if (!pnode->next)
                throw invalid_iterator();
            iterator tmp = *this;
            pnode = pnode->next;
            return tmp;
        }
        /**
         * ++iter
         */
        iterator & operator++() {
            if (!pnode->next)
                throw invalid_iterator();
            pnode = pnode->next;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int) {
            if (!pnode->prev->prev)
                throw invalid_iterator();
            iterator tmp = *this;
            pnode = pnode->prev;
            return tmp;
        }
        /**
         * --iter
         */
        iterator & operator--() {
            if (!pnode->prev->prev)
                throw invalid_iterator();
            pnode = pnode->prev;
            return *this;
        }
        /**
         * TODO *it
         * remember to throw if iterator is invalid
         */
        T & operator *() const {
            if (pnode==nullptr || !pnode->prev || !pnode->next)
                throw invalid_iterator();
            return *(pnode->data);
        }
        /**
         * TODO it->field
         * remember to throw if iterator is invalid
         */
        T *operator ->() const {
            if (pnode==nullptr || !pnode->prev || !pnode->next)
                throw invalid_iterator();
            return pnode->data;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const {
            if ((lis==rhs.lis) && (pnode==rhs.pnode)) return 1;
            return 0;
        }
        bool operator==(const const_iterator &rhs) const {
            if ((lis==rhs.lis) && (pnode==rhs.pnode)) return 1;
            return 0;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     * should be able to construct from an iterator.
     */
    class const_iterator {
        public:
        node* pnode;
        const list *lis;
        const_iterator(){}
        const_iterator(node *_pnode,const list*_lis):pnode(_pnode),lis(_lis){}
        const_iterator(const iterator & other){
            pnode = other.pnode;
            lis = other.lis;
        }
        const_iterator operator++(int) {
            if (!pnode->next)
                throw invalid_iterator();
            const_iterator tmp = *this;
            pnode = pnode->next;
            return tmp;
        }
        const_iterator & operator++() {
            if (!pnode->next)
                throw invalid_iterator();
            pnode = pnode->next;
            return *this;
        }
        const_iterator operator--(int) {
            if (!pnode->prev->prev)
                throw invalid_iterator();
            const_iterator tmp = *this;
            pnode = pnode->prev;
            return tmp;
        }
        const_iterator & operator--() {
            if (!pnode->prev->prev)
                throw invalid_iterator();
            pnode = pnode->prev;
            return *this;
        }
        const T & operator *() const {
            if (pnode==nullptr || !pnode->prev || !pnode->next)
                throw invalid_iterator();
            return *(pnode->data);
        }
        const T * operator ->() const {
            if (pnode==nullptr || !pnode->prev || !pnode->next)
                throw invalid_iterator();
            return *(pnode->data);
        }
        bool operator==(const iterator &rhs) const {
            if ((lis==rhs.lis) && (pnode==rhs.pnode)) return 1;
            return 0;
        }
        bool operator==(const const_iterator &rhs) const {
            if ((lis==rhs.lis) && (pnode==rhs.pnode)) return 1;
            return 0;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };
    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    list() {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
        cur_len = 0;
    }
    list(const list &other) {
        head = new node();
        tail = new node();
        node *p = other.head;
        node *q = head;
        cur_len = other.cur_len;
        for (size_t i = 1; i <= cur_len;++i){
            p = p->next;
            q->next = new node(*(p->data));
            q->next->prev = q;
            q = q->next;
        }
        q->next = tail;
        tail->prev = q;
    }
    /**
     * TODO Destructor
     */
    virtual ~list() {
        clear();
        delete head;
        delete tail;
    }
    /**
     * TODO Assignment operator
     */
    list &operator=(const list &other) {
        if (this==&other)
            return *this;
        clear();
        delete head;
        delete tail;
        cur_len = other.cur_len;
        head = new node();
        tail = new node();
        node *p = other.head;
        node *q = head;
        for (size_t i = 1; i <= cur_len;++i){
            p = p->next;
            q->next = new node(*(p->data));
            q->next->prev = q;
            q = q->next;
        }
        q->next = tail;
        tail->prev = q;
        return *this;
    }
    /**
     * access the first / last element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const {
        if (cur_len==0)
            throw container_is_empty();
        return *(head->next->data);
    }
    const T & back() const {
        if (cur_len==0)
            throw container_is_empty();
        return *(tail->prev->data);
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        iterator iter(head->next, this);
        return iter;
    }
    const_iterator cbegin() const {
        const_iterator c_iter(head->next, this);
        return c_iter;
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        iterator iter(tail, this);
        return iter;
    }
    const_iterator cend() const {
        const_iterator c_iter(tail, this);
        return c_iter;
    }
    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const {
        if (cur_len) return 0;
        return 1;
    }
    /**
     * returns the number of elements
     */
    virtual size_t size() const {
        return cur_len;
    }

    /**
     * clears the contents
     */
    virtual void clear() {
        size_t num = cur_len;
        for (size_t i = 0; i < num;++i){
            pop_back();
        }
    }
    /**
     * insert value before pos (pos may be the end() iterator)
     * return an iterator pointing to the inserted value
     * throw if the iterator is invalid
     */
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.lis!=this || pos.pnode==nullptr || pos.pnode==head)
            throw invalid_iterator();
        node *n = new node(value);
        iterator tmp(insert(pos.pnode, n), this);
        return tmp;
    }
    /**
     * remove the element at pos (the end() iterator is invalid)
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid
     */
    virtual iterator erase(iterator pos) {
        if (!cur_len)
            throw container_is_empty();
        if (pos.lis!=this || pos.pnode==nullptr || pos.pnode==head || pos.pnode==tail)
            throw invalid_iterator();
        iterator tmp(pos.pnode->next, this);
        node *erased = erase(pos.pnode);
        delete erased;
        return tmp;
    }
    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        insert(end(), value);
        return;
    }
    /**
     * removes the last element
     * throw when the container is empty.
     */
    void pop_back() {
        if (!cur_len)
            throw container_is_empty();
        iterator iter(tail->prev, this);
        erase(iter);
        return;
    }
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        iterator iter(head->next, this);
        insert(iter, value);
        return;
    }
    /**
     * removes the first element.
     * throw when the container is empty.
     */
    void pop_front() {
        if (!cur_len)
            throw container_is_empty();
        erase(begin());
        return;
    }
    /**
     * sort the values in ascending order with operator< of T
     */
    void sort() {
        auto cmp = [](const T& a, const T& b)->bool { return a < b; };
        iterator iter(head, this);
        T *tmp = (T *)malloc(cur_len * sizeof(T));
        for (size_t i = 0; i < cur_len;++i){
            ++iter;
            new (tmp + i) T(*iter);
            (*iter).~T();
        }
        iter.pnode = head;
        sjtu::sort<T>(tmp, tmp + cur_len, cmp);
        for (size_t i = 0; i < cur_len;++i){
            ++iter;
            new (&(*iter)) T(tmp[i]);
            tmp[i].~T();
        }
        free(tmp);
    }
    /**
     * merge two sorted lists into one (both in ascending order)
     * compare with operator< of T
     * container other becomes empty after the operation
     * for equivalent elements in the two lists, the elements from *this shall always precede the elements from other
     * the order of equivalent elements of *this and other does not change.
     * no elements are copied or moved
     */
    void merge(list &other) {
        iterator ite1(head->next, this), ite2(other.head->next, &other);
        while (ite1.pnode!=tail) {
            if (ite2==other.end())
                break;
            while (*ite2<*ite1) {
                node* tmp = ite2.pnode;
                ite2.pnode=ite2.pnode->next;
                other.erase(tmp);
                insert(ite1.pnode, tmp);
                if (ite2==other.end()) break;
            }
            ++ite1;
        }
        while (ite2!=other.end()){
            node* tmp = ite2.pnode;
            ite2.pnode=ite2.pnode->next;
            other.erase(tmp);
            insert(ite1.pnode, tmp);
        }
    }
    /**
     * reverse the order of the elements
     * no elements are copied or moved
     */
    void reverse() {
        if (cur_len<=1)
            return;
        node *p = head->next;
        node *q = tail->prev;
        tail->next = q;
        tail->prev = nullptr;
        head->next = nullptr;
        head->prev = p;
        q = head, head = tail, tail = q;
        for (size_t i = 1; i <= cur_len;++i){
            q = p->next;
            p->next = p->prev;
            p->prev = q;
            p = p->prev;
        }
    }
    /**
     * remove all consecutive duplicate elements from the container
     * only the first element in each group of equal elements is left
     * use operator== of T to compare the elements.
     */
    void unique() {
        if (cur_len<=1)
            return;
        node *p=head->next;
        while (p->next!=tail){
            if (*(p->data)==*(p->next->data)) {
                node *erased = erase(p->next);
                delete erased;
            }
            else p = p->next;
        }
    }
};

}

#endif //SJTU_LIST_HPP