#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include "../include/wod_hashmap.h"
#include <assert.h>
#define MAX_LENGTH 1024*1024

static struct wod_hashmap_table *  _hmt_new(unsigned key,unsigned sz);
static void _hmt_delete(struct wod_hash_map * hm,struct wod_hashmap_table * hmt);
static void  _hmt_rehash(struct wod_hash_map * hm,int index);
static void _hmt_detach(struct wod_hash_map *hm,int lindex,int rindex);
static struct wod_hashmap_entry *  _hmt_remove(struct wod_hash_map * hm,int lindex,unsigned key);
static struct wod_hashmap_entry *  _hmt_query(struct wod_hash_map * hm,int lindex,unsigned key);
static int _hmt_insert(struct wod_hash_map * hm,int index,unsigned key ,struct wod_hashmap_entry * entry);
static void _hmt_need_detach(struct wod_hash_map  *hm,unsigned index);




struct wod_hash_map *
wod_hashmap_new(struct wod_hash_map_type hmt,void *hmtenv)
{
	struct wod_hash_map * hm = malloc(sizeof(struct wod_hash_map));
	hm->ktenv = hmtenv;
	hm->ktype = hmt;
	hm->tblen = 4;
	hm->tbcap = 4;
	hm->tbs = malloc(sizeof(void*)*(hm->tblen));
	unsigned i = 0;
	unsigned df = 0xffffffff/hm->tblen;
	for(;i<hm->tblen;i++){
		hm->tbs[i] = _hmt_new((i+1)*df,32);
	}
	return hm;
}
static inline unsigned
 first_hash_func(struct wod_hash_map * hm,const void *inkey){
 	unsigned key = hm->ktype.hash_func(hm->ktenv,inkey);
 	//Tomas Wang
	key += ~(key << 15);
    key ^=  (key >> 10);
    key +=  (key << 3);
    key ^=  (key >> 6);
    key += ~(key << 11);
    key ^=  (key >> 16);
    return key;
 }

void 	
wod_hashmap_delete(struct wod_hash_map * hm)
{
	int i = 0;
	for(;i<hm->tblen;i++){
		_hmt_delete(hm,hm->tbs[i]);
	}
	free(hm->tbs);
	free(hm);
}
unsigned 
wod_hashmap_size(struct wod_hash_map *hm)
{
	unsigned sz = 0;
	int i=0;
	for(;i<hm->tblen;i++){
		sz+=hm->tbs[i]->used;
	}
	return sz;
}
int 	
wod_hashmap_insert(struct wod_hash_map * hm,const void *key,const void *value)
{
	struct wod_hashmap_entry * entry = malloc(sizeof(struct wod_hashmap_entry));
	entry->kv.key =   hm->ktype.key_clone   ? hm->ktype.key_clone(hm->ktenv,key):key;
	entry->kv.value = hm->ktype.value_clone ? hm->ktype.value_clone(hm->ktenv,value):value;
	entry->tkey = first_hash_func(hm,key);
	entry->next = NULL;
	int i=0;
	for(;i<hm->tblen;i++){
		if(entry->tkey <= hm->tbs[i]->hashkey){
			_hmt_insert(hm,i,entry->tkey,entry);
			break;
		}
	}
	return 0;
}
void *
wod_hashmap_query(struct wod_hash_map *hm,const void *key)
{
	struct wod_hashmap_entry * entry;
	unsigned tkey =first_hash_func(hm,key);
	int i=0;
	for(;i<hm->tblen;i++){
		if(tkey <= hm->tbs[i]->hashkey){
			entry = _hmt_query(hm,i,tkey);
			break;
		}
	}
	if(entry){
		return (void *)(hm->ktype.value_clone ? hm->ktype.value_clone(hm->ktenv,entry->kv.value):entry->kv.value);
	}
	return NULL;
}
void *
wod_hashmap_remove(struct wod_hash_map *hm,const void *key)
{
	struct wod_hashmap_entry * entry;
	unsigned tkey = first_hash_func(hm,key);
	int i=0;
	void *value = NULL;
	for(;i<hm->tblen;i++){
		if(tkey <= hm->tbs[i]->hashkey){
			entry = _hmt_remove(hm,i,tkey);
			if(entry){
				value = (void *)(hm->ktype.value_clone ? hm->ktype.value_clone(hm->ktenv,entry->kv.value):entry->kv.value);
				free(entry);
			}
			break;
		}
	}
	return value;
}
unsigned
hmtHashFunc(struct wod_hashmap_table *hmt,unsigned key)
{
	return (key * 7)&(hmt->cap-1);
}

static struct wod_hashmap_table *
_hmt_new(unsigned key,unsigned sz)
{
	struct wod_hashmap_table * hmt = malloc(sizeof(struct wod_hashmap_table));
	hmt->etys = malloc(sz*sizeof(void *));
	memset(hmt->etys,0,sz*sizeof(void *));
	hmt->used = 0;
	hmt->cap = sz;
	hmt->hashkey = key;
	return hmt;
}
static void
_hmt_delete(struct wod_hash_map *hm,struct wod_hashmap_table * hmt)
{
	int i=0;
	struct wod_hashmap_entry *tmp,*cut;
	for(;i<hmt->cap;i++){
		cut = hmt->etys[i];
		while(cut){
			tmp = cut->next;
			if(hm->ktype.key_destroy)hm->ktype.key_destroy(hm->ktenv,cut->kv.key);
			if(hm->ktype.key_destroy)hm->ktype.value_destroy(hm->ktenv,cut->kv.value);
			free(cut);
			cut = tmp;
		}
	}
	free(hmt->etys);
	free(hmt);
}
static int
_hmt_insert(struct wod_hash_map * hm,int index,unsigned key ,struct wod_hashmap_entry * entry)
{
	struct wod_hashmap_table *hmt = hm->tbs[index];
	
	int i = hmtHashFunc(hmt,key);
	entry->next = hmt->etys[i];
	hmt->etys[i] = entry;
	hmt->used++;
	if(hmt->cap == hmt->used){
		if( hmt->cap <= MAX_LENGTH/2 ){
			_hmt_rehash(hm,index);
		}else{
			_hmt_need_detach(hm,index);
		}
		
	}
	return 0;
}
static struct wod_hashmap_entry *
_hmt_query(struct wod_hash_map * hm,int index,unsigned key)
{
	struct wod_hashmap_table * hmt = hm->tbs[index];
	struct wod_hashmap_entry * cut;
	unsigned i = hmtHashFunc(hmt,key);
	cut = hmt->etys[i];
	while(cut){
		if( cut->tkey == key ){
			return cut;
		}
		cut = cut->next;
	}
	return NULL;
}
static struct wod_hashmap_entry *
_hmt_remove(struct wod_hash_map * hm,int index,unsigned key)
{
	struct wod_hashmap_table * hmt = hm->tbs[index];
	struct wod_hashmap_entry * cut,*pre;
	int i = hmtHashFunc(hmt,key);
	cut = hmt->etys[i]; pre = NULL;
	while(cut){
		if( cut->tkey == key ){
			if(pre) pre->next = cut->next;
			else 	hmt->etys[i]=cut->next;
			hmt->used--;
			return cut;
		}
		pre = cut;
		cut = cut->next;
	}
	return NULL;
}
static void
_hmt_need_detach(struct wod_hash_map  *hm,unsigned index)
{
	if(hm->tblen == hm->tbcap){
		hm->tbcap*=2;
		hm->tbs = realloc(hm->tbs,hm->tbcap * sizeof(void *));
	}
	memmove(hm->tbs+index+1,hm->tbs+index,(hm->tblen-index)*sizeof(void *));
	hm->tblen++;
	unsigned pre = 0;
	if(index != 0) pre =hm->tbs[index-1]->hashkey;
	hm->tbs[index] = _hmt_new((hm->tbs[index+1]->hashkey + pre)/2,hm->tbs[index+1]->cap/2);
	_hmt_detach(hm,index,index+1);
}
static void
_hmt_detach(struct wod_hash_map *hm,int lindex,int rindex)
{
	struct wod_hashmap_table * hmtLft = hm->tbs[lindex];
	struct wod_hashmap_table * hmtRht = hm->tbs[rindex];
	struct wod_hashmap_entry * cut,*pre,*tmp;
	int i;
	for(i=0; i<hmtRht->cap;i++){
		cut = hmtRht->etys[i]; pre = NULL;
		while(cut){
			if( cut->tkey <= hmtLft->hashkey ){
				tmp = cut;
				cut = cut->next;
				_hmt_insert(hm,lindex,tmp->tkey,tmp);
				if(pre) pre->next = cut;
				else 	hmtRht->etys[i]=cut;
				continue;
			}
			pre = cut;
			cut = cut->next;
		}
	}
}

static void  
_hmt_rehash(struct wod_hash_map * hm,int index)
{
	struct wod_hashmap_entry * cut,*tmp;
	struct wod_hashmap_table * hmt = hm->tbs[index];
	hm->tbs[index] = _hmt_new(hmt->hashkey,hmt->cap*2);
	int i;
	for(i=0; i<hmt->cap;i++){
		cut = hmt->etys[i];
		while(cut){
			tmp = cut;
			cut = cut->next;
			_hmt_insert(hm,index,tmp->tkey,tmp);
		}
	}
	free(hmt->etys);
	free(hmt);
}
