#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"
#include <iostream>
#include <cstdio>
#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector {
private:
    T *data;
    size_t cur_len;
    size_t max_len;
    void doubleSpace(){
        T *tmp = data;
        data = (T *)malloc(2 * max_len * sizeof(T));
        for (size_t i = 0; i < cur_len;++i){
            new (data + i) T(tmp[i]);
            tmp[i].~T();
        }
        max_len *= 2;
        free(tmp);
    }
public:
    /**
     * TODO
     * a type for actions of the elements of a vector, and you should write
     *   a class named const_iterator with same interfaces.
     */
    /**
     * you can see RandomAccessIterator at CppReference for help.
     */
    
    class const_iterator;
    class iterator {
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
    public:
        size_t po;
        vector *vec;
        iterator(){}
        iterator(size_t _pos,vector* _vec):po(_pos),vec(_vec){}
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        iterator operator+(const int &n) const {
            //TODO
            iterator tmp = *this;
            tmp.po += n;
            return tmp;
        }
        iterator operator-(const int &n) const {
            //TODO
            iterator tmp = *this;
            tmp.po -= n;
            return tmp;
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw invaild_iterator.
        int operator-(const iterator &rhs) const {
            //TODO
            if (this->vec!=rhs.vec)
                throw invalid_iterator();
            //潜在错误：是否取绝对值、正负
            return this->po - rhs.po;
        }
        iterator& operator+=(const int &n) {
            //TODO
            this->po += n;
            return *this;
        }
        iterator& operator-=(const int &n) {
            //TODO
            this->po -= n;
            return *this;
        }
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            iterator tmp = *this;
            ++this->po;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        iterator& operator++() {
            ++this->po;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            iterator tmp = *this;
            --this->po;
            return tmp;
        }
        /**
         * TODO --iter
         */
        iterator& operator--() {
            --this->po;
            return *this;
        }
        /**
         * TODO *it
         */
        T& operator*() const{
            return this->vec->data[po];
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory address).
         */
        bool operator==(const iterator &rhs) const {
            if (this->vec==rhs.vec && this->po==rhs.po) return 1;
            return 0;
        }
        bool operator==(const const_iterator &rhs) const {
            if (this->vec==rhs.vec && this->po==rhs.po) return 1;
            return 0;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return !(this->operator==(rhs));
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(this->operator==(rhs));
        }
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
    public:
        size_t po;
        const vector *vec;
        const_iterator(){}
        const_iterator(size_t _pos,const vector* _vec):po(_pos),vec(_vec){}
        const_iterator operator+(const int &n) const {
            const_iterator tmp(this->po + n, this->vec);
            return tmp;
        }
        const_iterator operator-(const int &n) const {
            const_iterator tmp(this->po-n,this->vec);
            return tmp;
        }
        int operator-(const const_iterator &rhs){
            if (this->vec!=rhs.vec)
                throw invalid_iterator();
                //潜在错误：是否取绝对值、正负
            return this->po - rhs.po;
        }
        const_iterator& operator+=(const int &n) {
            this->po += n;
            return *this;
        }
        const_iterator& operator-=(const int &n) {
            this->po -= n;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++this->po;
            return tmp;
        }
        const_iterator& operator++() {
            ++this->po;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --this->po;
            return tmp;
        }
        const_iterator& operator--() {
            --this->po;
            return *this;
        }
        const T& operator*() const{
            return this->vec->data[po];
        }
        bool operator==(const iterator &rhs) const {
            if (this->vec==rhs.vec && this->po==rhs.po) return 1;
            return 0;
        }
        bool operator==(const const_iterator &rhs) const {
            if (this->vec==rhs.vec && this->po==rhs.po) return 1;
            return 0;
        }
        bool operator!=(const iterator &rhs) const {
            return !(this->operator==(rhs));
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(this->operator==(rhs));
        }
    };
    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    vector() {
        cur_len = 0;
        max_len = 1;
        data = (T*)malloc(max_len*sizeof(T));
    }
    vector(const vector &other) {
        this->cur_len = other.cur_len;
        //潜在错误：T可能没有默认的构造函数and没free空间
        this->max_len = other.max_len;
        this->data = (T*)malloc(max_len*sizeof(T));
        for (size_t i = 0; i < cur_len;++i) new (data + i) T(other.data[i]);
    }
    /**
     * TODO Destructor
     */
    ~vector() {
        clear();
        free(data);
    }
    /**
     * TODO Assignment operator
     */
    vector &operator=(const vector &other) {
        if (this->data==other.data) return *this;
        clear();
        free(this->data);
        this->cur_len = other.cur_len;
        this->max_len = other.max_len;
        this->data = (T *)malloc(max_len * sizeof(T));
        for (size_t i = 0; i < cur_len;++i) new (data + i) T(other.data[i]);
        return *this;
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T & at(const size_t &pos) {
        if (pos<0||pos>=cur_len) throw index_out_of_bound();
        return this->data[pos];
    }
    const T & at(const size_t &pos) const {
        if (pos<0||pos>=cur_len) throw index_out_of_bound();
        return this->data[pos];
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T & operator[](const size_t &pos) {
        if (pos<0||pos>=cur_len) throw index_out_of_bound();
        return this->data[pos];
    }
    const T & operator[](const size_t &pos) const {
        if (pos<0||pos>=cur_len) throw index_out_of_bound();
        return this->data[pos];
    }
    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T & front() const {
        if (cur_len==0) throw container_is_empty();
        return this->data[0];
    }
    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T & back() const {
        if (cur_len==0) throw container_is_empty();
        return this->data[cur_len - 1];
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        iterator iter(0, this);
        return iter;
    }
    const_iterator cbegin() const {
        const_iterator c_iter(0, this);
        return c_iter;
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        iterator iter(cur_len - 1, this);
        return iter;
    }
    const_iterator cend() const {
        const_iterator c_iter(cur_len - 1, this);
        return c_iter;
    }
    /**
     * checks whether the container is empty
     */
    bool empty() const {
        if (cur_len) return 0;
        return 1;
    }
    /**
     * returns the number of elements
     */
    size_t size() const {
        return cur_len;
    }
    /**
     * clears the contents
     */
    void clear() {
        //也可以顺便改改max_len
        for (size_t i = 0; i < cur_len;++i){
            data[i].~T();
        }
        cur_len = 0;
    }
    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value) {
        if (cur_len>=max_len) doubleSpace();
        for (size_t i = cur_len; i > pos.po;--i){
            new (data + i) T(data[i - 1]);
        }
        new (data + pos.po) T(value);
        ++cur_len;
        return pos;
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value) {
        if (ind>cur_len) throw index_out_of_bound();
        if (cur_len>=max_len) doubleSpace();
        iterator iter(ind, this);
        for (size_t i = cur_len; i > ind;--i){
            new (data + i) T(data[i - 1]);
        }
        new (data + ind) T(value);
        ++cur_len;
        return iter;
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator pos) {
        for (size_t i = pos.po; i < cur_len - 1;++i){
            new (data + i) T(data[i + 1]);
        }
        --cur_len;
        data[cur_len].~T();
        if (pos.po==cur_len) return end();
        return pos;
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind) {
        if (ind>=cur_len) throw index_out_of_bound();
        iterator iter(ind, this);
        for (size_t i = ind; i < cur_len - 1;++i){
            new (data + i) T(data[i + 1]);
        }
        --cur_len;
        data[cur_len].~T();
        if (ind==cur_len) throw index_out_of_bound();
        return iter;
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T &value) {
        if(cur_len==max_len){
            doubleSpace();
            new(data+cur_len)T(value);
        }
        else new(data+cur_len)T(value);
        ++cur_len;
        return;
    }
        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
    void pop_back() {
        if(cur_len==0) throw container_is_empty();
        --cur_len;
        data[cur_len].~T();
        return;
    }
};


}

#endif