#ifndef SJTU_LINKED_HASHMAP_HPP_STD
#define SJTU_LINKED_HASHMAP_HPP_STD

#include <cstddef>
#include <functional>
#include "utility.hpp"
#include "algorithm.hpp"
#include "exceptions.hpp"
#include "list.hpp"

namespace sjtu {
	/**
	 *  Maintains key-value pairs just like MAP
	 *  Dynamically sized hash table who handles collision with linked lists
	 *  Iterators arrange in order of insertion (maintained by base class LIST)
	 */

template <
        class Key,
        class Value,
        class Hash = std::hash<Key>,
        class Equal = std::equal_to<Key>
>
class linked_hashmap : public list<pair<const Key, Value> > {
public:
    using value_type = pair<const Key, Value>;
    using list<value_type>::head;
    using list<value_type>::tail;
    using list<value_type>::nil;
    static constexpr size_t CAPACITY = 1 << 4;
    static constexpr float LOAD_FACTOR = 0.75f;
    static constexpr size_t THRESHOLD = CAPACITY * LOAD_FACTOR;
    size_t cap, thre;
    static size_t get_hash(const Key&key) {
        static Hash hash;
        return hash(key);
    }
    size_t index(size_t h, size_t len) const {
        return h & (len - 1);
    }
    size_t index(const Key& key) const {
        return index(get_hash(key), cap);
    }
private:
    class Node : public list<value_type>::node {
    public:
        /**
         * add data members in addition to class node in LIST
         */
        Node *nx;
        size_t hv;
        Node():nx(nullptr),hv(-1){}
        Node(const Key &k,const Value &v):nx(nullptr){
            this->val=new value_type(k,v);
            hv=get_hash(k);
        }
        Node(const value_type &kv):nx(nullptr){
            this->val=new value_type(kv);
            hv=get_hash(kv->first);
        }
    };

    /**
     * singly-linked list used for hash collision
     */
    class BucketList {
    public:
        /**
         * data members, constructors and destructor
         */
        Node *head;
        BucketList():head(new Node()){}
        ~BucketList(){
            delete head;
        }
        /**
         *  TODO find corresponding Node with key o
         */
        Node * find(const Key &o) {
            Equal equal;
            Node *p=head->nx;
            while (p) {
                if (equal(p->val->first,o)) return p;
                p=p->nx;
            }
            return nullptr;
        }
        /**
         * TODO insert key-value pair(k, v) into this BucketList
         * return this new Node
         */
        Node * insert(const Key &k, const Value &v) {
            Node *p=new Node(k,v);
            p->nx=head->nx;
            head->nx=p;
            size_t hash_val=get_hash(k);
            p->hv=hash_val;
            return p;
        }
        Node * insert(const value_type &kv) {
            return insert(kv.first, kv.second);
        }
        /**
         * TODO remove the Node with key k from this BucketList (no need to delete)
         * return the removed Node
         */
        Node * erase(const Key &k) {
            Equal equal;
            Node *p=head;
            Node *q=head->nx;
            while (q && !equal(q->val->first,k)) q=q->nx,p=p->nx;
            if (!q) return nullptr;
            p->nx=q->nx;
            q->nx=nullptr;
            return q;
        }
    };

    /**
     * add data members as needed and necessary private function such as resize()
     */
    BucketList *hashtable;
    void resize(size_t newCap) {
        cap = newCap;
        thre = cap * LOAD_FACTOR;
        if (hashtable) delete [] hashtable;
        hashtable = new BucketList[cap];
        for (typename list<value_type>::node* p = head->nxt; p != this->nil; p = p->nxt) {
            Node* q = dynamic_cast<Node*>(p);
            Node*& n = hashtable[index(q->hv,cap)].head;
            //resize和copy在这里不同的原因是resize插入的是list上的原节点，而copy是创建一个新节点插入，调用insert会创建新节点
            q->nx = n->nx;
            n->nx = q;
        }
    }

    void copy(const linked_hashmap& other) {
        cap = other.cap;
        thre = other.thre;
        this->num = 0;
        hashtable = new BucketList[cap];
        for (typename list<value_type>::node* p = other.head->nxt; p != other.nil; p = p->nxt) {
            Node* n = hashtable[index(p->val->first)].insert(*p->val);
            list<value_type>::insert(this->nil, n);
        }
    }
public:
    /**
     * iterator is the same as LIST
     */
    using iterator = typename list<value_type>::iterator;
    using const_iterator = typename list<value_type>::const_iterator;

    /**
    * TODO two constructors
    */
    linked_hashmap():cap(0),thre(0),hashtable(nullptr) {}
    linked_hashmap(const linked_hashmap &other) {
        this->copy(other);
    }
    /**
	 * TODO assignment operator
	 */
    linked_hashmap &operator=(const linked_hashmap &other) {
        if (this==&other) return *this;
        list<value_type>::clear();
        delete []hashtable;
        this->copy(other);
        return *this;
    }
    /**
	 * TODO Destructors
	 */
    ~linked_hashmap() {
        //clear();这里会再次调用list的析构函数所有不用手动clear了
        delete []hashtable;
    }
    /**
	 * TODO access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
    Value &at(const Key &key) {
        if (!hashtable) throw index_out_of_bound();
        Node *p=hashtable[index(key)].find(key);
        if (!p) throw index_out_of_bound();
        return p->val->second;
    }
    const Value &at(const Key &key) const {
        if (!hashtable) throw index_out_of_bound();
        Node *p=hashtable[index(key)].find(key);
        if (!p) throw index_out_of_bound();
        return p->val->second;
    }
    /**
	 * TODO access specified element
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
    Value &operator[](const Key &key) {
        if (!hashtable) return insert({key, Value()}).first->second;
        Node *p=hashtable[index(key)].find(key);
        if (!p) {
            if (this->num>=thre) resize(cap<<1);
            p=hashtable[index(key)].insert(key,Value());
            list<value_type>::insert(this->nil,p);
        }
        return p->val->second;
    }
    /**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
    const Value &operator[](const Key &key) const {
        return this->at(key);
    }
    /**
	 * TODO override clear() in LIST
	 */
    void clear() override{
        if (!hashtable) return;
        list<value_type>::clear();
        delete [] hashtable;
        cap=CAPACITY;
        thre=THRESHOLD;
        hashtable = new BucketList[cap];
    }
    /**
	 * TODO insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion),
	 *   the second one is true if insert successfully, or false.
	 */
    pair<iterator, bool> insert(const value_type &value) {
        if (!hashtable) resize(CAPACITY);
        Node *n=hashtable[index(value.first)].find(value.first);
        if (n) return {iterator(n,this),false};
        else {
            if (this->num>=thre) resize(cap<<1);
            n=hashtable[index(value.first)].insert(value);
            list<value_type>::insert(this->nil,n);
            return {iterator(n,this),true};
        }
    }
    /**
	 * TODO erase the element at pos.
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     * return anything, it doesn't matter
	 */
    iterator erase(iterator pos) override{
        if (!hashtable || pos.invalid(this)) throw invalid_iterator();
        hashtable[index(pos->first)].erase(pos->first);
        iterator ite=list<value_type>::erase(pos);
        if (this->num<=cap>>2) resize(cap>>1);
        return ite;
    }
    /**
	 * TODO Returns the number of elements with key
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0
	 *     since this container does not allow duplicates.
	 */
    size_t count(const Key &key) const {
        if (!hashtable) return 0;
        Node *n=hashtable[index(key)].find(key);
        if (n) return 1;
        else return 0;
    }
    /**
	 * TODO Finds an element with key equivalent to key.
	 * return iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
    iterator find(const Key &key) {
        if (!hashtable) return this->end();
        Node *n=hashtable[index(key)].find(key);
        if (!n) return this->end();
        else return iterator(n,this);
    }
    const_iterator find(const Key &key) const {
        if (!hashtable) this->cend();
        Node *n=hashtable[index(key)].find(key);
        if (!n) return this->cend();
        else return const_iterator(n,this);
    }
};

}

#endif