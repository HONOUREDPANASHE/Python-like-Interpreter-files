/*tests.c*/

/**
  * @brief unit tests for nuPython's memory unit
  *
  * @note Honoured Chikwava
  *
  * @note Initial tests by Prof. Joe Hummel
  * @note Northwestern University
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtest/gtest.h>

#include "ram.h"


//
// private helper functions:
//


//
// some provided unit tests to get started:
//
TEST(memory_module, initialization)
{
  //
  // create a new memory and make sure it's initialized properly:
  //
  struct RAM* memory = ram_init();

  ASSERT_TRUE(memory != NULL);        // use ASSERT_TRUE with pointers
  ASSERT_TRUE(memory->cells != NULL);
  ASSERT_TRUE(memory->map != NULL);

  ASSERT_EQ(ram_size(memory), 0);
  ASSERT_EQ(ram_capacity(memory), 4);
  
  //
  // memory cells should be initialized to NONE:
  //
  for (int i=0; i<ram_capacity(memory); i++) {
    ASSERT_EQ(memory->cells[i].value_type, RAM_TYPE_NONE);
  }

  //
  // tests passed, free memory
  //
   ram_print(memory);
  ram_print_map(memory);
  ram_destroy(memory);
}

TEST(memory_module, write_one_int) 
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  //
  // store the integer 123:
  //
  struct RAM_VALUE i;

  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  bool success = ram_write_cell_by_name(memory, i, "x");
  ASSERT_TRUE(success);

  ASSERT_EQ(ram_size(memory), 1);

  //
  // now check the memory, was x = 123 stored properly?
  //
  // since this is the first variable written, it will go
  // into memory cell 0:
  //
  ASSERT_EQ(memory->cells[0].value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->cells[0].types.i, 123);
  //
  // it will also go into map location 0:
  //
  ASSERT_STREQ(memory->map[0].varname, "x");  // strings => ASSERT_STREQ
  ASSERT_EQ(memory->map[0].cell, 0);
  ram_print(memory);
  ram_print_map(memory);

  //
  // tests passed, free memory
  //
  ram_destroy(memory);
}


TEST(memory_module, write_five_values_to_RELOCC )
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  //
  // store the integer 123:
  //
  struct RAM_VALUE i;

  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;


  struct RAM_VALUE j;

  j.value_type = RAM_TYPE_REAL;
  j.types.d = 1.23;


  struct RAM_VALUE k;

  k.value_type = RAM_TYPE_INT;
  k.types.d = 5;

  struct RAM_VALUE d;

  d.value_type = RAM_TYPE_STR;
  d.types.s = "hey";


  struct RAM_VALUE e;

  e.value_type = RAM_TYPE_STR;
  e.types.s = "ni";

  bool success = ram_write_cell_by_name(memory, i, "x");
  bool success2 = ram_write_cell_by_name(memory, j, "y");
  bool success3 = ram_write_cell_by_name(memory, k, "a");
  bool success4 = ram_write_cell_by_name(memory, d, "b");
  bool success5 = ram_write_cell_by_name(memory, e, "e");


  ASSERT_TRUE(success);
  ASSERT_TRUE(success2);
  ASSERT_TRUE(success4);
  ASSERT_TRUE(success5);

  ASSERT_EQ(ram_size(memory), 5);
  // ASSERT_EQ(ram_size(memory), 1);
  //
  // now check the memory, was x = 123 stored properly?
  //
  // since this is the first variable written, it will go
  // into memory cell 0:
  //
  ASSERT_EQ(memory->cells[0].value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->cells[0].types.i, 123);

  ASSERT_EQ(memory->cells[1].value_type, RAM_TYPE_REAL);
  ASSERT_EQ(memory->cells[1].types.d, 1.23);


  ASSERT_EQ(memory->cells[2].value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->cells[2].types.d, 5);


  ASSERT_EQ(memory->cells[3].value_type, RAM_TYPE_STR);
  ASSERT_TRUE(strcmp(memory->cells[3].types.s, "hey") == 0);

   ASSERT_EQ(memory->cells[4].value_type, RAM_TYPE_STR);
  ASSERT_TRUE(strcmp(memory->cells[4].types.s, "ni") ==0);
 

  //
  // it will also go into map location 0:
  //
  ASSERT_STREQ(memory->map[3].varname, "x");  // strings => ASSERT_STREQ
  ASSERT_EQ(memory->map[3].cell, 0);


  ASSERT_STREQ(memory->map[4].varname, "y");  // strings => ASSERT_STREQ
  ASSERT_EQ(memory->map[4].cell, 1);



  ASSERT_STREQ(memory->map[0].varname, "a");  // strings => ASSERT_STREQ
  ASSERT_EQ(memory->map[0].cell, 2);

  ASSERT_STREQ(memory->map[1].varname, "b");  // strings => ASSERT_STREQ
  ASSERT_EQ(memory->map[1].cell, 3);

  ASSERT_STREQ(memory->map[2].varname, "e");  // strings => ASSERT_STREQ
  ASSERT_EQ(memory->map[2].cell, 4);

  
  ASSERT_EQ(ram_size(memory), 5);
  
  ASSERT_EQ(ram_capacity(memory), 8);
  //
  // tests passed, free memory
  //
  ram_print(memory);
  ram_print_map(memory);
  ram_destroy(memory);
}


TEST(memory_module, write_one_int_read_back) 
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  //
  // store the integer 123:
  //
  struct RAM_VALUE i;

  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  bool success = ram_write_cell_by_name(memory, i, "x");
  ASSERT_TRUE(success);

  ASSERT_EQ(ram_size(memory), 1);

  //
  // can we read the value back successfully?
  //
  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "x");

  ASSERT_TRUE(value != NULL);  
  ASSERT_EQ(value->value_type, RAM_TYPE_INT);
  ASSERT_EQ(value->types.i, 123);

  //
  // tests passed, free memory
  //
  ram_print(memory);
  ram_print_map(memory);
  ram_free_value(value);
  ram_destroy(memory);
}



TEST(memory_module, write_one_int_read_back2) 
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  // ram_print(memory);
  // ram_print_map(memory);

  //
  // store the integer 123:
  //
  struct RAM_VALUE i;

  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  bool success = ram_write_cell_by_name(memory, i, "x");
  ASSERT_TRUE(success);

  ASSERT_EQ(ram_size(memory), 1);

  //
  // can we read the value back successfully?
  //
  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "x");
  struct RAM_VALUE* value22 = ram_read_cell_by_name(memory, "hshshshhs");

  ASSERT_TRUE(value22 == NULL);
  ASSERT_TRUE(value != NULL);  

  ASSERT_EQ(value->value_type, RAM_TYPE_INT);
  ASSERT_EQ(value->types.i, 123);
  int addr = ram_get_addr( memory, "x");
  ASSERT_EQ( addr, 0);
  struct RAM_VALUE j;
  j.value_type = RAM_TYPE_INT;
  j.types.i = 2;

  struct RAM_VALUE k;
  k.value_type = RAM_TYPE_INT;
  k.types.i = 2;
  bool success2 = ram_write_cell_by_name(memory, k, "a");
  ASSERT_TRUE(success2);
  ASSERT_EQ(ram_size(memory), 2);
  struct RAM_VALUE* value2 = ram_read_cell_by_name(memory, "a");
  ASSERT_TRUE(value2 != NULL);
  ASSERT_EQ(value2->value_type, RAM_TYPE_INT);
  ASSERT_EQ(value2->types.i, 2);
  bool success10 = ram_write_cell_by_name(memory, j, "a");
  int addr2 = ram_get_addr( memory, "a");
  ASSERT_EQ( addr2, 1);
  RAM_VALUE* read = ram_read_cell_by_addr(memory, 1); 
  ASSERT_TRUE(read != NULL);
  ASSERT_EQ(read->value_type, RAM_TYPE_INT);
  ASSERT_EQ(read->types.i, 2);

  struct RAM_VALUE newvalue;
  newvalue.value_type = RAM_TYPE_INT;
  newvalue.types.i = 42;
  ram_write_cell_by_addr(memory, newvalue, 1);

  RAM_VALUE* read2 = ram_read_cell_by_addr(memory, 1);
  ASSERT_TRUE(read2 != NULL);
  ASSERT_EQ(read2->value_type, RAM_TYPE_INT);
  ASSERT_EQ(read2->types.i, 42);

  ram_print(memory);
  ram_print_map(memory);
  ram_free_value(value);
  ram_free_value(value2);
  ram_free_value(read);
  ram_free_value(read2);
  ram_destroy(memory);
}

//
// TODO: add more unit tests
//


TEST(memory_module, write_multiple) 
{
  //
  // create a new memory:

  //
  struct RAM* memory = ram_init();

  char name[2];

// build the string "A"
  name[0] = 'A';
  name[1] = 0;

  for (int i = 1; i <= 26; i++) {
  

    struct RAM_VALUE value;
    value.value_type = RAM_TYPE_INT;
    value.types.i = i;

    bool success = ram_write_cell_by_name(memory, value, name);
    
    ASSERT_TRUE(success);
    // std::cout << "Wrote variable " << name << " with value " << i << std::endl;
    
    ASSERT_EQ(ram_size(memory), i);
    // int x = ram_size(memory);
    // std::cout << x << std::endl;
    

    name[0]++; 
     
    }

//   ram_print(memory);
//   ram_print_map(memory);

  ASSERT_EQ(ram_capacity(memory), 32);
   ram_print(memory);
  ram_print_map(memory);


  ram_destroy(memory);
}



TEST(memory_module, get_addr_all_types) 
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  //
  // Create test values for all data types:
  //
  
  // Integer
  struct RAM_VALUE i;
  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  // String
  struct RAM_VALUE j;
  j.value_type = RAM_TYPE_STR;
  j.types.s = "HEYY";

   // String
  struct RAM_VALUE sk;
  sk.value_type = RAM_TYPE_STR;
  sk.types.s = "HEYY";

  // None
  struct RAM_VALUE k;
  k.value_type = RAM_TYPE_NONE;
  
  // Real (double)
  struct RAM_VALUE b;
  b.value_type = RAM_TYPE_REAL;
  b.types.d = 1.23;

  // Boolean (True)
  struct RAM_VALUE bool_true;
  bool_true.value_type = RAM_TYPE_BOOLEAN;
  bool_true.types.i = 1;  // True

  // Boolean (False)
  struct RAM_VALUE bool_false;
  bool_false.value_type = RAM_TYPE_BOOLEAN;
  bool_false.types.i = 0;  // False

  // Pointer
  struct RAM_VALUE ptr;
  ptr.value_type = RAM_TYPE_PTR;
  ptr.types.i = 42;  // Pointer value (address)

  //
  // Write all values to memory:
  //
  bool success1 = ram_write_cell_by_name(memory, i, "x");
  bool success8 = ram_write_cell_by_name(memory, sk, "j");
  bool success2 = ram_write_cell_by_name(memory, j, "j");
  bool success3 = ram_write_cell_by_name(memory, k, "k");
  bool success4 = ram_write_cell_by_name(memory, b, "b");
  bool success5 = ram_write_cell_by_name(memory, bool_true, "bool_t");
  bool success6 = ram_write_cell_by_name(memory, bool_false, "bool_f");
  bool success7 = ram_write_cell_by_name(memory, ptr, "ptr");


  ASSERT_EQ(ram_size(memory), 7);

  //
  // Read values back by name:
  //
  struct RAM_VALUE* value_x = ram_read_cell_by_name(memory, "x");
  struct RAM_VALUE* value_j = ram_read_cell_by_name(memory, "j");
  struct RAM_VALUE* value_k = ram_read_cell_by_name(memory, "k");
  struct RAM_VALUE* value_b = ram_read_cell_by_name(memory, "b");
  struct RAM_VALUE* value_bool_t = ram_read_cell_by_name(memory, "bool_t");
  struct RAM_VALUE* value_bool_f = ram_read_cell_by_name(memory, "bool_f");
  struct RAM_VALUE* value_ptr = ram_read_cell_by_name(memory, "ptr");


  ASSERT_TRUE(value_x != NULL);
  ASSERT_EQ(value_x->value_type, RAM_TYPE_INT);
  ASSERT_EQ(value_x->types.i, 123);

  ASSERT_TRUE(value_j != NULL);
  ASSERT_EQ(value_j->value_type, RAM_TYPE_STR);
  ASSERT_STREQ(value_j->types.s, "HEYY");

  ASSERT_TRUE(value_k != NULL);
  ASSERT_EQ(value_k->value_type, RAM_TYPE_NONE);

  ASSERT_TRUE(value_b != NULL);
  ASSERT_EQ(value_b->value_type, RAM_TYPE_REAL);
  ASSERT_DOUBLE_EQ(value_b->types.d, 1.23);

  ASSERT_TRUE(value_bool_t != NULL);
  ASSERT_EQ(value_bool_t->value_type, RAM_TYPE_BOOLEAN);
  ASSERT_EQ(value_bool_t->types.i, 1);

  ASSERT_TRUE(value_bool_f != NULL);
  ASSERT_EQ(value_bool_f->value_type, RAM_TYPE_BOOLEAN);
  ASSERT_EQ(value_bool_f->types.i, 0);

  ASSERT_TRUE(value_ptr != NULL);
  ASSERT_EQ(value_ptr->value_type, RAM_TYPE_PTR);
  ASSERT_EQ(value_ptr->types.i, 42);


  //
  // Test get_addr for non-existent variable:
  //
  ASSERT_EQ(ram_get_addr(memory, "nonexistent"), -1);
  
  //
  // Test read by invalid address:
  //
  ASSERT_TRUE(ram_read_cell_by_addr(memory, 99) == NULL);
  ASSERT_TRUE(ram_read_cell_by_addr(memory, -1) == NULL);

  //
  // Get addresses and verify they're valid:
  //
  int addr_x = ram_get_addr(memory, "x");
  int addr_j = ram_get_addr(memory, "j");
  int addr_k = ram_get_addr(memory, "k");
  int addr_b = ram_get_addr(memory, "b");
  int addr_bool_t = ram_get_addr(memory, "bool_t");
  int addr_bool_f = ram_get_addr(memory, "bool_f");
  int addr_ptr = ram_get_addr(memory, "ptr");

  ASSERT_GE(addr_x, 0);
  ASSERT_GE(addr_j, 0);
  ASSERT_GE(addr_k, 0);
  ASSERT_GE(addr_b, 0);
  ASSERT_GE(addr_bool_t, 0);
  ASSERT_GE(addr_bool_f, 0);
  ASSERT_GE(addr_ptr, 0);



  //
  // Read back by address and verify all types:
  //
  
  // Integer
  RAM_VALUE* read_int = ram_read_cell_by_addr(memory, addr_x);
  ASSERT_TRUE(read_int != NULL);
  ASSERT_EQ(read_int->value_type, RAM_TYPE_INT);
  ASSERT_EQ(read_int->types.i, 123);

  // String
  RAM_VALUE* read_str = ram_read_cell_by_addr(memory, addr_j);
  ASSERT_TRUE(read_str != NULL);
  ASSERT_EQ(read_str->value_type, RAM_TYPE_STR);
  ASSERT_STREQ(read_str->types.s, "HEYY");

  // None
  RAM_VALUE* read_none = ram_read_cell_by_addr(memory, addr_k);
  ASSERT_TRUE(read_none != NULL);
  ASSERT_EQ(read_none->value_type, RAM_TYPE_NONE);

  // Real
  RAM_VALUE* read_real = ram_read_cell_by_addr(memory, addr_b);
  ASSERT_TRUE(read_real != NULL);
  ASSERT_EQ(read_real->value_type, RAM_TYPE_REAL);
  ASSERT_DOUBLE_EQ(read_real->types.d, 1.23);

  // Boolean True
  RAM_VALUE* read_bool_t = ram_read_cell_by_addr(memory, addr_bool_t);
  ASSERT_TRUE(read_bool_t != NULL);
  ASSERT_EQ(read_bool_t->value_type, RAM_TYPE_BOOLEAN);
  ASSERT_EQ(read_bool_t->types.i, 1);

  // Boolean False
  RAM_VALUE* read_bool_f = ram_read_cell_by_addr(memory, addr_bool_f);
  ASSERT_TRUE(read_bool_f != NULL);
  ASSERT_EQ(read_bool_f->value_type, RAM_TYPE_BOOLEAN);
  ASSERT_EQ(read_bool_f->types.i, 0);

  // Pointer
  RAM_VALUE* read_ptr = ram_read_cell_by_addr(memory, addr_ptr);
  ASSERT_TRUE(read_ptr != NULL);
  ASSERT_EQ(read_ptr->value_type, RAM_TYPE_PTR);
  ASSERT_EQ(read_ptr->types.i, 42);

  //
  // Test overwriting values with write_cell_by_addr:
  //
  struct RAM_VALUE new_int;
  new_int.value_type = RAM_TYPE_INT;
  new_int.types.i = 999;
  
  bool write_success = ram_write_cell_by_addr(memory, new_int, addr_x);
  ASSERT_TRUE(write_success);
  
  RAM_VALUE* updated_int = ram_read_cell_by_addr(memory, addr_x);
  ASSERT_TRUE(updated_int != NULL);
  ASSERT_EQ(updated_int->value_type, RAM_TYPE_INT);
  ASSERT_EQ(updated_int->types.i, 999);
  


  // write by addy


  bool w_i = ram_write_cell_by_addr(memory, i, 1);
  bool w_j = ram_write_cell_by_addr(memory, j, 2);
  bool w_k = ram_write_cell_by_addr(memory, k, 3);
  bool w_b = ram_write_cell_by_addr(memory, b, 4);
  bool w_bt = ram_write_cell_by_addr(memory, bool_true, 5);
  bool w_bf = ram_write_cell_by_addr(memory, bool_false, 6);
  bool w_p = ram_write_cell_by_addr(memory, ptr, 0);

  ASSERT_TRUE(w_i);
  ASSERT_TRUE(w_j);
  ASSERT_TRUE(w_k);
  ASSERT_TRUE(w_b);
  ASSERT_TRUE(w_bt);
  ASSERT_TRUE(w_bf);
  ASSERT_TRUE(w_p);

  RAM_VALUE* updated_int2 = ram_read_cell_by_addr(memory, 1);
  ASSERT_TRUE(updated_int2 != NULL);
  ASSERT_EQ(updated_int2->value_type, RAM_TYPE_INT);
  ASSERT_EQ(updated_int2->types.i, 123);

   RAM_VALUE* updated_3 = ram_read_cell_by_addr(memory, 2);
  ASSERT_TRUE(updated_3 != NULL);
  ASSERT_EQ(updated_3->value_type, RAM_TYPE_STR);
  ASSERT_EQ(strcmp(updated_3->types.s, "HEYY"), 0);


  

   ram_print(memory);
  ram_print_map(memory);
   

  //
  // Clean up all allocated memory:
  //
  ram_free_value(value_x);
  ram_free_value(value_j);
  ram_free_value(value_k);
  ram_free_value(value_b);
  ram_free_value(value_bool_t);
  ram_free_value(value_bool_f);
  ram_free_value(value_ptr);
  ram_free_value(read_int);
  ram_free_value(read_str);
  ram_free_value(read_none);
  ram_free_value(read_real);
  ram_free_value(read_bool_t);
  ram_free_value(read_bool_f);
  ram_free_value(read_ptr);
  ram_free_value(updated_int);

 
  
  ram_destroy(memory);
}


TEST(memory_module, string_handling_test)
{
  struct RAM* memory = ram_init();

  
  char name1[10] = "hello";
  struct RAM_VALUE str1;
  str1.value_type = RAM_TYPE_STR;
  str1.types.s = name1;
  
  ram_write_cell_by_name(memory, str1, "var1");
  
  
  strcpy(name1, "world");
  

  struct RAM_VALUE* read1 = ram_read_cell_by_name(memory, "var1");
  ASSERT_TRUE(read1 != NULL);
  ASSERT_EQ(read1->value_type, RAM_TYPE_STR);
  ASSERT_STREQ(read1->types.s, "hello");  
 
  char buffer[20];
  strcpy(buffer, "first");
  struct RAM_VALUE str2;
  str2.value_type = RAM_TYPE_STR;
  str2.types.s = buffer;
  ram_write_cell_by_name(memory, str2, "var2");
  
  strcpy(buffer, "second");
  struct RAM_VALUE str3;
  str3.value_type = RAM_TYPE_STR;
  str3.types.s = buffer;
  ram_write_cell_by_name(memory, str3, "var3");
  
\
  struct RAM_VALUE* read2 = ram_read_cell_by_name(memory, "var2");
  struct RAM_VALUE* read3 = ram_read_cell_by_name(memory, "var3");
  
  ASSERT_TRUE(read2 != NULL);
  ASSERT_STREQ(read2->types.s, "first");  
  
  ASSERT_TRUE(read3 != NULL);
  ASSERT_STREQ(read3->types.s, "second");  
  

  char new_str[20] = "updated";
  struct RAM_VALUE str4;
  str4.value_type = RAM_TYPE_STR;
  str4.types.s = new_str;
  
  ram_write_cell_by_name(memory, str4, "var1");  
  
  struct RAM_VALUE* read4 = ram_read_cell_by_name(memory, "var1");
  ASSERT_TRUE(read4 != NULL);
  ASSERT_STREQ(read4->types.s, "updated");
  

  int addr = ram_get_addr(memory, "var2");
  ASSERT_GE(addr, 0);
  
  char addr_str[20] = "by_address";
  struct RAM_VALUE str5;
  str5.value_type = RAM_TYPE_STR;
  str5.types.s = addr_str;
  
  ram_write_cell_by_addr(memory, str5, addr);
  
  struct RAM_VALUE* read5 = ram_read_cell_by_addr(memory, addr);
  ASSERT_TRUE(read5 != NULL);
  ASSERT_STREQ(read5->types.s, "by_address");  
\
  ram_free_value(read1);
  ram_free_value(read2);
  ram_free_value(read3);
  ram_free_value(read4);
  ram_free_value(read5);
  ram_destroy(memory);
}