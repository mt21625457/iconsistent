#ifndef __ICONSISTENT_H__
#define __ICONSISTENT_H__

#include "imap.h"

typedef unsigned long long  uint64_t;
typedef unsigned int		uint32_t;
typedef unsigned char		uchar_t;

//=====================================================================
// IARRAY DEFINITION                                                  
//=====================================================================
typedef struct _array {
	uint64_t * arr;
	uint32_t capacity;
	uint32_t size;
}array;

//=====================================================================
// IARRAY API                                                  
//=====================================================================

array * array_init(uint32_t capacity);

int array_append(array * arr, uint64_t value);

int array_del(array * arr, uint32_t opt);

void array_delValue(array * arr, uint64_t value);

void array_sort(array * arr);

uint32_t array_search(array * arr, uint64_t key);

void array_destory(array ** arr);


//=====================================================================
// ICONSISTENT DEFINITION                                                  
//=====================================================================



typedef struct _member {
	char * string;
	int len;
}member;

typedef struct _iconfig {
	uint64_t(*Hasher)(const void *, int, uint64_t);         //哈希函数 
	int PartitionCount;                                     //密钥分布在分区之间。质数是均匀分布密钥的好方法。如果键太多，选择一个大的PartitionCount。
	int ReplicationFactor;                                  //成员在一致的哈希环上复制。这个数字表示一个成员在环上复制了多少次。
	double load;                                            //用于计算平均荷载
}iconfig;

typedef struct _iconsistent {
	iconfig config;
	uint64_t(*Hasher)(const void *, int, uint64_t);
	array * sortedSet;
	uint64_t partitionCount;
	imap * loads;
	imap * members;
	imap * partitions;
	imap * ring; 
}iconsistent;

//=====================================================================
// ICONSISTENT API                                                  
//=====================================================================

#ifdef __cplusplus
extern "C" {
#endif

// GetMembers returns a thread-safe copy of members.
iconsistent * iconsistent_init(iconfig config);

// AverageLoad exposes the current average load.
double iconsistent_AverageLoad(iconsistent * c);

// Add adds a new member to the consistent hash circle.
int iconsistent_add(iconsistent * c, member *Menber);

// GetMembers returns a thread-safe copy of members.
member * iconsistent_getmembers(iconsistent * c);

// FindPartitionID returns partition id for given key.
int iconsistent_FindPartitionID(iconsistent * c,void * key, int len);

// GetPartitionOwner returns the owner of the given partition.
member * iconsistent_GetPartitionOwner(iconsistent * c, uint32_t partID);

// LocateKey finds a home for given key
member * iconsistent_locatekey(iconsistent * c, void *key, int len);

// 
imap * iconsistent_loadDistribution(iconsistent * c);

// Remove removes a member from the consistent hash circle.
int iconsistent_remove(iconsistent * c, member *Menber);

//close member
void  iconsistent_dismember(member ** Member);

#ifdef __cplusplus
}
#endif



#endif
