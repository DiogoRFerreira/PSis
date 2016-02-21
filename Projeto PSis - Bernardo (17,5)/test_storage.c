#include "unity.h"
#include "storage.h"
#include "structures.h"

int index_for_list; 
int index_for_vector;

variables v;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_get_index(void)
{
	get_index(41, &index_for_list, &index_for_vector);
	TEST_ASSERT_EQUAL_INT(3,index_for_list);
	TEST_ASSERT_EQUAL_INT(0,index_for_vector);
	
	get_index(20, &index_for_list, &index_for_vector);
	TEST_ASSERT_EQUAL_INT(1,index_for_list);
	TEST_ASSERT_EQUAL_INT(19,index_for_vector);
	
	get_index(1, &index_for_list, &index_for_vector);
	TEST_ASSERT_EQUAL_INT(1,index_for_list);
	TEST_ASSERT_EQUAL_INT(0,index_for_vector);
	
	get_index(5, &index_for_list, &index_for_vector);
	TEST_ASSERT_EQUAL_INT(1,index_for_list);
	TEST_ASSERT_EQUAL_INT(4,index_for_vector);
	
	get_index(21, &index_for_list, &index_for_vector);
	TEST_ASSERT_EQUAL_INT(2,index_for_list);
	TEST_ASSERT_EQUAL_INT(0,index_for_vector);
}

void test_init_storage(void){
	init_storage(&v);
	TEST_ASSERT_EQUAL_INT(0,v.n_stored_messages);
	TEST_ASSERT_EQUAL_INT(0,v.n_aux_message_storage);
	TEST_ASSERT_NULL(v.head);	
}

void test_check_add_new_vector(void){
	/*pretende se verificar se ao adicionar mais uma mensagem e preciso
	 * gerar outro vector de armazenamento de mensagens*/
	v.n_stored_messages=40;
	v.n_aux_message_storage=2;
	TEST_ASSERT_EQUAL_INT(1,check_add_new_vector(v));
	
	v.n_stored_messages=39;
	v.n_aux_message_storage=2;	
	TEST_ASSERT_EQUAL_INT(0,check_add_new_vector(v));
	
	v.n_stored_messages=2;
	v.n_aux_message_storage=1;
	TEST_ASSERT_EQUAL_INT(0,check_add_new_vector(v));
	
	v.n_stored_messages=41;
	v.n_aux_message_storage=3;
	TEST_ASSERT_EQUAL_INT(0,check_add_new_vector(v));
	
}


