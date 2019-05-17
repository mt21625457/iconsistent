#include  "imap.h"


//=====================================================================
// IHASH API                                                 
//=====================================================================


#if defined(_MSC_VER)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else	// defined(_MSC_VER)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

static uint64_t send = 0xEE6B27EB;

static uint64_t MurmurHash64A_x64(const void * key, int len, uint64_t seed)
{
	const uint64_t m = BIG_CONSTANT(0xc6a4a7935bd1e995);
	const int r = 47;

	uint64_t h = seed ^ (len * m);

	const uint64_t * data = (const uint64_t *)key;
	const uint64_t * end = data + (len / 8);

	while (data != end)
	{
		uint64_t k = *data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	const unsigned char * data2 = (const unsigned char*)data;

	switch (len & 7)
	{
	case 7: h ^= (uint64_t)(data2[6]) << 48;
	case 6: h ^= (uint64_t)(data2[5]) << 40;
	case 5: h ^= (uint64_t)(data2[4]) << 32;
	case 4: h ^= (uint64_t)(data2[3]) << 24;
	case 3: h ^= (uint64_t)(data2[2]) << 16;
	case 2: h ^= (uint64_t)(data2[1]) << 8;
	case 1: h ^= (uint64_t)(data2[0]);
		h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}

static uint64_t MurmurHash64B_x86(const void * key, int len, uint64_t seed)
{
	const uint32_t m = 0x5bd1e995;
	const int r = 24;

	uint32_t h1 = ((uint32_t)seed) ^ len;
	uint32_t h2 = (uint32_t)(seed >> 32);

	const uint32_t * data = (const uint32_t *)key;

	while (len >= 8)
	{
		uint32_t k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;

		uint32_t k2 = *data++;
		k2 *= m; k2 ^= k2 >> r; k2 *= m;
		h2 *= m; h2 ^= k2;
		len -= 4;
	}

	if (len >= 4)
	{
		uint32_t k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;
	}

	switch (len)
	{
	case 3: h2 ^= ((unsigned char*)data)[2] << 16;
	case 2: h2 ^= ((unsigned char*)data)[1] << 8;
	case 1: h2 ^= ((unsigned char*)data)[0];
		h2 *= m;
	};

	h1 ^= h2 >> 18; h1 *= m;
	h2 ^= h1 >> 22; h2 *= m;
	h1 ^= h2 >> 17; h1 *= m;
	h2 ^= h1 >> 19; h2 *= m;

	uint64_t h = h1;

	h = (h << 32) | h2;

	return h;
}


//=====================================================================
// IRBTREE DEFINITION                                                  
//=====================================================================

#define RED				1
#define BLACK 			2

typedef struct _rbtree_node {
	unsigned char color;
	struct _rbtree_node *right;
	struct _rbtree_node *left;
	struct _rbtree_node *parent;

	KEY_TYPE key;
	void *value;
} rbtree_node;


typedef struct _rbtree {
	rbtree_node *root;
	rbtree_node *nil;
} rbtree;


//=====================================================================
// IRBTREE API                                           
//=====================================================================
static rbtree_node *rbtree_mini(rbtree *T, rbtree_node *x) {
	while (x->left != T->nil) {
		x = x->left;
	}
	return x;
}


static rbtree_node *rbtree_maxi(rbtree *T, rbtree_node *x) {
	while (x->right != T->nil) {
		x = x->right;
	}
	return x;
}

static rbtree_node *rbtree_successor(rbtree *T, rbtree_node *x) {
	rbtree_node *y = x->parent;

	if (x->right != T->nil) {
		return rbtree_mini(T, x->right);
	}

	while ((y != T->nil) && (x == y->right)) {
		x = y;
		y = y->parent;
	}
	return y;
}


static void rbtree_left_rotate(rbtree *T, rbtree_node *x) {

	rbtree_node *y = x->right;

	x->right = y->left; //1 1
	if (y->left != T->nil) { //1 2
		y->left->parent = x;
	}

	y->parent = x->parent; //1 3
	if (x->parent == T->nil) { //1 4
		T->root = y;
	} else if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}

	y->left = x; //1 5
	x->parent = y; //1 6
}


static void rbtree_right_rotate(rbtree *T, rbtree_node *y) {

	rbtree_node *x = y->left;

	y->left = x->right;
	if (x->right != T->nil) {
		x->right->parent = y;
	}

	x->parent = y->parent;
	if (y->parent == T->nil) {
		T->root = x;
	} else if (y == y->parent->right) {
		y->parent->right = x;
	} else {
		y->parent->left = x;
	}

	x->right = y;
	y->parent = x;
}

static void rbtree_insert_fixup(rbtree *T, rbtree_node *z) {

	while (z->parent->color == RED) { //z ---> RED
		if (z->parent == z->parent->parent->left) {
			rbtree_node *y = z->parent->parent->right;
			if (y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;

				z = z->parent->parent; //z --> RED
			} else {

				if (z == z->parent->right) {
					z = z->parent;
					rbtree_left_rotate(T, z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_right_rotate(T, z->parent->parent);
			}
		}else {
			rbtree_node *y = z->parent->parent->left;
			if (y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;

				z = z->parent->parent; //z --> RED
			} else {
				if (z == z->parent->left) {
					z = z->parent;
					rbtree_right_rotate(T, z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_left_rotate(T, z->parent->parent);
			}
		}
		
	}

	T->root->color = BLACK;
}


static int rbtree_insert(rbtree *T, rbtree_node *z) {

	rbtree_node *y = T->nil;
	rbtree_node *x = T->root;

	while (x != T->nil) {
		y = x;
		if (z->key < x->key) {
			x = x->left;
		} else if (z->key > x->key) {
			x = x->right;
		} else { //Exist
			return -1;
		}
	}

	z->parent = y;
	if (y == T->nil) {
		T->root = z;
	} else if (z->key < y->key) {
		y->left = z;
	} else {
		y->right = z;
	}

	z->left = T->nil;
	z->right = T->nil;
	z->color = RED;

	rbtree_insert_fixup(T, z);
	return 0;
}

static void rbtree_delete_fixup(rbtree *T, rbtree_node *x) {

	while ((x != T->root) && (x->color == BLACK)) {
		if (x == x->parent->left) {

			rbtree_node *w= x->parent->right;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;

				rbtree_left_rotate(T, x->parent);
				w = x->parent->right;
			}

			if ((w->left->color == BLACK) && (w->right->color == BLACK)) {
				w->color = RED;
				x = x->parent;
			} else {

				if (w->right->color == BLACK) {
					w->left->color = BLACK;
					w->color = RED;
					rbtree_right_rotate(T, w);
					w = x->parent->right;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				rbtree_left_rotate(T, x->parent);

				x = T->root;
			}

		} else {

			rbtree_node *w = x->parent->left;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				rbtree_right_rotate(T, x->parent);
				w = x->parent->left;
			}

			if ((w->left->color == BLACK) && (w->right->color == BLACK)) {
				w->color = RED;
				x = x->parent;
			} else {

				if (w->left->color == BLACK) {
					w->right->color = BLACK;
					w->color = RED;
					rbtree_left_rotate(T, w);
					w = x->parent->left;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				rbtree_right_rotate(T, x->parent);

				x = T->root;
			}

		}
	}

	x->color = BLACK;
}

static rbtree_node *rbtree_delete(rbtree *T, rbtree_node *z) {

	rbtree_node *y = T->nil;
	rbtree_node *x = T->nil;

	if ((z->left == T->nil) || (z->right == T->nil)) {
		y = z;
	} else {
		y = rbtree_successor(T, z);
	}

	if (y->left != T->nil) {
		x = y->left;
	} else if (y->right != T->nil) {
		x = y->right;
	}

	x->parent = y->parent;
	if (y->parent == T->nil) {
		T->root = x;
	} else if (y == y->parent->left) {
		y->parent->left = x;
	} else {
		y->parent->right = x;
	}

	if (y != z) {
		z->key = y->key;
		z->value = y->value;
	}

	if (y->color == BLACK) {
		rbtree_delete_fixup(T, x);
	}

	return y;
}

static rbtree_node *rbtree_search(rbtree *T, KEY_TYPE key) {

	rbtree_node *node = T->root;
	while (node != T->nil) {
		if (key < node->key) {
			node = node->left;
		} else if (key > node->key) {
			node = node->right;
		} else {
			return node;
		}	
	}
	return T->nil;
}


void rbtree_traversal(rbtree *T, rbtree_node *node) {
	if (node != T->nil) {
		rbtree_traversal(T, node->left);
		printf("key:%lld, value:%d,color:%d \n", node->key, *((int*)node->value), node->color);
		rbtree_traversal(T, node->right);
	}
}


//=====================================================================
// ISTACK API                                           
//=====================================================================

istack * istack_init()
{
	istack * stack = (istack*)calloc(sizeof(istack),1);
	if (!stack) {
		return NULL;
	}

	stack->bot = NULL;
	stack->top = NULL;
	stack->size = 0;

	return stack;
}

int istack_push(istack * stack, void *value)
{
	if (stack == NULL) {
		return -1;
	}

	istack_node * node = (istack_node*)calloc(sizeof(istack_node), 1);
	if (node == NULL) {
		return -1;
	}

	node->value = value;
	node->next = NULL;
	
	if (stack->top == NULL && stack->bot == NULL) {	
		node->pre = NULL;
		stack->top = node;
		stack->bot = node;
		++(stack->size);
		return 0;
	}
	else {
		node->pre = stack->top;
		stack->top->next = node;
		stack->top = node;
		++(stack->size);
		return 0;
	}
}


void * istack_pop(istack * stack)
{
	if (stack == NULL || stack->size == 0) {
		return NULL;
	}

	istack_node * node = stack->top;
	void * tmp = node->value;
	

	if (stack->size == 1 ) {	
		stack->bot = NULL;
		stack->top = NULL;	
	}
	else {
		stack->top = node->pre;
		stack->top->next = NULL;
	}

	free(node);
	--(stack->size);

	return tmp;
}

int istack_isEnpty(istack * stack)
{
	if (stack) {
		if (stack->size == 0) {
			return 0;
		}
		else {
			return 1;
		}
	}
	return -1;
}

void istack_destory(istack ** stack)
{
	if (*stack) {
		free(*stack);
		*stack = NULL;
	}
}


//---------------------------------------------------------------------
// IMAP API                      
//---------------------------------------------------------------------
imap * imap_init()
{
    imap * map = (imap*)calloc(sizeof(struct _imap),1);
    if(!map) {
        return NULL;
    }

	map->root = NULL;
    map->size = 0;

	map->root = (rbtree*)malloc(sizeof(rbtree));
	if (map->root == NULL) {
		free(map);
		return NULL;
	}

	map->root->nil = (rbtree_node*)calloc(sizeof(rbtree_node), 1);
	map->root->nil->color = BLACK;
	map->root->root = map->root->nil;

    return map;
}

int imap_insert(imap * map, ikey  key , void * value)
{
	if (map == NULL) {
		return -1;
	}

	rbtree_node *node = (rbtree_node*)calloc(sizeof(rbtree_node), 1);
    node->key = MurmurHash64A_x64(key.key,key.len,send);
    node->value = value;

    int ret = rbtree_insert(map->root,node);
	if (ret < 0) {
		free(node);
		return -1;
	}

    ++(map->size);
    return 0;
}

void * imap_find(imap * map,ikey key)
{
	if (map == NULL ||  map->size == 0) {
		return NULL;
	}

    KEY_TYPE key_tmp = MurmurHash64A_x64(key.key,key.len,send);
    rbtree_node *node = rbtree_search(map->root, key_tmp);
    if(!node) {
        return NULL;
    }

    return node->value;
}

void * imap_del(imap * map, ikey key)
{
	if (map == NULL || map->size == 0) {
		return NULL;
	}

    KEY_TYPE key_tmp = MurmurHash64A_x64(key.key,key.len,send);

    rbtree_node *node = rbtree_search(map->root, key_tmp);
    if(!node) {
        return NULL;
    }

    rbtree_node *cur = rbtree_delete(map->root, node);
	if (!cur) {
		return NULL;
	}

    void * tmp = cur->value;
	free(cur);
	--(map->size);

    return tmp;    
}

void * imap_delete_mini(imap * map)
{
	rbtree_node * node = rbtree_mini(map->root, map->root->root);
	rbtree_node *cur = rbtree_delete(map->root, node);
	if (!cur) {
		return NULL;
	}
	void * tmp = cur->value;
	free(cur);
	--(map->size);

	return tmp;
}

void  imap_traversal(imap *map)
{
	if (map == NULL ||  map->size == 0) {
		return;
	}

	rbtree_traversal(map->root, map->root->root);
}

void imap_print(imap*map)
{



}

uint32_t imap_size(imap*map)
{
	if (map == NULL) {
		return 0;
	}

	return map->size;
}

void imap_destory(imap **map)
{
	if (*map) {
		if ((*map)->root) {
			free((*map)->root);
		}
		free((*map));
		*map = NULL;
	}
}

/*
int test() {

	int keyArray[20] = {24,25,13,35,23, 26,67,47,38,98, 20,19,17,49,12, 21,9,18,14,15};

	rbtree *T = (rbtree *)malloc(sizeof(rbtree));
	if (T == NULL) {
		printf("malloc failed\n");
		return -1;
	}
	
	T->nil = (rbtree_node*)malloc(sizeof(rbtree_node));
	T->nil->color = BLACK;
	T->root = T->nil;

	rbtree_node *node = T->nil;
	int i = 0;
	for (i = 0;i < 20;i ++) {
		node = (rbtree_node*)malloc(sizeof(rbtree_node));
		node->key = keyArray[i];
		node->value = NULL;

		rbtree_insert(T, node);
		
	}

	rbtree_traversal(T, T->root);
	printf("----------------------------------------\n");

	for (i = 0;i < 20;i ++) {

		rbtree_node *node = rbtree_search(T, keyArray[i]);
		rbtree_node *cur = rbtree_delete(T, node);
		free(cur);

		rbtree_traversal(T, T->root);
		printf("----------------------------------------\n");
	}
		
}

*/


