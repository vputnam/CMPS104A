
// Rashmi Sehgal rasehgal
// Vanessa Putnam vputnam
// May 23 2016
// Assignment 4
//symtable.h

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include <bitset>
#include "astree.h"

struct symbol;
using symbol_table = unordered_map<string*,symbol*>;

// not sure about entry delacration 
using symbol_entry = symbol_table::value_type;

struct symbol{
   attr_bitset attr;
   symbol_table* fields; // value of (key,value) pair  
   size_t filenr, linenr, offset;
   size_t blocknr;
   vector<symbol*>* parameters; // list if function, otherwsie null
};

symbol* new_symbol(astree* node);

void sym_insert(symbol_table* st,  astree* node);

symbol* sym_lookup(symbol_table* st, astree* node);

