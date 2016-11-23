// symstack.cpp
// Rashmi Sehgal rasehgal
// Vanessa Putnam vputnam
// May 23 2016
// Assignment 4

#include "symstack.h"
#include <iostream>
#include "astree.h"

size_t next_block = 0;

void symbol_stack::enter_block(){

   sym_stack.push_back(nullptr);
   block_stack.push_back(next_block);
   ++next_block;
}

void symbol_stack::leave_block(){
   sym_stack.pop_back();
   block_stack.pop_back();
}

void symbol_stack::define_ident(astree* node){
   if(sym_stack.back() == nullptr){
      sym_stack.back() = new symbol_table;
   }else{
      sym_insert(sym_stack.back(), node);
   }
}

symbol* symbol_stack::find_ident(astree* node){
   for(symbol_table* i : sym_stack){
      if(i == nullptr || i->empty())
         continue;
      symbol* sym = sym_lookup(i, node);
      if( sym != nullptr ){ return sym; }
   }
   return nullptr;
}

