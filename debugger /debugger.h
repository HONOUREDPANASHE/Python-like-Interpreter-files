/*debugger.h*/

/**
  * @brief simplified version of gdb for nuPython
  *
  * Debugger for nuPython, C++ edition! Provides a simple gdb-like
  * interface, with support for step-by-step execution of nuPython.
  * Uses nuPython interpreter as execution engine.
  *
  * @note YOUR NAME
  *
  * @note original version by Ata Guvendi
  * @note turned into assignment by Prof. Hummel
  * @note Northwestern University
  */

#pragma once
#include <vector>
#include <iostream>
#include <string>
#include "ram.h"
#include "execute.h"
#include "programgraph.h"

using namespace std;


class Debugger
{
private:
  struct STMT* program; ///< pointer to first stmt in program graph
  struct RAM* ram;              ///< RAM instance for program execution
  vector<int> lines_executed;
public:
  /**
    * @brief constructor
    *
    * @param program Pointer to first stmt in the program graph
    */
  Debugger(struct STMT* program);

  /**
    * @brief destructor
    */
  ~Debugger();
  
  /**
    * @brief called to run one session of the debugger
    *
    * Run the debugger for one execution run of the given program.
    *
    * @return nothing
    */
  void run();
};

