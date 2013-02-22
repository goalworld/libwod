/*
Date:2013.2.2
Auther:goalworld

*/
#ifndef _WOD__HASH_MAP_
#define _WOD__HASH_MAP_
struct wodHashMapEntry
{
	struct{
		const void *key;
		const void *value;
	}kv;
	unsigned tkey;
	struct wodHashMapEntry *next;
};
struct wodHashMapTable
{
	struct wodHashMapEntry** etys;
	unsigned used;
	unsigned cap;
	unsigned hashkey;
};
struct wodHashMapType
{
	unsigned 	(*hashFunc)(void *env,const void *key);
	int 		(*keyCampre)(void *env,const void *key1,const void *key2);
	void *		(*keyClone)(void *env,const void *key);
	void *		(*valueClone)(void *env,const void *value);
	void 		(*keyDestroy)(void *env,const void *key);
	void 		(*valueDestroy)(void *env,const void *value);
};
struct wodHashMap
{
	struct wodHashMapType ktype;
	void * ktenv;
	struct wodHashMapTable** tbs;
	unsigned tblen;
	unsigned tbcap;
};
struct wodHashMap * wodHashMapNew(struct wodHashMapType hmt,void *hmtenv);
void 	wodHashMapDelete(struct wodHashMap * hm);
int 	wodHashMapInsert(struct wodHashMap * hm,const void *key,const void *value);
void * 	wodHashMapQuery(struct wodHashMap *hm,const void *key);
void * 	wodHashMapRemove(struct wodHashMap *hm,const void *key);
unsigned wodHashMapSize(struct wodHashMap *hm);
#endif
