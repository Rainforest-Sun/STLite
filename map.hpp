/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
    class Key,
    class T,
    class Compare = std::less<Key>
> class map {
public:
    typedef pair<const Key, T> value_type;
private:
    bool equal(const Key& x,const Key& y) const
    {
        Compare cmp;
        return (!cmp(x,y) && !cmp(y,x));
    }
    class Node{
    public:
        value_type *data;
        Node *fa,*ls,*rs;
        int h;
        
        Node():data(NULL),fa(NULL),ls(NULL),rs(NULL),h(0){}
        Node(const value_type& val,Node *_fa=NULL,Node *_ls=NULL,Node *_rs=NULL):data(new value_type(val)),fa(_fa),ls(_ls),rs(_rs),h(1){}
        ~Node(){if (data) delete data;}
    };

    //map的成员
    Node *root;
    size_t cur_size;
    int get_height(Node *x) const{
		return (x==NULL?0:x->h);
	}
    void update_height(Node* x){
		int lh=get_height(x->ls);
		int rh=get_height(x->rs);
		x->h=(lh>rh?lh+1:rh+1);
	}
    //给迭代器写的中序遍历：得到第一个元素要一直往左走
    //得到下一个元素：遍历该元素的右子树，如果右子树存在，则找到右子树的第一个元素（一直往左走），如果右子树不存在，往上找到由左儿子找到的祖先并停
    Node* get_first() const
    {
        Node* t=root;
		while (t->ls!=NULL) t=t->ls;
		return t;
    }
    Node* get_last() const
    {
        return this->root;
    }
    Node* get_next(Node *x) const
    {
        if (x==root) return NULL;
		Node* t=x->rs;
		if (t!=NULL){
			while (t->ls!=NULL) t=t->ls;
			return t;
		}
		t=x;
		while (t->fa->ls!=t) t=t->fa;
		return t->fa;
    }
    Node* get_prev(Node *x) const
    {
        Node* t=x->ls;
		if (t!=NULL){
			while (t->rs!=NULL) t=t->rs;
			return t;
		}
		t=x;
		while (t!=root && t->fa->rs!=t) t=t->fa;
		return (t==root?NULL:t->fa);
    }
    //四个旋转函数
    void LL(Node *&x){
		Node* l=x->ls;
		Node* par=x->fa;
		x->ls=l->rs;
		l->rs=x;
		x=l;
		x->fa=par;
		x->rs->fa=x;
		if (x->rs->ls!=NULL) x->rs->ls->fa=x->rs;
		update_height(x->rs);
		update_height(x);
	}
	
	void RR(Node *&x){
		Node* r=x->rs;
		Node* par=x->fa;
		x->rs=r->ls;
		r->ls=x;
		x=r;
		x->fa=par;
		x->ls->fa=x;
		if (x->ls->rs!=NULL) x->ls->rs->fa=x->ls;
		update_height(x->ls);
		update_height(x);
	}
	
	void LR(Node *&x){
		RR(x->ls);
		LL(x);
	}
	
	void RL(Node *&x){
		LL(x->rs);
		RR(x);
	}
    Node* insert(Node *&x,Node* p,const value_type &val){
		Node* tmp=NULL;
		if (x==NULL){
			x=new Node(val,p);
			return x;
		}
		if (Compare()(val.first,x->data->first)){
			tmp=insert(x->ls,x,val);
			if (x->ls->h-get_height(x->rs)>=2){
				if (Compare()(val.first,x->ls->data->first)) LL(x); else LR(x);
			}
		}
		else{
			tmp=insert(x->rs,x,val);
			if (x->rs->h-get_height(x->ls)>=2){
				if (Compare()(x->rs->data->first,val.first)) RR(x); else RL(x);
			}
		}
		update_height(x);
		return tmp;
	}

    bool adjust(Node *&x,int dir){
		int lh=get_height(x->ls);
		int rh=get_height(x->rs);
		if (dir==0){
			if (lh==rh) {update_height(x);return 1;}
			if (lh==rh-1) return 0;
			int rlh=get_height(x->rs->ls);
			int rrh=get_height(x->rs->rs);
			if (rlh==rrh) {RR(x);return 0;}
			else if (rlh<rrh) {RR(x);return 1;}
			else {RL(x);return 1;}		
		}
		else{
			if (lh==rh) {update_height(x);return 1;}
			if (rh==lh-1) return 0;
			int llh=get_height(x->ls->ls);
			int lrh=get_height(x->ls->rs);
			if (llh==lrh) {LL(x);return 0;}
			else if (lrh<llh) {LL(x);return 1;}
			else {LR(x);return 1;}
		}
	}
	
	bool erase(Node *&x,Node* par,const Key& target){
		if (x==NULL) return false;
		const Key &cur=x->data->first;
		if (equal(target,cur)){
			if (x->ls==NULL || x->rs==NULL){
				Node* t=x;
				x=(t->ls==NULL?t->rs:t->ls);
				if (x!=NULL) x->fa=par;
				delete t;
				return true;
			}
			else{
				Node* t=x->rs;
				while (t->ls!=NULL) t=t->ls;

				Node* newNode=new Node(*(t->data),t->fa,NULL,t->rs);
				update_height(newNode);
				if (newNode->rs!=NULL) newNode->rs->fa=newNode;
				if (newNode->fa->ls==t) newNode->fa->ls=newNode;
				else newNode->fa->rs=newNode;
				
				t->fa=x->fa;
				t->ls=x->ls;
				t->rs=x->rs;
				update_height(t);
				t->ls->fa=t;
				t->rs->fa=t;
				Node* oldx=x;
				x=t;
				
				delete oldx;
				if (!erase(x->rs,x,x->data->first)) return false;
                else return adjust(x,1);
			}
		}
		else{
			if (Compare()(target,x->data->first)){
				if (!erase(x->ls,x,target)) return false;
                else return adjust(x,0);
			}
			else{
				if (!erase(x->rs,x,target)) return false;
                else return adjust(x,1);
			}
		}
	}
	
    void copy(Node* &x,Node *p,Node *t)
    {
        if (t==NULL) {
            x=NULL;
            return;
        }
        x=new Node(*(t->data),p);
        x->h=t->h;
        copy(x->ls,x,t->ls);
        copy(x->rs,x,t->rs);
    }
    void make_empty(Node* &x)
    {
        if (!x) return;
        make_empty(x->ls);
        make_empty(x->rs);
        delete x;
        x=NULL;
    }
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
public:
    class const_iterator;
    class iterator {
        friend class map;
    private:
        map *ctx;
        Node *ptn;
    public:
        iterator() {
            // TODO
            ctx=NULL;
            ptn=NULL;
        }
        iterator(map *_ctx,Node *_ptn) {
            ctx=_ctx;
            ptn=_ptn;
        }
        iterator(const iterator &other) {
            // TODO
            ctx=other.ctx;
            ptn=other.ptn;
        }
        /**
         * return a new iterator which pointer n-next elements
         *   even if there are not enough elements, just return the answer.
         * as well as operator-
         */
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            if (ctx==NULL) throw invalid_iterator();
            iterator ite=*this;
            Node* nxt=ctx->get_next(ptn);
			if (nxt==NULL) throw invalid_iterator();
			ptn=nxt;
            return ite;
        }
        /**
         * TODO ++iter
         */
        iterator & operator++() {
            if (ctx==NULL) throw invalid_iterator();
            Node* nxt=ctx->get_next(ptn);
			if (nxt==NULL) throw invalid_iterator();
			ptn=nxt;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            if (ctx==NULL) throw invalid_iterator();
            iterator ite=*this;
            Node* prv=ctx->get_prev(ptn);
			if (prv==NULL) throw invalid_iterator();
			ptn=prv;
            return ite;
        }
        /**
         * TODO --iter
         */
        iterator & operator--() {
            if (ctx==NULL) throw invalid_iterator();
            Node* prv=ctx->get_prev(ptn);
			if (prv==NULL) throw invalid_iterator();
			ptn=prv;
            return *this;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        value_type & operator*() const {
            if (ctx==NULL || ptn==ctx->root) throw invalid_iterator();
            return *(this->ptn->data);
        }
        bool operator==(const iterator &rhs) const {
            return (this->ctx==rhs.ctx && this->ptn==rhs.ptn);
        }
        bool operator==(const const_iterator &rhs) const {
            return (this->ctx==rhs.ctx && this->ptn==rhs.ptn);
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return (this->ctx!=rhs.ctx || this->ptn!=rhs.ptn);
        }
        bool operator!=(const const_iterator &rhs) const {
            return (this->ctx!=rhs.ctx || this->ptn!=rhs.ptn);
        }

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
         */
        value_type* operator->() const noexcept {
            if (ctx==NULL || ptn==ctx->root) throw invalid_iterator();
            return this->ptn->data;
        }
    };
    class const_iterator {
        friend class map;
    private:
        const map* ctx;
		Node* ptn; 
    public:
        const_iterator() {
            // TODO
            ctx=NULL;
            ptn=NULL;
        }
        const_iterator(const map *_ctx,Node *_ptn){
            ctx=_ctx;
            ptn=_ptn;
        }
        const_iterator(const const_iterator &other) {
            // TODO
            ctx=other.ctx;
            ptn=other.ptn;
        }
        const_iterator(const iterator &other) {
            // TODO
            ctx=other.ctx;
            ptn=other.ptn;
        }
        // And other methods in iterator.
        // And other methods in iterator.
        // And other methods in iterator.
        const_iterator operator++(int) {
            if (ctx==NULL) throw invalid_iterator();
            const_iterator ite=*this;
            Node* nxt=ctx->get_next(ptn);
			if (nxt==NULL) throw invalid_iterator();
			ptn=nxt;
            return ite;
        }
        const_iterator & operator++() {
            if (ctx==NULL) throw invalid_iterator();
            Node* nxt=ctx->get_next(ptn);
			if (nxt==NULL) throw invalid_iterator();
			ptn=nxt;
            return *this;
        }
        const_iterator operator--(int) {
            if (ctx==NULL) throw invalid_iterator();
            const_iterator ite=*this;
            Node* prv=ctx->get_prev(ptn);
			if (prv==NULL) throw invalid_iterator();
			ptn=prv;
            return ite;
        }
        const_iterator & operator--() {
            if (ctx==NULL) throw invalid_iterator();
            Node* prv=ctx->get_prev(ptn);
			if (prv==NULL) throw invalid_iterator();
			ptn=prv;
            return *this;
        }
        value_type & operator*() const {
            if (ctx==NULL || ptn==ctx->root) throw invalid_iterator();
            return *(this->ptn->data);
        }
        bool operator==(const iterator &rhs) const {
            return (this->ctx==rhs.ctx && this->ptn==rhs.ptn);
        }
        bool operator==(const const_iterator &rhs) const {
            return (this->ctx==rhs.ctx && this->ptn==rhs.ptn);
        }
        bool operator!=(const iterator &rhs) const {
            return (this->ctx!=rhs.ctx || this->ptn!=rhs.ptn);
        }
        bool operator!=(const const_iterator &rhs) const {
            return (this->ctx!=rhs.ctx || this->ptn!=rhs.ptn);
        }
        value_type* operator->() const noexcept {
            if (ctx==NULL || ptn==ctx->root) throw invalid_iterator();
            return this->ptn->data;
        }
    };
    /**
     * TODO two constructors
     */
    map() {
        root=new Node();
        cur_size=0;
    }
    map(const map &other) {
        root=new Node();
        cur_size=other.cur_size;
        copy(root->ls,root,other.root->ls);
    }
    /**
     * TODO assignment operator
     */
    map & operator=(const map &other) {
        if (this==&other) return *this;
        clear();
        cur_size=other.cur_size;
        copy(root->ls,root,other.root->ls);
        return *this;
    }
    /**
     * TODO Destructors
     */
    ~map() {
        clear();
        delete root;
    }
    /**
     * TODO
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key equivalent to key.
     * If no such element exists, an exception of type `index_out_of_bound'
     */
    Node* find(Node* x,const Key &key) const {
        if (x==nullptr) return NULL;
        if (equal(x->data->first,key)) return x;
        if (Compare()(key,x->data->first)) return find(x->ls,key);
        return find(x->rs,key);
    }
    T & at(const Key &key) {
        Node* t=find(root->ls,key);
        if (t==NULL) throw index_out_of_bound();
        return t->data->second;
    }
    const T & at(const Key &key) const {
        Node* t=find(root->ls,key);
        if (t==NULL) throw index_out_of_bound();
        return t->data->second;
    }
    /**
     * TODO
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to key,
     *   performing an insertion if such key does not already exist.
     */
    T & operator[](const Key &key) {
        Node* t=find(root->ls,key);
        if (t==NULL) {
            t=insert(root->ls,root,value_type(key,T()));
            ++cur_size;
        }
        return t->data->second;
    }
    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T & operator[](const Key &key) const {
        Node* t=find(root->ls,key);
        if (t==NULL) throw index_out_of_bound();
        return t->data->second;
    }
    /**
     * return a iterator to the beginning
     */
    iterator begin() {
        return iterator(this,get_first());
    }
    const_iterator cbegin() const {
        return const_iterator(this,get_first());
    }
    /**
     * return a iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() {
        return iterator(this,get_last());
    }
    const_iterator cend() const {
        return const_iterator(this,get_last());
    }
    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const {
        return cur_size==0;
    }
    /**
     * returns the number of elements.
     */
    size_t size() const {
        return cur_size;
    }
    /**
     * clears the contents
     */
    void clear() {
        make_empty(root->ls);
        cur_size=0;
    }
    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    pair<iterator, bool> insert(const value_type &value) {
        Node* t=find(root->ls,value.first);
        if (t) return {iterator(this,t),false};
        t=insert(root->ls,root,value);
        ++cur_size;
        return {iterator(this,t),true};
    }
    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos) {
        if (pos.ctx!=this || pos.ptn==root || pos.ptn==NULL) throw invalid_iterator();
        erase(root->ls,root,pos.ptn->data->first);
        --cur_size;
    }
    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const {
        return (find(root->ls,key)?1:0);
    }
    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key &key) {
        Node *t=find(root->ls,key);
        if (t) return iterator(this,t);
        else return end(); 
    }
    const_iterator find(const Key &key) const {
        Node *t=find(root->ls,key);
        if (t) return const_iterator(this,t);
        else return cend(); 
    }
};

}

#endif