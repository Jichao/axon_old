// hash table with iteration ability(addition link list)
// Created: Daly 2012-11-18

#ifndef _AX_HASHMAP_H_
#define _AX_HASHMAP_H_

#include "stdheader.h"
#include "ax_debug.h"
#include "ax_string.h"

/*
 * The most frequently used data structure in server engine 
 * is that support both key-value data search/insert/remove and iteration.
 * The implementation below combined HashTable and LinkList, 
 * which satisfy the needs in most situation.
 * Sometimes, the hash table has huge number of items, 
 * but iteration is not necessary. 
 * So the LinkList pointer in hmap_entry_t may be waste of memory. 
 * But this is rare situation, if memory usage becomes a problem, it's easy to implement a specific hash table for this module.
 *
 * */

namespace axon {

template<class _K>
class hmap_t 
{
public:
	template<class _Key>
	class hmap_entry_t 
	{
	public:
		friend class hmap_t<_Key>;
		hmap_entry_t() : 
			data(NULL), 
			next(NULL), prev(NULL), link_next(NULL), link_prev(NULL)
		{}
		~hmap_entry_t() {}

		hmap_entry_t<_Key>* get_link_next() { return link_next; }
			
	public:
		void* data;

	protected:	
		_Key key;
		hmap_entry_t<_Key> *next;
		hmap_entry_t<_Key> *prev;	
		hmap_entry_t<_Key> *link_next;
		hmap_entry_t<_Key> *link_prev;
	};

public:
	hmap_t(uint32_t n)
	{
		RT_ASSERT(n > 0);
		//alloc table
		uint32_t alloc_size = n * sizeof(hmap_entry_t<_K>**);
		table_ = (hmap_entry_t<_K>**)malloc(alloc_size);
		RT_ASSERT(table_ != NULL);	
		memset(table_, 0, alloc_size);

		nslot_ = n;
		size_ = 0;
		link_head_ = NULL;
	}

	~hmap_t() 
	{
		remove_all();		
		free(table_);
	}

	uint32_t size() const { return size_; }
	uint32_t nslot() const { return nslot_; }

	//remove all entries
	void remove_all()
	{
		while(link_head_ != NULL) {
			remove(link_head_);
		}
	}

	//resize the hash table and rehash the items
	void rehash(uint32_t n)
	{
		hmap_entry_t<_K>* p;
		hmap_entry_t<_K>** new_table;
		uint32_t alloc_size;
		uint32_t h;

		RT_ASSERT(n > 0);
	    alloc_size = n * sizeof(hmap_entry_t<_K>*);	
		new_table = (hmap_entry_t<_K>*)malloc(alloc_size);
		RT_ASSERT(new_table != NULL);	
		memset(new_table, 0, alloc_size);
		//rehash nodes
		p = link_head_;	
		while (p != NULL) {
			h = do_hash(p->key) % n;
			if (new_table[h] != NULL) {
				new_table[h]->prev = p;
			}
			p->next = new_table[h];
			new_table[h] = p;	
			p = p->link_next;
		}
		nslot_ = alloc_size;
		free(table_);
		table_ = new_table;
	}

	//string hash. ELF hash
	uint32_t str_hash(const char* s)
	{
		uint32_t hash = 0;
		uint32_t x = 0;
		while(*s) {
			hash = (hash << 4) + (*s++);
			if ((x = hash & 0xF0000000) != 0) {
				hash ^= (x >> 24);
				hash &= ~x;
			}
		}
		return (hash & 0x7FFFFFFF);
	}

	hmap_entry_t<_K>* alloc_entry()
	{
		return new hmap_entry_t<_K>;
	}

	//do_hash cannot write a universal implementation
	//support string and uint only. If any new key type, 
	//write a new overload implementation
	uint32_t do_hash(string_t k)
	{
		//ELF hash
		return str_hash(k.c_str());
	}

	uint32_t do_hash(uint32_t k) {
		return k;
	}

	
	void insert(_K key, void* val)
	{
		uint32_t h = do_hash(key) % nslot_;
		hmap_entry_t<_K> *entry;
		entry = alloc_entry();
		if (table_[h] != NULL) {
			table_[h]->prev = entry;
		}
		entry->next = table_[h];
		table_[h] = entry;	

		attach_link_head(entry);
		++size_;
	}

	void* get_data(_K key)
	{
		hmap_entry_t<_K>* p = find(key);
		if (p == NULL) return NULL;
		return p->data;
	}

	hmap_entry_t<_K>* find(_K key)
	{
		uint32_t h = do_hash(key) % nslot_;
		hmap_entry_t<_K> *entry;
		entry = table_[h];
		while(entry != NULL) {
			if (entry->key == key) return entry;
			entry = entry->next;
		}
		return NULL;
	}

	//special optimized for string_t key.
	//Don't use it for Integer key
	hmap_entry_t<string_t>* find(const char* s)
	{
		uint32_t h = str_hash(s) % nslot_;
		hmap_entry_t<string_t> *entry;
		entry = table_[h];
		while(entry != NULL) {
			if (entry->key.eqaul(s)) return entry;
			entry = entry->next;
		}
		return NULL;
	}

	//remove the entry and get value
	void* remove_get(_K key)
	{
		hmap_entry_t<_K> *entry = find(key);
		void* data;
		if (entry == NULL) return NULL;
		data = entry->data;
		remove(entry);
		return data;
	}

	void remove(hmap_entry_t<_K> *entry)
	{
		void *data;
		if (NULL == entry) return;
		detach(entry);
		data = entry->data;
		//don't refer the entry again after remove
		delete entry;
	}

	void detach(hmap_entry_t<_K> *entry)
	{
		hmap_entry_t<_K>* p;
		hmap_entry_t<_K>* q;
		uint32_t slot;

		if (entry == NULL) return;

		slot = do_hash(entry->key) % nslot_;
		//detach bucket list
	    p = entry->prev;
		q = entry->next;
		if (p != NULL) p->next = q;	
		if (q != NULL) q->prev = p;
		if (table_[slot] == entry) {
			table_[slot] = q;
		}
		entry->next = NULL;
		entry->prev = NULL;

		//detack link list
		detach_link(entry);
	}


	//link list =====================================
	//
	hmap_entry_t<_K>* head() {
		return link_head_;
	}
	
	//insert to link list head
	void attach_link_head(hmap_entry_t<_K> *entry)
	{
		RT_ASSERT(entry != NULL);
		if (link_head_ == NULL) {
			link_head_ = entry;
			return;
		}
	
		entry->link_prev = NULL;	
		entry->link_next = link_head_;
		link_head_->link_prev = entry;
		link_head_ = entry;
	}

	//detach from link list
	void detach_link(hmap_entry_t<_K> *entry)
	{
		hmap_entry_t<_K>* p;
		hmap_entry_t<_K>* q;
		if (NULL == entry) return;

	    p = entry->link_prev;
		q = entry->link_next;
		if (p != NULL) p->link_next = q;	
		if (q != NULL) q->link_prev = p;
		if (entry == link_head_) {
			link_head_ = q;
		}
		entry->link_next = NULL;
		entry->link_prev = NULL;
	}

protected:
	hmap_entry_t<_K> **table_;
	hmap_entry_t<_K> *link_head_;
	uint32_t nslot_;
	uint32_t size_;
};

//Int and String key satisfy almost all situation
//We seldom use other key type in server engine program.
typedef hmap_t<int> HashMapInt;
typedef hmap_t<string_t> HashMapStr;
typedef hmap_t<int>::hmap_entry_t<int> HashEntryInt;
typedef hmap_t<string_t>::hmap_entry_t<string_t> HashEntryStr;

} //namespace

#endif
