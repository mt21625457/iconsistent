#include "iconsistent.h"

#include <math.h>
#include <string.h>

#if defined(_MSC_VER)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else	// defined(_MSC_VER)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

static uint64_t send = 0xEE6B27EB;

//=====================================================================
// IARRAY API                                                  
//=====================================================================


static void HeapAdjust(uint64_t arr[], int i, int length)
{

	int max = i;
	
	int lchild = i * 2 + 1;
	int rchild = i * 2 + 2;
	if (lchild < length && arr[lchild] > arr[max])
	{
		max = lchild;
	}
	if (rchild < length && arr[rchild] > arr[max])
	{
		max = rchild;
	}

	if (max != i)
	{
		uint64_t temp;
		temp = arr[i];
		arr[i] = arr[max];
		arr[max] = temp;
		
		HeapAdjust(arr, max, length);
	}
}


static void HeapSort(uint64_t arr[], int length)
{
	
	for (int i = length / 2 - 1; i >= 0; i--)
	{
		HeapAdjust(arr, i, length);
	}
	
	for (int i = length - 1; i >= 0; i--)
	{
		uint64_t temp;
		temp = arr[i];
		arr[i] = arr[0];
		arr[0] = temp;
		HeapAdjust(arr, 0, i);
	}
}

array * array_init(uint32_t capacity)
{
	array * arr = (array *)calloc(sizeof(array), 1);
	if (!arr) {
		return NULL;
	}

	arr->arr = (uint64_t*)calloc(sizeof(uint64_t), capacity);
	if (!(arr->arr)) {
		free(arr);
		return NULL;
	}

	arr->capacity = capacity;
	arr->size = 0;

	return arr;
}

int array_append(array * arr, uint64_t value)
{
	if (!arr) {
		return -1;
	}

	if (arr->size == arr->capacity) {
		return -2;
	}

	arr->arr[arr->size] = value;
	++(arr->size);

	return 0;
}


int array_del(array * arr, uint32_t opt)
{
	if (!arr || opt >= (arr->size)) {
		return -1;
	}

	for (uint32_t i = opt; i < arr->size; ++i) {
		arr->arr[i] = arr->arr[i + 1];
	}

	--(arr->size);
	return 0;
}

void array_delValue(array * arr, uint64_t value)
{
	for (uint32_t i = 0; i < arr->size; ++i)
	{
		if (value == arr->arr[i]) {
			array_del(arr, i);
		}
	}

}

uint64_t array_get(array * arr, uint32_t opt)
{
	if (!arr || opt >= (arr->size)) {
		return -1;
	}

	return arr->arr[opt];
}

void array_sort(array * arr)
{
	HeapSort(arr->arr, (int)(arr->size));
}

uint32_t array_search(array * arr,uint64_t key)
{
	for (uint32_t i = 0; i < arr->size; ++i) {
		if (arr->arr[i] >= key) {
			return i;
		}
	}

	return arr->size;
}

void array_destory(array ** arr)
{
	if (*arr) {
		if ((*arr)->arr) {
			free((*arr)->arr);
		}
		free(*arr);
		*arr = NULL;
	}
}


//=====================================================================
// ICONSISTENT API                                                  
//=====================================================================

static void		iconsistent_distributePartitions(iconsistent * c);
static int		iconsistent_distributeWithLoad(iconsistent * c, uint32_t partID,uint32_t idx,imap *partitions,imap * loads);
static int		iconsistent_add_menber(iconsistent * c, member *Menber);
static void		iconsistent_delSlice(iconsistent * c, uint64_t value);
static member * iconsistent_getClosestN(iconsistent * c,uint32_t partID, uint32_t count);

iconsistent * iconsistent_init(iconfig config)
{
	if (config.Hasher == NULL) {
		return NULL;
	}

	iconsistent * consistent = (iconsistent*)calloc(sizeof(iconsistent), 1);
	if (!consistent) {
		return NULL;
	}

	consistent->config			= config;
	consistent->Hasher			= config.Hasher;
	consistent->partitionCount	= config.PartitionCount;
	consistent->sortedSet		= array_init(1024);

	consistent->members			= imap_init();
	consistent->ring			= imap_init();
	consistent->loads = NULL;
	consistent->partitions = NULL;
	return consistent;
}

double iconsistent_AverageLoad(iconsistent * c)
{
	double avgload = ((c->partitionCount * 1.0) / imap_size(c->members)) * c->config.load;
	return ceil(avgload);
}


static int iconsistent_add_menber(iconsistent * c, member *Menber)
{
	int size = Menber->len + 4;
	char * buf = (char *) calloc(size,1);
	ikey key;
	for (int i = 0; i < c->config.ReplicationFactor; ++i) {
		sprintf(buf,"%s%d", Menber->string, i);
		uint64_t _key = c->Hasher(buf, size, send);
		key.key = &key;
		key.len = sizeof(uint64_t);
		if (imap_insert(c->ring, key, Menber) < 0) {
			free(buf);
			return -1;
		};
		if (array_append(c->sortedSet, _key) < 0) {
			free(buf);
			return -1;
		}
	}

	free(buf);

	// sort hashes ascendingly
	array_sort(c->sortedSet);

	//Storing member at this map is useful to find backup members of a partition.
	key.key = Menber->string;
	key.len = Menber->len;
    return imap_insert(c->members, key, Menber);
}

static void  iconsistent_distributePartitions(iconsistent * c)
{
	imap * loads = NULL;
	imap * partitions = NULL;

	if (c->loads == NULL) {
		loads = imap_init();
		partitions = imap_init();
	}
	else {
		loads = c->loads;
		partitions = c->partitions;
	}


	for (uint64_t partID = 0; partID < c->partitionCount; ++partID) {
		uint64_t _key = c->Hasher(&partID, sizeof(partID), send);
		uint32_t idx = array_search(c->sortedSet, _key);
		if (idx >= c->sortedSet->size) {
			idx = 0;
		}
		iconsistent_distributeWithLoad(c, (uint32_t)partID, idx, partitions, loads);
	}

	c->partitions = partitions;
	c->loads = loads;
}

static int	iconsistent_distributeWithLoad(iconsistent * c,uint32_t partID, uint32_t idx, imap *partitions, imap * loads)
{
	double avgload = iconsistent_AverageLoad(c);

	uint32_t count = 0;
	ikey key;

	for (;;) {
		if ( (++count) >= (c->sortedSet->size) ) {
			return -1;
		}

		uint64_t i = c->sortedSet->arr[idx];

		key.key = &i;
		key.len = sizeof(uint64_t);
		void * tmp = imap_find(c->ring, key);
		if (tmp == NULL) {
			return -1;
		}

		member * Member = (member *)tmp;
		key.key = Member->string;
		key.len = Member->len;

		tmp = imap_find(c->loads, key);
		if (tmp == NULL) {
			return -1;
		}

		double load = *((double*)tmp);
		if ((load + 1.0) <= avgload) {

			tmp = imap_find(c->loads, key);
			if (tmp != NULL) {
				double * d = (double*)tmp;
				++d;
			}
			else {
				return -1;
			}

			member * m = (member*)calloc(sizeof(member),1);
			m->string = (char*)calloc(Member->len, 1);
			memcpy(m->string, Member->string, Member->len);
			key.key = &partID;
			key.len = sizeof(uint32_t);
			imap_insert(c->partitions, key,m);
			
			return 0;
		}
		++idx;
		if (idx >= c->sortedSet->size) {
			idx = 0;
		}
	}
}

int iconsistent_add(iconsistent * c,member * Menber)
{
	ikey key;
	key.key = Menber->string;
	key.len = Menber->len;

	void * value = imap_find(c->members, key);
	if (value != NULL) {
		return -1;
	}

	iconsistent_add_menber(c, Menber);
	iconsistent_distributePartitions(c);
	return 0;
}

member * iconsistent_getmembers(iconsistent * c)
{
	member * tmp = (member*)calloc(c->members->size, 1);
	memcpy(tmp, c->members, c->members->size);
	return tmp;
}

uint32_t iconsistent_FindPartitionID(iconsistent * c, void * key, int len)
{
	uint64_t hkey = c->Hasher(key, len, send);
	return (uint32_t)(hkey % (c->partitionCount));
}

member * iconsistent_GetPartitionOwner(iconsistent * c, uint32_t partID)
{
	ikey key;
	key.key = &partID;
	key.len = sizeof(uint32_t);

	void * tmp = imap_find(c->partitions, key);
	if (tmp == NULL) {
		return NULL;
	}

	member *Member = (member*)tmp;
	member * m = (member*)calloc(Member->len, 1);

	m->len = Member->len;
	memcpy(m->string, Member->string, Member->len);
	
	return m;
}

member * iconsistent_locatekey(iconsistent * c, void *key, int len)
{
	uint32_t partID = iconsistent_FindPartitionID(c, key, len);
	return iconsistent_GetPartitionOwner(c, partID);
}


void  iconsistent_dismember(member ** Member)
{
	if (*Member) {
		if ((*Member)->string != NULL) {
			free((*Member)->string);
		}
		free(*Member);
		*Member = NULL;
	}
}

static void iconsistent_delSlice(iconsistent * c,uint64_t value)
{
	array_delValue(c->sortedSet, value);
}

int iconsistent_remove(iconsistent * c, member *Member)
{
	ikey _key;
	_key.key = Member->string;
	_key.len = Member->len;

	void * tmp = imap_find(c->members, _key);
	if (tmp == NULL) {
		return -1;
	}

	int size = Member->len + 4;
	char * buf = (char *)calloc(size, 1);

	for (int i = 0; i < c->config.ReplicationFactor; ++i) {
		sprintf(buf,"%s%d", Member->string, i);
		uint64_t key = c->Hasher(buf, size, send);
		_key.key = &key;
		_key.len = sizeof(uint64_t);
		tmp = imap_del(c->ring, _key);
		iconsistent_dismember( ((member**)&tmp) );
		iconsistent_delSlice(c,key);
	}

	_key.key = Member->string;
	_key.len = Member->len;

	tmp = imap_del(c->members, _key);
	if (tmp == NULL) {
		free(buf);
		return -1;
	}

	if (imap_size(c->members) == 0) {
		return 0;
	}

	iconsistent_distributePartitions(c);
	return 0;
}

imap * iconsistent_loadDistribution(iconsistent * c)
{
	return NULL;
}

static member * iconsistent_getClosestN(iconsistent * c,uint32_t partID, uint32_t count)
{
	if (count > c->members->size) {
		return NULL;
	}

	uint64_t ownerKey = 0;
	member * m = iconsistent_GetPartitionOwner(c, partID);
	if (m == NULL) {
		return NULL;
	}



}