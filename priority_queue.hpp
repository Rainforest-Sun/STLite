#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
public:
    class node{
        public:
            T* data;
            size_t npl;
            node *lson;
            node *rson;
            node *fa;
            node():data(nullptr),npl(-1),lson(nullptr),rson(nullptr),fa(nullptr){}
            // node(T &value, node *father = nullptr) : fa(father), lson(nullptr), rson(nullptr)
            // {
            //     if (lson==nullptr || rson==nullptr) npl = 0;
            //     else npl = 1;
            //     data = new T(value);
            // }
            node(T &value, size_t _npl,node *father = nullptr) :npl(_npl), fa(father), lson(nullptr), rson(nullptr)
            {
                data = new T(value);
            }
            ~node(){
                if (data) delete data;
            }
    };
    node *root;
    size_t cur_size;
    /**
	 * TODO constructors
	 */
private:
    void dfs(node* n,node* _n)
    {
        node *tmp;
        tmp = _n->lson;
        if (tmp) {
            n->lson = new node(*(tmp->data), tmp->npl, n);
            dfs(n->lson, tmp);
        }
        tmp = _n->rson;
        if (tmp) {
            n->rson = new node(*(tmp->data), tmp->npl, n);
            dfs(n->rson, tmp);
        }
    }
    void clear(node *n)
    {
        if (!n) return;
        if (n->lson) clear(n->lson);
        if (n->rson) clear(n->rson);
        delete n;
    }
    void swap(node *&rt1,node *&rt2)
    {
        node *tmp = rt1;
        rt1 = rt2, rt2 = tmp;
    }
    node* merge_(node *&rt1,node *&rt2)
    {
        if (!rt1) return rt2;
        if (!rt2) return rt1;
        if (Compare()(*(rt1->data),*(rt2->data))) swap(rt1, rt2);
        if (Compare()(*(rt2->data),*(rt1->data))) {
            rt1->rson = merge_(rt1->rson, rt2);
            if (rt1->lson==nullptr || rt1->lson->npl<rt1->rson->npl) swap(rt1->lson, rt1->rson);
            if (rt1->rson) rt1->npl = rt1->rson->npl + 1;
            else rt1->npl = 0;
            return rt1;
        }
    }
public:
	priority_queue() {
        root = nullptr;
        cur_size = 0;
    }
	priority_queue(const priority_queue &other) {
        cur_size = other.cur_size;
        root = new node(*(other.root->data), other.root->npl, nullptr);
        dfs(root, other.root);
    }
	/**
	 * TODO deconstructor
	 */
	~priority_queue() {
        clear(root);
    }
	/**
	 * TODO Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) {
        if (this==&other)
            return *this;
        clear(root);
        cur_size = other.cur_size;
        root = new node(*(other.root->data), other.root->npl, nullptr);
        dfs(root, other.root);
        return *this;
    }
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const {
        if (empty()) throw container_is_empty();
        return *(root->data);
    }
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
	void push(const T &e) {
        T ee = e;
        node *tmp = new node(ee, 0,nullptr);
        root=merge_(root,tmp);
        ++cur_size;
    }
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() {
        if (empty()) throw container_is_empty();
        node *tmp = root;
        root = root->lson;
        root = merge_(root, tmp->rson);
        --cur_size;
        delete tmp;
    }
	/**
	 * return the number of the elements.
	 */
	size_t size() const {
        return cur_size;
    }
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const {
        if (cur_size) return false;
        else return true;
    }
	/**
	 * merge two priority_queues with at least O(logn) complexity.
	 * clear the other priority_queue.
	 */
	void merge(priority_queue &other) {
        root=merge_(root, other.root);
        cur_size += other.cur_size;
        other.cur_size = 0;
        other.root = nullptr;
    }
};

}

#endif