/*ram.c*/

/**
  * @brief Random access memory (RAM) for nuPython
  *
  * To execute Python code, the nuPython interpreter needs to store the
  * values of Python variables. The RAM functions serve this purpose.
  *
  * @note Honoured Chikwava 
  *
  * @note template by Prof. Joe Hummel
  * @note Northwestern University
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // true, false
#include <string.h>
#include <assert.h>

#include "ram.h"


//
// Public functions:
//

/**
  * @brief ram_init: initialize memory unit
  *
  * Returns a pointer to a dynamically-allocated memory
  * for storing nuPython variables and their values. All
  * memory cells are initialized to the value None. You
  * take ownership of the returned memory and must call
  * ram_destroy() when you are done.
  *
  * @return pointer to struct denoting memory unit
  * 
  * 
  
  */


// Binary search helper function
//  
 int b_srch(struct RAM* mem, char* target)
 {   int low = 0;
     int high = mem->size - 1;
    //  printf(" Checking variable %s\n", target); 

     while (low <= high)
     {   
          int mid = low + ( (high - low) / 2);


          if (strcmp(mem->map[mid].varname, target) == 0)
          {
            // printf("Found at mid %i\n", mid);
          return mid;
          }

          if (strcmp(mem->map[mid].varname, target) < 0)
          {
              low = mid + 1;
          }

          else
          {
              high = mid - 1 ;
          }
      }

     return -1;
 }


// Insertion sort helper function
//sorts the map array in RAM struct

void mapsort(struct RAM* memory)
{
    //Insertion sort 
    int n = memory->size;

    for (int i = 1; i < n; i++)
    {
        struct RAM_MAP key = memory->map[i];
        int j = i - 1;

        while (j >= 0 && strcmp(memory->map[j].varname, key.varname) > 0)
        {
            memory->map[j + 1] = memory->map[j];
            j = j - 1;
        }
        memory->map[j + 1] = key;
    }
}



struct RAM* ram_init(void)
{

  struct RAM* memory = (struct RAM*) malloc(sizeof(struct RAM));
   
  struct RAM_VALUE* cells = (RAM_VALUE*) malloc(4 * sizeof(RAM_VALUE));    ///clean up might be needed 
  struct RAM_MAP* map = (struct RAM_MAP*) malloc(4 * sizeof( RAM_MAP));

  for (int i = 0; i < 4; i++)
  {
    cells[i].value_type = RAM_TYPE_NONE;
  }
  //  for (int i = 0; i < 4; i++)
  // {
  //   map[i].varname = NULL;
  //   map[i].cell = 0;
  // }
  memory->cells = cells;
  memory->map = map;
  memory->size = 0;
  memory->capacity = 4;
  return memory;
}


/**
  * @brief ram_destroy: frees memory associated with memory unit
  * 
  * Frees the dynamically-allocated memory associated with
  * the given memory. After the call returns, you cannot
  * use the memory.
  *
  * @return void
  */
void ram_destroy(struct RAM* memory)
{
  if (memory == NULL) return;
    
    
    for (int i = 0; i < memory->size; i++) {
        if (memory->cells[i].value_type == RAM_TYPE_STR) {
            free(memory->cells[i].types.s);
        }
    }
   
    for (int i = 0; i < memory->size; i++) {
        free(memory->map[i].varname);  // <-- ADD THIS
    }
    
    free(memory->cells);
    free(memory->map);
    free(memory);
}


/**
  * @brief ram_size: # of vars in memory
  *
  * Returns the # of variables currently stored in memory.
  *
  * @return # of vars in memory
  */
int ram_size(struct RAM* memory)
{
  return memory->size ;
}


/**
  * @brief ram_capacity: # of cells available in memory
  *
  * Returns the # of cells currently available in memory.
  *
  * @return # of cells available in memory
  */
int ram_capacity(struct RAM* memory)      ////this shiii is always zero update it rnnnnnn in your add
{
  return memory->capacity;
}


/**
  * @brief ram_get_addr: address of memory cell occupied by variable
  *
  * If the given variable (e.g. "x") has been written to 
  * memory, returns the address of this variable --- an integer
  * in the range 0..N-1 where N is the number of vars currently 
  * stored in memory. Returns -1 if no such variable exists 
  * in memory. 
  *
  * NOTE: a variable has to be written to memory before you can
  * get its address. Once a variable is written to memory, its
  * address never changes. 
  *
  * @param memory Pointer to struct denoting memory unit
  * @param varname variable name
  * @return address of variable or -1 if doesn't exist
  */
int ram_get_addr(struct RAM* memory, char* varname)
{
  int x =  b_srch( memory, varname); 

  if ( x == -1)
  {
      return -1;
  }

  else{
      return memory->map[x].cell;
  }

}


/**
  * @brief ram_read_cell_by_addr: returns value in memory cell at this address
  *
  * Given a memory address (an integer in the range 0..N-1), 
  * returns a COPY of the value contained in that memory cell.
  * Returns NULL if the address is not valid.
  * 
  * NOTE: this function allocates memory for the value that
  * is returned. The caller takes ownership of the copy and 
  * must eventually free this memory via ram_free_value().
  *
  * NOTE: a variable has to be written to memory before its
  * address becomes valid. Once a variable is written to memory,
  * its address never changes.
  *
  * @param memory Pointer to struct denoting memory unit
  * @param address memory cell address
  * @return pointer to struct containing value or NULL if doesn't exist
  */
struct RAM_VALUE* ram_read_cell_by_addr(struct RAM* memory, int address)    ////test this 
{
  if (address > memory->size - 1 || address < 0)  //boundary check
  {
      return NULL;
  }

  struct RAM_VALUE val = memory->cells[address];
  struct RAM_VALUE* copy = (struct RAM_VALUE*) malloc(sizeof(struct RAM_VALUE));
  copy->value_type = val.value_type;
  if (val.value_type == RAM_TYPE_STR)
  {
      copy->types.s = strdup(val.types.s);
  }
  else if (val.value_type == RAM_TYPE_INT)
  {
      copy->types.i = val.types.i;
  }
  else if (val.value_type == RAM_TYPE_REAL)
  {
      copy->types.d = val.types.d;
  }
  else if (val.value_type == RAM_TYPE_PTR)
  {
      copy->types.i = val.types.i;
  }
  else if (val.value_type == RAM_TYPE_BOOLEAN)
  {
      copy->types.i = val.types.i;
  }
  else if (val.value_type == RAM_TYPE_NONE)
  {
      // do nothing
  }
  return copy;
}


/**
  * @brief ram_read_cell_by_name: returns value in memory cell for this variable
  *
  * If the given variable (e.g. "x") has been written to 
  * memory, returns a COPY of the value contained in memory.
  * Returns NULL if no such name exists in memory.
  *
  * NOTE: this function allocates memory for the value that
  * is returned. The caller takes ownership of the copy and 
  * must eventually free this memory via ram_free_value().
  *
  * @param memory Pointer to struct denoting memory unit
  * @param varname variable name
  * @return pointer to struct containing value or NULL if doesn't exist
  */
struct RAM_VALUE* ram_read_cell_by_name(struct RAM* memory, char* varname)
{

  int index = b_srch( memory, varname );
  if (index == -1)
  {
      return NULL;
  }


  struct RAM_VALUE val = memory->cells[memory->map[index].cell];
  struct RAM_VALUE* copy = (struct RAM_VALUE*) malloc(sizeof(struct RAM_VALUE));
  copy->value_type = val.value_type;
  if (val.value_type == RAM_TYPE_STR)
  {
      copy->types.s = strdup(val.types.s);
  }
  else if (val.value_type == RAM_TYPE_INT)
  {
      copy->types.i = val.types.i;
  }
  else if (val.value_type == RAM_TYPE_REAL)
  {
      copy->types.d = val.types.d;
  }
  else if (val.value_type == RAM_TYPE_PTR)
  {
      copy->types.i = val.types.i;
  }
  else if (val.value_type == RAM_TYPE_BOOLEAN)
  {
      copy->types.i = val.types.i;
  }
  else if (val.value_type == RAM_TYPE_NONE)
  {
      // do nothing
  }


  return copy;
  
  
}


/**
  * @brief ram_free_value: free value returned by read_cell() functions
  *
  * Frees the memory value returned by ram_read_cell_by_name and
  * ram_read_cell_by_addr.
  *
  * @param value Pointer to struct containing value
  * @return void
  */
void ram_free_value(struct RAM_VALUE* value)

{

  if (value == NULL) return;
  if (value->value_type == RAM_TYPE_STR) //////////WHY
  {
      free(value->types.s);
  }
  free(value);
  
  return;
}


/**
  * @brief ram_write_cell_by_addr: writes a value to memory cell at this address
  *
  * Writes the given value to the memory cell at the given 
  * address. If a value already exists at this address, that
  * value is overwritten by this new value. Returns true if 
  * the value was successfully written, false if not (which 
  * implies the memory address is invalid).
  *
  * NOTE: if the value being written is a string, it will
  * be duplicated and stored.
  * 
  * NOTE: a variable has to be written to memory before its
  * address becomes valid. Once a variable is written to memory,
  * its address never changes.
  *
  * @param memory Pointer to struct denoting memory unit
  * @param value value to be written to memory
  * @param address memory cell address
  * @return true if successful, false if not (invalid address)
  */
bool ram_write_cell_by_addr(struct RAM* memory, struct RAM_VALUE value, int address)
{ if (address >= 0 && address < memory->size)

  {   if (memory->cells[address].value_type == RAM_TYPE_STR) {
          free(memory->cells[address].types.s);  
      }}
    
    
    
    
    if (value.value_type == RAM_TYPE_STR)
      {
          memory->cells[address].types.s = strdup(value.types.s);  //this enough??
          memory->cells[address].value_type = RAM_TYPE_STR;
          return true;
      }

      else if (value.value_type == RAM_TYPE_INT)
      {
          memory->cells[address].types.i = value.types.i;
          memory->cells[address].value_type = RAM_TYPE_INT;
          return true;
      }

      else if (value.value_type == RAM_TYPE_REAL)
      {
          memory->cells[address].types.d = value.types.d;
          memory->cells[address].value_type = RAM_TYPE_REAL;
          return true;
      }
      else if (value.value_type == RAM_TYPE_PTR)
      {
          memory->cells[address].types.i = value.types.i;
          memory->cells[address].value_type = RAM_TYPE_PTR;
          return true;
      }
      else if (value.value_type == RAM_TYPE_BOOLEAN)
      {
          memory->cells[address].types.i = value.types.i;
          memory->cells[address].value_type = RAM_TYPE_BOOLEAN;
          return true;
      }
      else  
      {
          memory->cells[address].value_type = RAM_TYPE_NONE;   //add none?
          return true;
      }

  }
  



/**
  * ram_write_cell_by_name
  *
  * Writes the given value to a memory cell named by the given
  * variable. If a memory cell already exists with this name,
  * the existing value is overwritten by this new value. Returns
  * true since this operation always succeeds.
  *
  * NOTE: if the value being written is a string, it will
  * be duplicated and stored.
  *
  * NOTE: a variable has to be written to memory before its
  * address becomes valid. Once a variable is written to memory,
  * its address never changes.
  *
  * @param memory Pointer to struct denoting memory unit
  * @param value value to be written to memory
  * @param varname variable name
  * @return true (always successful)
  */
bool ram_write_cell_by_name(struct RAM* memory, struct RAM_VALUE value, char* varname) ///what if it exists already?????? 
{ 
  if (memory->size == memory->capacity)
  {   //resize 
      int new_capacity = memory->capacity * 2;

      memory->cells = (RAM_VALUE*) realloc( memory->cells, new_capacity * sizeof(RAM_VALUE));

      for (int i = memory->size ; i < new_capacity; i++)
        {
          
        memory->cells[i].value_type = RAM_TYPE_NONE;
        }

      memory->map = (RAM_MAP*) realloc( memory->map, new_capacity * sizeof(RAM_MAP));
      memory->capacity = new_capacity;
  }

  int index = b_srch( memory, varname );   //duplicate???  pass by value??


  if (index != -1)
  {
    int addy = memory->map[index].cell;
  if (memory->cells[addy].value_type == RAM_TYPE_STR) {
    free(memory->cells[addy].types.s);
  }
  
  if (value.value_type == RAM_TYPE_STR) {
    memory->cells[addy] = value;
    memory->cells[addy].types.s = strdup(value.types.s);
  } else {
    memory->cells[addy] = value;
  }
    return true;
  }



  
  else{
   
    // printf("Adding new variable");

    if (value.value_type == RAM_TYPE_STR) {
    memory->cells[memory->size] = value;
    memory->cells[memory->size].types.s = strdup(value.types.s);
    memory->map[memory->size].varname = strdup(varname);  
    memory->map[memory->size].cell = memory->size;
    memory->size = memory->size + 1;
    mapsort( memory);
    return true;

  }

  else {
 
    memory->cells[memory->size] = value;
      memory->map[memory->size].varname = strdup(varname);   
      memory->map[memory->size].cell = memory->size;  
      memory->size = memory->size + 1;
      mapsort( memory);

      return true;
  }

  
}


}

 


/**
  * @brief ram_print: prints the contents of memory
  *
  * Prints the contents of RAM to the console, for debugging.
  * RAM is printed in alphabetical order by variable name.
  *
  * @param memory Pointer to struct denoting memory unit
  * @return void
  */
void ram_print(struct RAM* memory)
{
  printf("**MEMORY PRINT**\n");

  printf("Size: %d\n", memory->size);
  printf("Capacity: %d\n", memory->capacity);
  printf("Contents:\n");

  for (int i = 0; i < memory->size ; i++)
  {
   printf(" %s: ", memory->map[i].varname);


    struct RAM_VALUE value = memory->cells[memory->map[i].cell];


    if (value.value_type == RAM_TYPE_INT)
    {
        printf("int, %d", value.types.i);
    }
    else if (value.value_type == RAM_TYPE_REAL)
    {
        printf("real, %lf", value.types.d);
    }
    else if (value.value_type == RAM_TYPE_STR)
    {
        printf("str, '%s'", value.types.s);
    }
    else if (value.value_type == RAM_TYPE_PTR)
    {
        printf("ptr, %d", value.types.i);
    }
    else if (value.value_type == RAM_TYPE_BOOLEAN)
    {
        if (value.types.i == 0)
        {
            printf("boolean, False");
        }
        else
        {
            printf("boolean, True");
        }
    }
    else if (value.value_type == RAM_TYPE_NONE)
    {
        printf("none, None");
    }

  
  //  printf("int, %d", value?);
  //  printf("real, %lf", value?);
  //  printf("str, '%s'", value?);
  //  printf("ptr, %d", value?);
  //  printf("boolean, False");
  //  printf("boolean, True");
  //  printf("none, None");
  
   printf("\n");
  }

  printf("**END PRINT**\n");
}


/**
  * @brief ram_print_map: prints the contents of memory map
  *
  * Prints contents of RAM map to the console, for debugging.
  *
  * @param memory Pointer to struct denoting memory unit
  * @return void
  */
void ram_print_map(struct RAM* memory)
{
  printf("**MEMORY MAP PRINT**\n");

  for (int i = 0; i < memory->size; i++)
  {
   printf(" %s: %d\n", memory->map[i].varname, memory->map[i].cell);
  }

  printf("**END PRINT**\n");
}
