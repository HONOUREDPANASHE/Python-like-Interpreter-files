/*execute.c*/

/**
  * @brief Executes nuPython program, given as a Program Graph.
  *
  * @note HONOURED CHIKWAVA
  *
  * @note Starter code and partial solution: Prof. Joe Hummel
  * @note Northwestern University
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  // true, false
#include <string.h>
#include <assert.h>
#include <math.h>

#include "programgraph.h"
#include "ram.h"
#include "execute.h"


//
// Private functions:
//

//
// dupString
// 
// Duplicates the given string and returns a pointer
// to the copy.
// 
// NOTE: this function allocates memory for the copy,
// the caller takes ownership of the copy and must
// eventually free that memory.
//
static char* dupString(char* s)
{
  assert(s != NULL);

  //
  // be sure to include extra location for null terminator:
  //
  char* copy = (char*)malloc(sizeof(char) * (strlen(s) + 1));
  if (copy == NULL) {
    printf("**EXECUTION ERROR: out of memory (dupString)");
    return NULL;
  }

  strcpy(copy, s);

  return copy;
}


//
// execute_get_value
//
// Given a unary expr, returns the value that it represents.
// This could be the result of a literal (e.g. "123"), or it
// could be the value from memory for an identifier (e.g. "x").
// 
// Note that this function can fail --- success or failure is
// returned via the success parameter, which is a pointer to 
// where True or False is "returned" to denote success/failure.
//
// Why would it fail? If the identifier does not exist in 
// memory. This is a semantic error, and an error message is 
// output before returning.
//
// NOTE: this function allocates memory for the value that
// is returned. The caller takes ownership of the copy and 
// must eventually free this memory via ram_free_value().
//
static struct RAM_VALUE* execute_get_value(
  struct UNARY_EXPR* unary,
  struct STMT* stmt,
  struct RAM* memory,
  bool* success)
{
  struct RAM_VALUE* value = NULL;

  //
  // we only have simple elements so far (no unary operators):
  //
  assert(unary->expr_type == UNARY_ELEMENT);

  struct ELEMENT* element = unary->element;

  *success = true;  // assume we'll be successful

  if (element->element_type == ELEMENT_IDENTIFIER) {
    //
    // identifier => variable
    //
    assert(element->element_type == ELEMENT_IDENTIFIER);

    char* var_name = element->element_value;

    value = ram_read_cell_by_name(memory, var_name);

    if (value == NULL) {
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", var_name, stmt->line);
      *success = false;
      return NULL;
    }
  }
  else {
    //
    // one of the literal types, we need to allocate memory:
    //
    char* literal = element->element_value;

    value = malloc(sizeof(struct RAM_VALUE));
    if (value == NULL) {
      printf("**EXECUTION ERROR: out of memory (line %d)\n", stmt->line);
      *success = false;
      return NULL;
    }

    switch (element->element_type) {
      case ELEMENT_INT_LITERAL:
        value->value_type = RAM_TYPE_INT;
        value->types.i = atoi(literal);
        break;

      case ELEMENT_REAL_LITERAL:
        value->value_type = RAM_TYPE_REAL;
        value->types.d = atof(literal);
        break;

      case ELEMENT_STR_LITERAL:
        value->value_type = RAM_TYPE_STR;
        value->types.s = dupString(literal);
        break;
      case ELEMENT_TRUE:
        value->value_type = RAM_TYPE_BOOLEAN;
        value->types.i = 1;  // True = 1
        break;

      case ELEMENT_FALSE:
        value->value_type = RAM_TYPE_BOOLEAN;
        value->types.i = 0;  // False = 0
        break;

      default:
        printf("**EXECUTION ERROR: unexpected element type in get_element_value (line %d)\n", stmt->line);
        *success = false;
        break;
    }
  }//else

  return value;
}


//
// execute_function_call
//
// Executes a function call statement, returning true if 
// successful and false if not (an error message will be
// output before false is returned, so the caller doesn't
// need to output anything).
// 
// Examples: print()
//           print(x)
//           print(123)
//
static bool execute_function_call(struct STMT* stmt, struct RAM* memory)
{
  struct STMT_FUNCTION_CALL* call = stmt->types.function_call;

  //
  // for now we are assuming it's a call to print:
  //
  char* function_name = call->function_name;

  if (strcmp(function_name, "print") != 0) {
    printf("**SEMANTIC ERROR: unknown function (line %d)\n", stmt->line);
    return false;
  }

  if (call->parameter == NULL)
    printf("\n");
  else {
    //
    // we have a parameter, which type of parameter?
    // Note that a parameter is a simple element, i.e.
    // identifier or literal (or True, False, None):
    //
    char* element_value = call->parameter->element_value;

    if (call->parameter->element_type == ELEMENT_STR_LITERAL) {
      printf("%s\n", element_value);
    }
    else if (call->parameter->element_type == ELEMENT_INT_LITERAL) {
      char* literal = element_value;
      int i = atoi(literal);
      printf("%d\n", i);
    }
    else if (call->parameter->element_type == ELEMENT_REAL_LITERAL) {
      char* literal = element_value;
      double d = atof(literal);
      printf("%lf\n", d);
    }
    else if (call->parameter->element_type == ELEMENT_TRUE) {//need to check for 0 or 1?
      printf("True\n");
    }
    else if (call->parameter->element_type == ELEMENT_FALSE) {
      printf("False\n");
    }
   
    else {
    
      assert(call->parameter->element_type == ELEMENT_IDENTIFIER);

      char* var_name = element_value;
      struct RAM_VALUE* value = ram_read_cell_by_name(memory, var_name);

      if (value == NULL) {
        printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", var_name, stmt->line);
        return false;
      }

      if (value->value_type == RAM_TYPE_INT)
        printf("%d\n", value->types.i);
      else if (value->value_type == RAM_TYPE_REAL)
        printf("%lf\n", value->types.d);
      else if (value->value_type == RAM_TYPE_STR)
        printf("%s\n", value->types.s);

      else if (value->value_type == RAM_TYPE_BOOLEAN){
        if (value->types.i == 1)
          printf("True\n");
        else
          printf("False\n");
      }

      else {
        printf("**EXECUTION ERROR: unexpected value type in execute_function_call (line %d)\n", stmt->line);
        return false;
      }
    }
  }

  return true;
}




//
// execute_function_call_inside
//
// Executes an assignment  function call statement, returning RAM_VALUE* if 
// successful and NULL if not (an error message will be
// output before false is returned, so the caller doesn't
// need to output anything).
// 
// Examples: X = input()


static struct RAM_VALUE* execute_function_call_inside(struct STMT_ASSIGNMENT* assign, struct RAM* memory, struct STMT* stmt, struct RAM* mem)

{
  struct VALUE * rhs = assign->rhs;

  struct RAM_VALUE* return_value = NULL;
  struct FUNCTION_CALL* call = rhs->types.function_call;

  //
  // for now we are assuming it's a call to print:
  //
  char* function_name = call->function_name;
  struct ELEMENT* parameter = call->parameter;

  if (strcmp(function_name, "input") == 0) {
    if (parameter != NULL) {
  
    
    printf("%s", parameter->element_value); }
    
    char line[512];
    fgets(line, sizeof(line), stdin);
    line[strcspn(line, "\r\n")] = '\0';
    return_value = malloc(sizeof(struct RAM_VALUE));
    return_value->value_type = RAM_TYPE_STR;
    return_value->types.s = dupString(line);

  }

  else if (strcmp(function_name, "int") == 0) {
    struct ELEMENT* param = call->parameter;
    assert(param != NULL);

    assert (param->element_type == ELEMENT_IDENTIFIER) ;
      struct RAM_VALUE* v=  ram_read_cell_by_name( mem, param->element_value);
      if (v == NULL) {
        printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", param->element_value, stmt->line);
        return NULL;
      }
      
      assert (v->value_type == RAM_TYPE_STR);
        char* literal = dupString(v->types.s);
        int i = atoi(literal);   //check if it returns 0 for invalid strings?

        bool is_zeros = true;
        for (int j =0; j< strlen(literal); j++){
          if ( literal[j] != '0'){
            is_zeros = false;
            break;
          }
        }


        if (i == 0 && (is_zeros== false)) {
          printf("**SEMANTIC ERROR: conversion failed for int() (line %d)\n", stmt->line);
          return NULL;
        }


        return_value = malloc(sizeof(struct RAM_VALUE));
        return_value->value_type = RAM_TYPE_INT;
        return_value->types.i = i;
      
    }  

  

  else if (strcmp(function_name, "float") == 0) {
    struct ELEMENT* param = call->parameter;
    assert(param != NULL);

    assert (param->element_type == ELEMENT_IDENTIFIER) ;
      struct RAM_VALUE* v=  ram_read_cell_by_name( mem, param->element_value);
      if (v == NULL) {
        printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", param->element_value, stmt->line);
        return NULL;
      }
      
      assert (v->value_type == RAM_TYPE_STR);
        char* literal = dupString(v->types.s);
        double i = atof(literal);   //check if it returns 0 for invalid strings?

        bool is_zeros = true;
        for (int j =0; j< strlen(literal); j++){
          if ( literal[j] != '0' && literal[j] != '.'){
            is_zeros = false;
            break;
          }
        }


        if (i == 0 && (is_zeros== false)) {
          printf("**SEMANTIC ERROR: conversion failed for float() (line %d)\n", stmt->line);
          return NULL;
        }


        return_value = malloc(sizeof(struct RAM_VALUE));
        return_value->value_type = RAM_TYPE_REAL;
        return_value->types.d = i;
      
    }    

  

  else  {
    printf("**SEMANTIC ERROR: unknown function (line %d)\n", stmt->line); //changed to input
    // return false;
    return NULL;
  }

  return return_value;
}


//
// execute_binary_expression_ints
//
// Given two integer values and an operator, performs the operation
// and returns the integer result.
//
static int execute_binary_expression_ints(struct STMT* stmt, int lhs, int operator_type, int rhs, bool* success)
{
  assert(operator_type != OPERATOR_NO_OP);

  int result = -1;
  
  *success = true;  // assume success, change if not:

  //
  // perform the operation:
  //
  switch (operator_type)
  {
  case OPERATOR_PLUS:
    result = lhs + rhs;
    break;

  case OPERATOR_MINUS:
    result = lhs - rhs;
    break;

  case OPERATOR_ASTERISK:
    result = lhs * rhs;
    break;

  case OPERATOR_POWER:
    result = (int)pow(lhs, rhs);
    break;

  case OPERATOR_MOD:
    if (rhs == 0) {
      printf("**SEMANTIC ERROR: modulus by 0 (line %d)\n", stmt->line);
      *success = false;
    }
    else {
      result = lhs % rhs;
    }
    break;

  case OPERATOR_DIV:
    if (rhs == 0) {
      printf("**SEMANTIC ERROR: division by 0 (line %d)\n", stmt->line);
      *success = false;
    }
    else {
      result = lhs / rhs;
    }
    break;

    case OPERATOR_EQUAL:
    result = (lhs == rhs) ? 1 : 0;
    break;
  case OPERATOR_NOT_EQUAL:
    result = (lhs != rhs) ? 1 : 0;
    break;
  case OPERATOR_LT:
    result = (lhs < rhs) ? 1 : 0;
    break;
  case OPERATOR_LTE:
    result = (lhs <= rhs) ? 1 : 0;
    break;
  case OPERATOR_GT:
    result = (lhs > rhs) ? 1 : 0;
    break;
  case OPERATOR_GTE:
    result = (lhs >= rhs) ? 1 : 0;
    break;

  default:
    //
    // did we miss something?
    //
    printf("**INTERNAL ERROR: unexpected operator_type (%d) in execute_binary_expr (line %d)\n", operator_type, stmt->line);
    *success = false;
  }

  return result;
}


//
// execute_binary_expression_REALS
//
// Given two real values and an operator, or an integer and a real value, 
// performs the operation for real numbers
// and returns the result 
//
static double execute_binary_expression_reals(struct STMT* stmt, double lhs, int operator_type, double rhs, bool* success)
{
  assert(operator_type != OPERATOR_NO_OP);

  double result = -1.00;
  
  *success = true;  // assume success, change if not:

  //
  // perform the operation:
  //
  switch (operator_type)
  {
  case OPERATOR_PLUS:
    result = lhs + rhs;
    break;

  case OPERATOR_MINUS:
    result = lhs - rhs;
    break;

  case OPERATOR_ASTERISK:
    result = lhs * rhs;
    break;

  case OPERATOR_POWER:
    result = (double)pow(lhs, rhs);
    break;

  case OPERATOR_MOD:                                        
    if (rhs == 0) {
      printf("**SEMANTIC ERROR: modulus by 0 (line %d)\n", stmt->line);
      *success = false;
    }
    else {
      result = fmod(lhs, rhs);
    }

    break;

  case OPERATOR_DIV:
    if (rhs == 0) {
      printf("**SEMANTIC ERROR: division by 0 (line %d)\n", stmt->line);
      *success = false;
    }
    else {
      result = lhs / rhs;
    }
    break;

  case OPERATOR_EQUAL:
    result = (lhs == rhs) ? 1 : 0;
    break;
  case OPERATOR_NOT_EQUAL:
    result = (lhs != rhs) ? 1 : 0;
    break;
  case OPERATOR_LT:
    result = (lhs < rhs) ? 1 : 0;
    break;
  case OPERATOR_LTE:
    result = (lhs <= rhs) ? 1 : 0;
    break;
  case OPERATOR_GT:
    result = (lhs > rhs) ? 1 : 0;
    break;
  case OPERATOR_GTE:
    result = (lhs >= rhs) ? 1 : 0;
    break;
   

  default:
    //
    // did we miss something?
    //
    printf("**INTERNAL ERROR: unexpected operator_type (%d) in execute_binary_expr (line %d)\n", operator_type, stmt->line);
    *success = false;
  }

  return result;
}


//
// execute_binary_expression_strs
//
// Given two string values and an operator, performs the operation for strings
// and returns the result as a string.
//
static char* execute_binary_expression_strs(struct STMT* stmt, char* lhs, int operator_type, char* rhs, bool* success)
{
  assert(operator_type != OPERATOR_NO_OP);

  char* result = malloc(sizeof(char) * (strlen(lhs) + strlen(rhs) + 1)); // +1 for null terminator

  
  *success = true;  // assume success, change if not:

  //
  // perform the operation:
  //
  switch (operator_type)
  {
  case OPERATOR_PLUS:
    strcpy(result, lhs);
    strcat(result, rhs);
    break;
  case OPERATOR_EQUAL:
    strcpy(result, (strcmp(lhs, rhs) == 0) ? "1" : "0");
    break;
  case OPERATOR_NOT_EQUAL:
    strcpy(result, (strcmp(lhs, rhs) != 0) ? "1" : "0");
    break;
  case OPERATOR_LT:
    strcpy(result, (strcmp(lhs, rhs) < 0) ? "1" : "0");
    break;
  case OPERATOR_LTE:
    strcpy(result, (strcmp(lhs, rhs) <= 0) ? "1" : "0");
    break;
  case OPERATOR_GT:
    strcpy(result, (strcmp(lhs, rhs) > 0) ? "1" : "0");
    break;
  case OPERATOR_GTE:
    strcpy(result, (strcmp(lhs, rhs) >= 0) ? "1" : "0");
    break;


  default:
    //
    // did we miss something?
    //
    printf("**INTERNAL ERROR: unexpected operator_type (%d) in execute_binary_expr (line %d)\n", operator_type, stmt->line);
    *success = false;
  }

  return result;
}


//
// execute_expr
// given an expression, memeory and a statement
//executes an expression and returns the resulting RAM_VALUE*
//
static struct RAM_VALUE*  execute_expr (struct STMT* stmt, struct RAM* memory,struct EXPR* expr){

  assert(expr->lhs != NULL);

  bool success;

  struct RAM_VALUE* lhs_value = execute_get_value(expr->lhs, stmt, memory, &success);
  if (!success)  // semantic error? If so, return now:
    return NULL;


   if (expr->isBinaryExpr) {
    //
    // binary expression such as x + y
    //
    assert(expr->operator_type != OPERATOR_NO_OP);  // we must have an operator

    struct RAM_VALUE* rhs_value = execute_get_value(expr->rhs, stmt, memory, &success);

    if (!success)  // semantic error? If so, return now:
      return NULL;
    
    if (lhs_value->value_type == RAM_TYPE_INT && rhs_value->value_type == RAM_TYPE_INT) {
     int result = execute_binary_expression_ints(stmt, lhs_value->types.i, expr->operator_type, rhs_value->types.i, &success);
         if (!success)
      return NULL;      /////work on this mf 

    if (expr->operator_type == OPERATOR_EQUAL || expr->operator_type == OPERATOR_NOT_EQUAL || expr->operator_type == OPERATOR_LT  
    || expr->operator_type == OPERATOR_LTE || expr->operator_type == OPERATOR_GT || expr->operator_type == OPERATOR_GTE){

      lhs_value->value_type = RAM_TYPE_BOOLEAN;
    }

    lhs_value->types.i = result;
    
    }





    else if ((lhs_value->value_type == RAM_TYPE_REAL && rhs_value->value_type == RAM_TYPE_REAL)  
    || (lhs_value->value_type == RAM_TYPE_INT && rhs_value->value_type == RAM_TYPE_REAL) 
    || (lhs_value->value_type == RAM_TYPE_REAL && rhs_value->value_type == RAM_TYPE_INT))
    
    {

      //convert int to real if needed
      if (lhs_value->value_type == RAM_TYPE_INT){

        lhs_value->types.d = (double)lhs_value->types.i;
        lhs_value->value_type = RAM_TYPE_REAL;}

      if (rhs_value->value_type == RAM_TYPE_INT){
        rhs_value->types.d = (double)rhs_value->types.i;
        }
     double result = execute_binary_expression_reals(stmt, lhs_value->types.d, expr->operator_type, rhs_value->types.d, &success);
         if (!success)
        return NULL;

        if (expr->operator_type == OPERATOR_EQUAL || expr->operator_type == OPERATOR_NOT_EQUAL || expr->operator_type == OPERATOR_LT
        || expr->operator_type == OPERATOR_LTE || expr->operator_type == OPERATOR_GT || expr->operator_type == OPERATOR_GTE){
      lhs_value->value_type = RAM_TYPE_BOOLEAN;
      lhs_value->types.i = (int)result;

    }
    else{
      lhs_value->types.d = result;
    }

    // assert(lhs_value->value_type == RAM_TYPE_REAL);
    
    }






    else if (lhs_value->value_type == RAM_TYPE_STR && rhs_value->value_type == RAM_TYPE_STR) {

     char* result = execute_binary_expression_strs(stmt, lhs_value->types.s, expr->operator_type, rhs_value->types.s, &success);
         if (!success)
      return NULL;

        if (expr->operator_type == OPERATOR_EQUAL || expr->operator_type == OPERATOR_NOT_EQUAL || expr->operator_type == OPERATOR_LT
        || expr->operator_type == OPERATOR_LTE || expr->operator_type == OPERATOR_GT || expr->operator_type == OPERATOR_GTE){
      lhs_value->value_type = RAM_TYPE_BOOLEAN;
      lhs_value->types.i = atoi(result);

    }
    else{
      lhs_value->types.s =  dupString(result);  //MAKE SURE TO FREE THIS LATER
    }
    // make a copy of the result string
    }








    else {
      //
      // unsupported type combination:
      //
      
      printf("**SEMANTIC ERROR: invalid operand types (line %d)\n", stmt->line);
      return NULL;
     

  }}


  return lhs_value;
}


//
// execute_assignment
//
// Executes an assignment statement, returning true if 
// successful and false if not (an error message will be
// output before false is returned, so the caller doesn't
// need to output anything).
// 
// Examples: x = 123
//           y = x ** 2
//
static bool execute_assignment(struct STMT* stmt, struct RAM* memory)
{
  struct STMT_ASSIGNMENT* assign = stmt->types.assignment;

  char* var_name = assign->var_name;
  bool success;
  //
  // we only have expressions on the RHS, no function calls:
  //
  if (assign->rhs->value_type == VALUE_EXPR){

  struct EXPR* expr = assign->rhs->types.expr;
  struct RAM_VALUE* lhs_value = execute_expr (stmt, memory, expr);
  if (lhs_value == NULL)  // semantic error? If so, return now:
    return false;


    

  if (assign->isPtrDeref) {
    //
    // we have *ptr = ...
    //
    struct RAM_VALUE* address = ram_read_cell_by_name(memory, var_name);

    if (address == NULL) {
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", var_name, stmt->line);
      return false;
    }
    
    if (address->value_type != RAM_TYPE_INT) {
      printf("**SEMANTIC ERROR: invalid address type for pointer assignment (line %d)\n", stmt->line);
      return false; 
    }

    int addr = address->types.i;

    success = ram_write_cell_by_addr(memory, *lhs_value, addr);

    if (!success) {
      printf("**SEMANTIC ERROR: invalid memory address for pointer assignment (line %d)\n", stmt->line);
      return false;
    }

    return true;
  }

  else {
    //
    // normal assignment x = ...
    //
    success = ram_write_cell_by_name(memory, *lhs_value, var_name);
    return success;
  }



}



  else {
    //
    // we have a function call on the RHS
    //
    assert(assign->rhs->value_type == VALUE_FUNCTION_CALL);
    struct  RAM_VALUE* value = execute_function_call_inside(assign, memory,stmt, memory);

    if (value == NULL)  // semantic error? If so, return now:
      return false;
    bool success = ram_write_cell_by_name(memory, *value, var_name);
    return success;

    

    printf("**SEMANTIC ERROR: function calls not supported on RHS of assignment (line %d)\n", stmt->line);
    return false;
  }
}


//
// Public functions:
//

/**
  * @brief executes a nuPython program
  *
  * Given a nuPython program graph and a memory, 
  * executes the statements in the program graph.
  * If a semantic error occurs (e.g. type error),
  * an error message is output, execution stops,
  * and the function returns.
  *
  * @param program pointer to first stmt in program graph
  * @param memory pointer to memory unit
  * @return void
  */
void execute(struct STMT* program, struct RAM* memory)
{
  struct STMT* stmt = program;

  //
  // traverse through the program statements:
  //
  while (stmt != NULL) {

    if (stmt->stmt_type == STMT_ASSIGNMENT) {

      bool success = execute_assignment(stmt, memory);

      if (!success)
        return;

      stmt = stmt->types.assignment->next_stmt;  // advance
    }
    else if (stmt->stmt_type == STMT_FUNCTION_CALL) {

      bool success = execute_function_call(stmt, memory);

      if (!success)
        return;

      stmt = stmt->types.function_call->next_stmt;
    }

    else if (stmt->stmt_type == STMT_IF_THEN_ELSE) {

      struct EXPR* condition = stmt->types.if_then_else->condition;
      struct RAM_VALUE* cond_value = execute_expr (stmt, memory, condition);
      if (cond_value == NULL){ // semantic error? If so, return now:
        // printf("**SEMANTIC ERROR: invalid condition type (line %d)\n", stmt->line);
        return;}

      if (!((cond_value->value_type)== RAM_TYPE_BOOLEAN || (cond_value->value_type) == RAM_TYPE_INT)) {
         printf("**SEMANTIC ERROR: invalid condition type (line %d)\n", stmt->line);
        return;
      }


      if (cond_value->types.i == 1) {
        stmt = stmt->types.if_then_else->true_path;
      }
      else {
        if (stmt->types.if_then_else->false_path == NULL) {
          stmt = stmt->types.if_then_else->next_stmt;    //how about next path
        }
        stmt = stmt->types.if_then_else->false_path;    //how about next path
      }


    }


  else if (stmt->stmt_type == STMT_WHILE_LOOP) {
  struct EXPR* condition = stmt->types.while_loop->condition;
  struct RAM_VALUE* cond_value = execute_expr (stmt, memory, condition);
  if (cond_value == NULL){ 
    return;
  }

  if (!((cond_value->value_type)== RAM_TYPE_BOOLEAN || (cond_value->value_type) == RAM_TYPE_INT)) {
    printf("**SEMANTIC ERROR: Invalid condition type (line %d)\n", stmt->line);
    return;
  }

  if (cond_value->types.i != 0) {  // condition is true
    stmt = stmt->types.while_loop->loop_body;
  }
  else {  // condition is false
    stmt = stmt->types.while_loop->next_stmt;
  }
  
  
}





    else {
      assert(stmt->stmt_type == STMT_PASS);

      //
      // nothing to do!
      //

      stmt = stmt->types.pass->next_stmt;
    }
  }//while

  //
  // done:
  //
  return;
}
