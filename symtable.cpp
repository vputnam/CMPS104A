
// Rashmi Sehgal rasehgal
// Vanessa Putnam vputnam
// May 23 2016
// Assignment 4
// symtable.cpp 

#include "symtable.h"
#include <iostream>
#include "astree.h"

// check this !
symbol* new_symbol(astree* node){
   symbol* sym = new symbol();
   sym->filenr = node->filenr;
   sym->linenr = node->linenr;
   sym->offset = node->offset;
   sym->blocknr = node->blocknr;
   sym->attr = node->attr;
   sym->parameters = nullptr;
   return sym;
}

void sym_insert(symbol_table* st,  astree* node){
   symbol* sym = new_symbol(node);
   symbol_entry ent = symbol_entry( 
   const_cast<string*>(node->lexinfo), sym);
   st->insert(ent);
}


symbol* sym_lookup(symbol_table* st, astree* node){
   string* ident = const_cast<string*>(node->lexinfo);
   if(st->count(ident) <= 0){ return nullptr; }
   symbol_entry ent = *st->find(ident);
   node->deflinenr = ent.second->linenr;
   node->deffilenr = ent.second->filenr;
   node->defoffset = ent.second->offset; 
   return ent.second; 
}

