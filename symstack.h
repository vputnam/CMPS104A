// symstack.h
// Rashmi Sehgal rasehgal
// Vanessa Putnam vputnam
// May 23 2016
// Assignment 4
#include "symtable.h"

extern size_t next_block;

class symbol_stack{
 public:
   vector <size_t> block_stack;
   vector<symbol_table*> sym_stack; // vector of symbol tables
   void enter_block();
   void leave_block();
   void define_ident(astree* node);
   symbol* find_ident(astree* node);
};


