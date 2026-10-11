/*debugger.cpp*/

/**
  * @brief simplified version of gdb for nuPython
  *
  * Debugger for nuPython, C++ edition! Provides a simple gdb-like
  * interface, with support for step-by-step execution of nuPython.
  * Uses nuPython interpreter as execution engine.
  *
  * @note HONOURED CHIKWAVA
  *
  * @note original version by Ata Guvendi
  * @note turned into assignment by Prof. Hummel
  * @note Northwestern University
  */



#include "debugger.h"
#include <algorithm>


using namespace std;
//helpers for manipulating program graph



//gets the next statement given the current stmt 
struct STMT* get_next_stmt(struct STMT* cur, struct RAM* ram){
  struct STMT* next = nullptr;
  if (cur != nullptr) {
     
    if (cur->stmt_type == STMT_ASSIGNMENT) {
       struct STMT_ASSIGNMENT* assignment = cur->types.assignment;
       next = assignment->next_stmt;
      
       
    } 

    else if (cur->stmt_type == STMT_FUNCTION_CALL) {
        // handle function call
         struct STMT_FUNCTION_CALL* func = cur->types.function_call;
           next = func->next_stmt;
        
    } 

    else if (cur->stmt_type == STMT_PASS) {
          struct STMT_PASS* pass_c = cur->types.pass;
           next = pass_c->next_stmt;
        
    } 

    else if (cur->stmt_type == STMT_WHILE_LOOP) {
        struct STMT_WHILE_LOOP* while_l = cur->types.while_loop;
          

        struct RAM_VALUE* value = execute_expr(cur,ram, while_l->condition);
       

        if (value == nullptr){
          cout << "here" << endl;
          next = nullptr;    
          }  ///should more be done here??  //return nullptr

          else if (value->types.i == 0){
            next = while_l->next_stmt; 
          }
          else {

            next = while_l->loop_body;
          }

        ram_free_value(value);}       

    else if (cur->stmt_type == STMT_IF_THEN_ELSE) {
        struct STMT_IF_THEN_ELSE* if_else = cur->types.if_then_else;
          

        struct RAM_VALUE* value2 = execute_expr(cur,ram, if_else->condition);
        

        if (value2 == nullptr){
          
          // cout << "execution failed" << endl;
          next = nullptr;
          }  ///should more be done here??

          else if (value2->types.i == 0){
            next = if_else->false_path; 
          }
          else {
            next = if_else->true_path;
          }
          if (value2 != nullptr){
        ram_free_value(value2);}
}



    else {
        cout << "unknown stmt type" << endl;
        
    }}
    
    return next;
    }
    

//unlinks the graph at current stmt
// params: cur - current stmt
//       next - pointer to next stmt (to be filled)
//       loop_body - pointer to loop body (to be filled)
//       true_p - pointer to true path (to be filled)/
//      false_p - pointer to false path (to be filled)

void unlink_pg(struct STMT* cur, struct STMT*& next, struct STMT*& loop_body, struct STMT*& true_p, struct STMT*& false_p){
  if (cur != nullptr) {
    
    if (cur->stmt_type == STMT_ASSIGNMENT) {
       struct STMT_ASSIGNMENT* assignment = cur->types.assignment;
       next = assignment->next_stmt;
       assignment->next_stmt = nullptr;
       
      
       
    } 

    else if (cur->stmt_type == STMT_FUNCTION_CALL) {
        // handle function call
         struct STMT_FUNCTION_CALL* func = cur->types.function_call;
           next = func->next_stmt;
           func->next_stmt = nullptr;
        
    } 

    else if (cur->stmt_type == STMT_PASS) {
          struct STMT_PASS* pass_c = cur->types.pass;
           next = pass_c->next_stmt;
           pass_c->next_stmt = nullptr;
        
    } 

   
    else if (cur->stmt_type == STMT_WHILE_LOOP) {
        struct STMT_WHILE_LOOP* while_l = cur->types.while_loop;
          loop_body = while_l->loop_body;
           next = while_l->next_stmt;
           
           while_l->next_stmt = nullptr;
           while_l->loop_body = nullptr;
        
    }


    else if (cur->stmt_type == STMT_IF_THEN_ELSE) {
        struct STMT_IF_THEN_ELSE* if_else = cur->types.if_then_else;
          true_p = if_else->true_path;
           false_p = if_else->false_path;
           next = if_else->next_stmt;
           
           if_else->true_path = nullptr;
           if_else->false_path = nullptr;
           if_else->next_stmt = nullptr;
        
    }

    else {
        cout << "unknown stmt type" << endl;
     
    }
}}
  
  ;
void relink_pg(struct STMT* cur, struct STMT* next,struct STMT* loop_body, struct STMT* true_p, struct STMT* false_p){
  if (cur != nullptr) {
    if (cur->stmt_type == STMT_ASSIGNMENT) {
       struct STMT_ASSIGNMENT* assignment = cur->types.assignment;
       assignment->next_stmt = next;
       
    } 

    else if (cur->stmt_type == STMT_FUNCTION_CALL) {
        // handle function call
         struct STMT_FUNCTION_CALL* func = cur->types.function_call;
           func->next_stmt = next;
        
    } 

    else if (cur->stmt_type == STMT_PASS) {
          struct STMT_PASS* pass_c = cur->types.pass;
           pass_c->next_stmt = next;
        
    } 
    else if (cur->stmt_type == STMT_WHILE_LOOP) {
        struct STMT_WHILE_LOOP* while_l = cur->types.while_loop;

           while_l->loop_body = loop_body;
           while_l->next_stmt = next;
        
    } 

    else if (cur->stmt_type == STMT_IF_THEN_ELSE) {
        struct STMT_IF_THEN_ELSE* if_else = cur->types.if_then_else;
          
           
           if_else->true_path = true_p;
           if_else->false_path = false_p;
           if_else->next_stmt = next;
        
    }



    else {
        cout << "unknown stmt type" << endl;
     
    }
}}

//prints variable type and value
// params: varname - name of variable to lookup
//         ram - pointer to RAM instance
// returns: nothing
void get_type(string varname, struct RAM*& ram){
        const char* name = varname.c_str();
        struct RAM_VALUE* value = ram_read_cell_by_name(ram, (char*)name);
        if (value == NULL){cout << "no such variable" << endl;}
        else{
          string type;
          
          if (value->value_type == RAM_TYPE_INT){int val = value->types.i; type = "int";
          cout << varname << " ("<<type<<"): " << val << endl;
          } 
          else if (value->value_type == RAM_TYPE_REAL){double val =value->types.d; type = "real";
          cout << varname << " ("<<type<<"): " << val << endl;
          }
          else if (value->value_type == RAM_TYPE_STR){string val = value->types.s; type = "str";
            cout << varname << " ("<<type<<"): " << val << endl;
          }
          else if (value->value_type == RAM_TYPE_PTR){int val = value->types.i; type = "ptr";
          cout << varname << " ("<<type<<"): " << val << endl;
          }
          else if (value->value_type == RAM_TYPE_BOOLEAN){int val = value->types.i; type = "bool";
          cout << varname << " ("<<type<<"): " << (val ? 1 : 0) << endl;
          }
          else if (value->value_type == RAM_TYPE_NONE){type = "none"; string value = "None";
          cout << varname << " ("<<type<<"): " << value << endl;
          }
          
          
          ram_free_value(value);
        }}
  
  void update_var(string varname, string newvalue, struct RAM*& ram){
        const char* name = varname.c_str();
        struct RAM_VALUE* value = ram_read_cell_by_name(ram, (char*)name);
        if (value == NULL){cout << "no such variable" << endl;}
        else{

          ram_free_value(value); //free the read value before overwriting

          struct RAM_VALUE value;

          if (newvalue[0] == '"' || newvalue[0] == '\'') {
              value.value_type = RAM_TYPE_STR;
              value.types.s = (char*) newvalue.c_str();
              ram_write_cell_by_name(ram, value, (char*) name);

          }
          else if (newvalue == "True" || newvalue == "False") {
              value.value_type = RAM_TYPE_BOOLEAN;
              value.types.i = newvalue == "True" ? 1 : 0;    /////numbers or??
              ram_write_cell_by_name(ram, value, (char*) name);
          }

          else {
            string s(1, newvalue[0]);



            try {stod(s); //try to convert to double to check if it's a number
              bool isReal = false;
              for (char c : newvalue) {
                  if (c == '.') {
                      isReal = true;
                      break;
                  }
              }
              if (isReal) {
                  value.value_type = RAM_TYPE_REAL;
                  value.types.d = stod(newvalue);
              } else {
                  value.value_type = RAM_TYPE_INT;
                  value.types.i = stoi(newvalue);
              }

              ram_write_cell_by_name(ram, value, (char*) name);
              
              }

              catch (...){
              value.value_type = RAM_TYPE_STR;
              value.types.s = (char*) newvalue.c_str();
              ram_write_cell_by_name(ram, value, (char*) name);
                      
              }


              
          }

          
  }}
  




/**
  * @brief constructor
  *
  * @param program Pointer to first stmt in the program graph
  */
Debugger::Debugger(struct STMT* program): program(program) ///should i??
{
  this->ram = ram_init();
}


/**
  * @brief destructor
  */
Debugger::~Debugger()
{
  ram_destroy(this->ram);
}

/**
  * @brief called to run one session of the debugger
  *
  * Run the debugger for one execution run of the given program.
  *
  * @return nothing
  */

void Debugger::run()
{
  string cmd;
   
  //
  // command loop until quit is entered:
  //
 
  struct STMT* cur = program;
  bool failed = false;
  while (true) { 
    struct STMT* next = nullptr;
    struct STMT* loop_body = nullptr;
    struct STMT* true_p = nullptr;
    struct STMT* false_p = nullptr;

   


    // struct STMT* loop_body = get_next_stmt(cur,ram);
    unlink_pg(cur, next,loop_body, true_p, false_p);
    cout << endl;
    cout << "Enter a command, h for help > " << endl;
    cin >> cmd;  //cosume command using getline??
    // getline(cin, cmd);
    
    if (cmd == "q") {
      relink_pg(cur, next,loop_body, true_p, false_p);
      break;
    }

    else if (cmd == "h") {
      cout << "Available commands:" << endl
        << "s -> Step to next stmt by executing current stmt" << endl
        << "p varname -> Print variable" << endl
        << "c varname newvalue -> Change variable to new value" << endl
        << "sm -> Show memory contents" << endl
        << "e -> Exection history" << endl
        << "w -> What line are we about to execute?" << endl
        << "pg -> Print program graph" << endl
        << "q -> Quit the debugger" << endl;
           //does this not mess up barsh further testing reqiuired???
          relink_pg(cur, next,loop_body, true_p, false_p);
        
         
    }

    else if (cmd == "s") {
      string hahaha;
      getline(cin, hahaha); // to consume the newline character after 's'

      if (failed == true){

        cout << "execution failed" << endl;
        relink_pg(cur, next,loop_body, true_p, false_p);
      


      }


      else{
      
      
      
      // struct STMT* cur_next = program;
      if (cur == nullptr){
        cout << "execution complete" << endl;
      }
      else{
      //   bool found = false;
      //  for (int line : lines_executed){
      //   if (line == cur->line){
      //     found = true;
      //   }
      //  }
       
      //  if (found == false){
       lines_executed.push_back(cur->line);
          
     
      struct ExecuteResult results = execute(cur, ram);
      if (results.Success == false) {
        
        // break;   //break or continue??
        failed = true;
        relink_pg(cur, next,loop_body, true_p, false_p);
      }

      else {
        

        // this->lines_executed.push_back(cur->line);
         relink_pg(cur, next,loop_body, true_p, false_p);

        cur = get_next_stmt(cur, ram);
      }

    }}}

    else if(cmd == "w"){

      if (cur == nullptr){
        cout<< "execution complete" << endl;
      }
      else{
        
      programgraph_print(cur);}
      relink_pg(cur, next,loop_body, true_p, false_p);
    }

    else if (cmd == "sm")
    {
      ram_print(this->ram);
      relink_pg(cur, next,loop_body, true_p, false_p);
    }
    else if (cmd == "p"){
        string varname;
        cin >> varname;
        get_type(varname, this->ram);
        relink_pg(cur, next,loop_body, true_p, false_p);
      }

    else if (cmd == "c"){
        string varname;

        cin >> varname;

        
        

        string newvalue;
        getline(cin, newvalue);   //should strip leading space
        // Remove leading whitespace
        newvalue.erase(newvalue.begin(), std::find_if_not(newvalue.begin(), newvalue.end(), ::isspace));
        // Remove trailing whitespace
        newvalue.erase(std::find_if_not(newvalue.rbegin(), newvalue.rend(), ::isspace).base(), newvalue.end());
        
        update_var(varname, newvalue, this->ram);

        relink_pg(cur, next,loop_body, true_p, false_p);

        


      }



    
    else if(cmd == "pg"){
      relink_pg(cur, next,loop_body, true_p, false_p);
      programgraph_print(program);
      
    }

    else if (cmd == "e"){
      cout << "Execution history:" << endl;
      for (int each : this->lines_executed){
        cout << each << " ";

      }
      cout << endl;
      relink_pg(cur, next,loop_body, true_p, false_p);
      
    }

    else {
      cout << "unknown command" << endl;
      relink_pg(cur, next,loop_body, true_p, false_p);
    }

  
  }//while
  
}
