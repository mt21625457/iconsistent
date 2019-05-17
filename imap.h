#ifndef __IMAP_H__
#define __IMAP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint64_t KEY_TYPE;


//=====================================================================
// ISTACK DEFINITION                                                  
//=====================================================================
#ifndef __ISTACK_DEF__
#define __ISTACK_DEF__

typedef struct _istack_node istack_node;
typedef struct _istack_node{
	void * value;
	istack_node *	next;
	istack_node *	pre;
}istack_node;

typedef struct _istack {
	istack_node  * top;
	istack_node  * bot;
	uint32_t size;
}istack;

//=====================================================================
// ISTACK API                                           
//=====================================================================


#ifdef __cplusplus
extern "C" {
#endif

istack * istack_init();

int istack_push(istack * stack,void *value);

void * istack_pop(istack * stack);

int  istack_isEnpty(istack * stack);

void istack_destory(istack ** stack);

#ifdef __cplusplus
}
#endif


#endif

//=====================================================================
// IMAP DEFINITION                                                  
//=====================================================================


typedef struct _rbtree rbtree;

typedef struct _imap {
    rbtree * root; 
    uint32_t size;
}imap;

typedef struct _ikey {
    void * key;
    int len;
}ikey ;


//=====================================================================
// IMAP API                      
//=====================================================================

#ifdef __cplusplus
extern "C" {
#endif
//init map
imap * imap_init();

//insert map
int imap_insert(imap * map, ikey  key , void * value);

//find map
void * imap_find(imap * map,ikey key);

//delete map
void * imap_del(imap * map,ikey key);

//delete map mini
void * imap_delete_mini(imap * map);

//±È¿˙ ≤‚ ‘
void imap_traversal(imap *map);

//non - recursion
void imap_print(imap*map);

//return imap size
uint32_t imap_size(imap*map);

//close map
void imap_destory(imap **map);

#ifdef __cplusplus
}
#endif


#endif //__IMAP_H__