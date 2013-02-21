/*
Date:2013.2.2
Auther:goalworld

*/
#ifndef _WOD__HASH_MAP_
#define _WOD__HASH_MAP_
struct wod_hashmap_entry
{
	struct{
		const void *key;
		const void *value;
	}kv;
	unsigned tkey;
	struct wod_hashmap_entry *next;
};
struct wod_hashmap_table
{
	struct wod_hashmap_entry** etys;
	unsigned used;
	unsigned cap;
	unsigned hashkey;
};
struct wod_hash_map_type
{
	unsigned 	(*hash_func)(void *env,const void *key);
	int 		(*key_compare)(void *env,const void *key1,const void *key2);
	void *		(*key_clone)(void *env,const void *key);
	void *		(*value_clone)(void *env,const void *value);
	void 		(*key_destroy)(void *env,const void *key);
	void 		(*value_destroy)(void *env,const void *value);
};
 struct wod_hash_map
{
	struct  wod_hash_map_type ktype;
	void * ktenv;
	struct wod_hashmap_table** tbs;
	unsigned tblen;
	unsigned tbcap;
} ;
struct wod_hash_map* wod_hashmap_new( struct wod_hash_map_type hmt,void *hmtenv);
void 	wod_hashmap_delete(struct wod_hash_map* hm);
int 	wod_hashmap_insert(struct wod_hash_map* hm,const void *key,const void *value);
void * 	wod_hashmap_query(struct wod_hash_map*hm,const void *key);
void * 	wod_hashmap_remove(struct wod_hash_map*hm,const void *key);
unsigned wod_hashmap_size(struct wod_hash_map*hm);
#endif
