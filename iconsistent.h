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

void array_sort(array * arr);

void array_destory(array ** arr);


//=====================================================================
// ICONSISTENT DEFINITION                                                  
//=====================================================================



typedef struct _member {
	char * string;
	int len;
}menber;

typedef struct _iconfig {
	uint64_t(*Hasher)(const void *, int, uint64_t);     //��ϣ���� 
	int PartitionCount;                                     //��Կ�ֲ��ڷ���֮�䡣�����Ǿ��ȷֲ���Կ�ĺ÷����������̫�࣬ѡ��һ�����PartitionCount��
	int ReplicationFactor;                                  //��Ա��һ�µĹ�ϣ���ϸ��ơ�������ֱ�ʾһ����Ա�ڻ��ϸ����˶��ٴΡ�
	double load;                                            //���ڼ���ƽ������
}iconfig;

typedef struct _iconsistent {
	iconfig config;
	uint64_t(*Hasher)(const void *, int, uint64_t);
	uint64_t * sortedSet;
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

iconsistent * iconsistent_init(iconfig config);

double iconsistent_AverageLoad(iconsistent * c);

int iconsistent_add(iconsistent * c,menber *Menber);

#ifdef __cplusplus
}
#endif






#endif
