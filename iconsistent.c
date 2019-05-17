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

/*
	@param arr 待调整的数组
	@param i 待调整的结点的下标
	@param length 数组的长度
*/
static void HeapAdjust(uint64_t arr[], int i, int length)
{
	// 调整i位置的结点
	// 先保存当前结点的下标
	int max = i;
	// 当前结点左右孩子结点的下标
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
	// 若i处的值比其左右孩子结点的值小，就将其和最大值进行交换
	if (max != i)
	{
		uint64_t temp;
		temp = arr[i];
		arr[i] = arr[max];
		arr[max] = temp;
		// 递归
		HeapAdjust(arr, max, length);
	}
}

// 堆排序
static void HeapSort(uint64_t arr[], int length)
{
	// 初始化堆
	// length / 2 - 1是二叉树中最后一个非叶子结点的序号
	for (int i = length / 2 - 1; i >= 0; i--)
	{
		HeapAdjust(arr, i, length);
	}
	// 交换堆顶元素和最后一个元素
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

	return 0;
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

	consistent->loads			= imap_init();
	consistent->members			= imap_init();
	consistent->partitions		= imap_init();
	consistent->ring			= imap_init();

	return consistent;
}

double iconsistent_AverageLoad(iconsistent * c)
{
	double avgload = ((c->partitionCount * 1.0) / imap_size(c->members)) * c->config.load;
	return ceil(avgload);
}


static int iconsistent_add_menber(iconsistent * c, menber *Menber)
{
	int size = Menber->len + 4;
	char * buf = (char *) calloc(size,1);
	ikey key;
	for (int i = 0; i < c->config.ReplicationFactor; ++i) {
		sprintf_s(buf, size,"%s%d", Menber->string, i);
		uint64_t _key = c->Hasher(buf, size, send);
		key.key = &key;
		key.len = sizeof(uint64_t);
		if (imap_insert(c->ring, key, Menber) < 0) {
			free(buf);
			return -1;
		};




	}
}

int iconsistent_add(iconsistent * c,menber Menber)
{
	ikey key;
	key.key = Menber.string;
	key.len = Menber.len;

	void * value = imap_find(c->members, key);
	if (value != NULL) {
		return -1;
	}

	//iconsistent_add_menber(c, Menber);


}