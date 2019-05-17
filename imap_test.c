#include "imap.h"




int imap_test1() 
{
	char key1[] = { "12345678901234567890121" };
	char key2[] = { "12345678901234567890122" };
	char key3[] = { "12345678901234567890123" };
	char key4[] = { "12345678901234567890124" };
	char key5[] = { "12345678901234567890125" };
	char key6[] = { "12345678901234567890126" };
	char key7[] = { "12345678901234567890127" };
	char key8[] = { "12345678901234567890128" };
	char key9[] = { "12345678901234567890129" };

	int value1 = 1;
	int value2 = 2;
	int value3 = 3;
	int value4 = 4;
	int value5 = 5;
	int value6 = 6;
	int value7 = 7;
	int value8 = 8;
	int value9 = 9;

	imap * map = imap_init();

	ikey _key1;
	_key1.key = key1;
	_key1.len = 24;

	ikey _key2;
	_key2.key = key2;
	_key2.len = 24;

	ikey _key3;
	_key3.key = key3;
	_key3.len = 24;

	ikey _key4;
	_key4.key = key4;
	_key4.len = 24;

	ikey _key5;
	_key5.key = key5;
	_key5.len = 24;

	ikey _key6;
	_key6.key = key6;
	_key6.len = 24;

	ikey _key7;
	_key7.key = key7;
	_key7.len = 24;

	ikey _key8;
	_key8.key = key8;
	_key8.len = 24;

	ikey _key9;
	_key9.key = key9;
	_key9.len = 24;

	int ret = -1;

	ret = imap_insert(map, _key1, &value1);
	if (ret < 0) {
		printf("²åÈëkey1Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));


	ret =  imap_insert(map, _key2, &value2);
	if (ret < 0) {
		printf("²åÈëkey2Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	ret = imap_insert(map, _key3, &value3);
	if (ret < 0) {
		printf("²åÈëkey3Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	ret = imap_insert(map, _key4, &value4);
	if (ret < 0) {
		printf("²åÈëkey4Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	ret = imap_insert(map, _key5, &value5);
	if (ret < 0) {
		printf("²åÈëkey5Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));


	ret = imap_insert(map, _key6, &value6);
	if (ret < 0) {
		printf("²åÈëkey6Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	ret = imap_insert(map, _key7, &value7);
	if (ret < 0) {
		printf("²åÈëkey7Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	ret = imap_insert(map, _key8, &value8);
	if (ret < 0) {
		printf("²åÈëkey8Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	ret = imap_insert(map, _key9, &value9);
	if (ret < 0) {
		printf("²åÈëkey9Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key3---------------\n");
	imap_del(map, _key3);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key9---------------\n");
	imap_del(map, _key9);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));


	printf("del key1---------------\n");
	imap_del(map, _key1);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));


	printf("del key6---------------\n");
	imap_del(map, _key6);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key5---------------\n");
	imap_del(map, _key5);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key8---------------\n");
	imap_del(map, _key8);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key2---------------\n");
	imap_del(map, _key2);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key7---------------\n");
	imap_del(map, _key7);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key4---------------\n");
	imap_del(map, _key4);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));


	printf("insert key3---------------\n");
	ret = imap_insert(map, _key3, &value3);
	if (ret < 0) {
		printf("²åÈëkey3Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("insert key4---------------\n");
	ret = imap_insert(map, _key4, &value4);
	if (ret < 0) {
		printf("²åÈëkey3Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key4---------------\n");
	imap_del(map, _key4);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	ret = imap_insert(map, _key1, &value1);
	if (ret < 0) {
		printf("²åÈëkey1Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));


	ret = imap_insert(map, _key2, &value2);
	if (ret < 0) {
		printf("²åÈëkey2Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key3---------------\n");
	imap_del(map, _key3);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	ret = imap_insert(map, _key3, &value3);
	if (ret < 0) {
		printf("²åÈëkey3Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key2---------------\n");
	imap_del(map, _key2);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key3---------------\n");
	imap_del(map, _key3);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	ret = imap_insert(map, _key1, &value1);
	if (ret < 0) {
		printf("²åÈëkey1Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key1---------------\n");
	imap_del(map, _key1);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));


	ret = imap_insert(map, _key1, &value1);
	if (ret < 0) {
		printf("²åÈëkey1Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("del key1---------------\n");
	imap_del(map, _key1);
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("insert key1------------\n");
	ret = imap_insert(map, _key1, &value1);
	if (ret < 0) {
		printf("²åÈëkey1Ê§°Ü!\n");
	}
	imap_traversal(map);
	printf("size: %d\n", imap_size(map));

	printf("close key1---------------\n");
	imap_delete_mini(map);
	imap_traversal(map);

	return 0;
}

void stack_test()
{
	int buf[] = { 0,1,2,3,4,5,6,7,8,9 };

	istack * stack = istack_init();

	for (int i = 0; i < 10; i++) {
		istack_push(stack, (void*)&(buf[i]));
	}

	for (int i = 0; i < 11; i++) {
		void * tmp = istack_pop(stack);
		if (tmp == NULL) {
			printf("stack is null \n");
		}
		else {
			printf("buf[%d] = %d  size= %d \n", i, *((int*)tmp), istack_isEnpty(stack));
		}
		
	}

	istack_destory(&stack);
}



int main1()
{
	//stack_test(); 
	imap_test1();
	return 0;
}
