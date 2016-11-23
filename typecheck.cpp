// typecheck.cpp
// Rashmi Sehgal rasehgal
// Vanessa Putnam vputnam
// May 23 2016
// Assignment 4

#include <string.h>
#include "typecheck.h"


//helper function to determine if a node is a primitive type
bool is_prim (astree* node) {
   //we are checking one node's status
   //if the node is bool, char, or int, return is_primitive 
   if (node->attr[attr_bool] == 1) { return true; }
   else if (node -> attr[attr_char] == 1) { return true; }
   else if (node -> attr[attr_int] == 1) { return true; }
   return false;
}

// helper function to determine if a node is a reference type
bool is_ref (astree* node) {
   // if null, string, struct, array
   if (node->attr[attr_null] == 1) { return true; }    
   else if (node -> attr[attr_string] == 1) { return true; }
   else if (node -> attr[attr_struct] == 1) { return true; }
   else if (node -> attr[attr_array] == 1) { return true; }
   return false;
}

//helper function to determine if a node is a base type
//base type can be used to construct an array
bool is_base (astree* node) {
   //if bool, char, int, string, struct
   if (node->attr[attr_bool]) return true;
   else if (node->attr[attr_char]) return true;
   else if (node->attr[attr_int]) return true;
   else if (node->attr[attr_string]) return true;
   else if (node->attr[attr_struct]) return true;
   return false;
}

// helper function to determine if a node is "any" type
// "any" type is either prim or ref
bool is_any (astree* node) {
   // if prim or ref, true
   if (is_prim(node) == true) return true;
   if (is_ref(node) == true) return true;
   else return false; //if neither, false
}

// helper function to determine if two nodes are compatible types
bool are_compat(astree* node1, astree* node2) {
   // same type - prim
   if ((is_prim(node1) == true) && (is_prim(node2) == true)) 
   return true;
   // same type - ref
   if ((is_ref(node1) == true) && (is_ref(node2) == true)) 
   return true;
   // any, null
   if ((is_any(node1) == true) && (node2->attr[attr_null] == 1)) 
   return true;
   // null, any
   if ((node1->attr[attr_null] == 1) && (is_any(node2) == true)) 
   return true;
   // if one of the prev is for some fucking reason not the case
   return false;
}


void block_rec(astree* node, symbol_stack* s){
   if(node->symbol == TOK_BLOCK){ s->enter_block(); }
   node->blocknr = s->block_stack.back();
   for (size_t child = 0; child < node->children.size(); ++child) {
      block_rec(node->children[child], s);
   }
}

void sym_proto( astree* node, symbol_stack* s){

     if (node->children[0]->attr[attr_array] ==1) {
            node->children[0]->children[1]->attr[attr_array] = 1;
            node->children[0]->children[1]->attr[attr_function] = 1;
       if( node->children[0]->children[0]->symbol == 274 ){
          node->children[0]->children[1]->attr[attr_string] = 1;
       }
       if( node->children[0]->children[0]->symbol == 272 ){
          node->children[0]->children[1]->attr[attr_char] = 1;
       }
       if( node->children[0]->children[0]->symbol == 273 ){
          node->children[0]->children[1]->attr[attr_int] = 1;
       }
       if( node->children[0]->children[0]->symbol == 277 ){
          node->children[0]->children[1]->attr[attr_struct] = 1;
       }
     }

     node->children[0]->children[0]->attr[attr_function] = 1;
     sym_insert(s->sym_stack[0], node->children[0]->children[0]);
     s->enter_block();
     for(auto param : node->children[1]->children){
      param->children[0]->attr[attr_variable] = 1;
      param->children[0]->attr[attr_lval] = 1;
      param->children[0]->attr[attr_param] = 1;
      param->children[0]->blocknr = next_block;
      s->define_ident(param->children[0]);
      }
     s->leave_block();
}

void sym_function( astree* node, symbol_stack* s){

     if (node->children[0]->attr[attr_array] ==1) {
            node->children[0]->children[1]->attr[attr_array] = 1;
            node->children[0]->children[1]->attr[attr_function] = 1;
       if( node->children[0]->children[0]->symbol == 274 ){
          node->children[0]->children[1]->attr[attr_string] = 1;
       }
       if( node->children[0]->children[0]->symbol == 272 ){
          node->children[0]->children[1]->attr[attr_char] = 1;
       }
       if( node->children[0]->children[0]->symbol == 273 ){
          node->children[0]->children[1]->attr[attr_int] = 1;
       }
       if( node->children[0]->children[0]->symbol == 277 ){
          node->children[0]->children[1]->attr[attr_struct] = 1;
       }
     }

     node->children[0]->children[0]->attr[attr_function] = 1;
     sym_insert(s->sym_stack[0], node->children[0]->children[0]);
     s->enter_block();
     for(auto param : node->children[1]->children){
      param->children[0]->attr[attr_variable] = 1;
      param->children[0]->attr[attr_lval] = 1;
      param->children[0]->attr[attr_param] = 1;
      param->children[0]->blocknr = next_block;
      s->define_ident(param->children[0]); 
      }
     s->leave_block(); 
     block_rec(node->children[2], s);
}


void typecheck_logic( astree* node, symbol_stack* s, symbol_table* t){

   symbol *sym; 
   switch(node->symbol){
      case TOK_ROOT:
      case TOK_DECLID:
      case TOK_WHILE:
      case TOK_IF:
      case TOK_IFELSE:
      case TOK_RETURN:
      case TOK_INDEX:
         node->attr[attr_lval] = 1;
         node->attr[attr_vaddr] = 1;
         break;
      case TOK_EQ:
         if (node->children.size()==2) {

            if (is_any(node->children[0]) && 
            (is_any(node->children[1]))) {
               node->attr[attr_bool] = 1;
               node->attr[attr_vreg] = 1;
            }
         }
         break;
 
      case TOK_NE:
         if (node->children.size()==2) {

            if (is_any(node->children[0]) && 
            (is_any(node->children[1]))) {
               node->attr[attr_bool] = 1;
               node->attr[attr_vreg] = 1;
            }
         }
         break;
      case TOK_LT: //prim < prim -> bool vreg

         if(node->children.size()>=2) {
            if ((is_prim(node->children[0]) == true) &&
                (is_prim(node->children[1]) == true)) {
                  node->attr[attr_bool] = 1;
                  node->attr[attr_vreg] = 1;
            }
         }

         break;
      case TOK_PARAMLIST:
      case TOK_LE:
         if(node->children.size()>=2) {
            if ((is_prim(node->children[0]) == true) &&
                (is_prim(node->children[1]) == true)) {
                  node->attr[attr_bool] = 1;
                  node->attr[attr_vreg] = 1;
            }
         }break;
      case TOK_GT:
         if(node->children.size()>=2) {
            if ((is_prim(node->children[0]) == true) && 
                (is_prim(node->children[1]) == true)) {
                  node->attr[attr_bool] = 1;
                  node->attr[attr_vreg] = 1;
            }
         }break;
      case TOK_GE:
         if(node->children.size()>=2) {
            if ((is_prim(node->children[0]) == true) &&
                (is_prim(node->children[1]) == true)) {
                  node->attr[attr_bool] = 1;
                  node->attr[attr_vreg] = 1;
            }
         }break;
      case '+': //int + int -> int vreg
         if (node->children.size()==2) {
            if ((node->children[0]->attr[attr_int] ==1) &&
              (node->children[1]->attr[attr_int] ==1)) {
               node->attr[attr_vreg] = 1;
               node->attr[attr_int] = 1;
            } 
         }
         break;
      case '-': // int - int -> int vreg
         if (node->children.size()==2) {
            if ((node->children[0]->attr[attr_int] ==1) &&
              (node->children[1]->attr[attr_int] ==1)) {
               node->attr[attr_vreg] = 1;
               node->attr[attr_int] = 1;
            }
         }
         break;
      case '/': // int / int -> int vreg
         if (node->children.size()==2) {
            if ((node->children[0]->attr[attr_int] ==1) &&
              (node->children[1]->attr[attr_int] ==1)) {
               node->attr[attr_vreg] = 1;
               node->attr[attr_int] = 1;
            }
         }
         break;
      case '*':
         if (node->children.size()==2) {
            if ((node->children[0]->attr[attr_int] ==1) &&
              (node->children[1]->attr[attr_int] ==1)) {
               node->attr[attr_vreg] = 1; 
               node->attr[attr_int] = 1;
            }
         }
         break;
      case '%':
         if (node->children.size()==2) {
            if ((node->children[0]->attr[attr_int] ==1) &&
              (node->children[1]->attr[attr_int] ==1)) {
               node->attr[attr_vreg] = 1; 
               node->attr[attr_int] = 1;
            }
         }
         break;

      case '=': // any lval = any -> any vreg
         if (node->children.size()==2) {
           if ((node->children[0]->attr[attr_lval]==1) &&
               (is_any(node->children[0]) == true) && 
               (is_any(node->children[1]) == true)) {
              node->attr[attr_vreg] = 1;
              // set to be "any"? 
              } 
          } break;
      case TOK_POS: //+ int -> int vreg
         if (node->children.size()==1) {
            if (node->children[0]->attr[attr_int]==1) {
               node->attr[attr_vreg] = 1;
               node->attr[attr_int] = 1;
            }
         }
         break;
      case TOK_NEG:// - int -> int vreg
         if (node->children.size()==1) {
            if (node->children[0]->attr[attr_int]==1) {
               node->attr[attr_vreg] = 1;
               node->attr[attr_int] = 1;
            }
         }
         break;

      case '!': //! bool -> bool vreg
         if (node->children.size()>=1) {
            if (node->children[0]->attr[attr_bool]==1) {
               node->attr[attr_vreg] = 1;
               node->attr[attr_bool] = 1;
            }
         }
         break;
      case TOK_ORD: //ord char -> int vreg
         if (node->children.size()>=1) {
            if (node->children[0]->attr[attr_char]==1) {
               node->attr[attr_int] = 1;
               node->attr[attr_vreg] = 1;
            }
         }
         break; 
      case TOK_CHR: //chr int -> char vreg
         if (node->children.size()>=1) {
            if (node->children[0]->attr[attr_int]==1) {
               node->attr[attr_char] = 1;
               node->attr[attr_vreg] = 1;
            }
         }
         break;
      case TOK_NULL:
          if(!node->children.empty()) {
         node->attr[attr_null] = 1;
         node->attr[attr_const] = 1;
         }break; 
      case TOK_CHAR:
         if(!node->children.empty()) {
            node->children[0]->attr[attr_char] = 1;
            for(size_t i=0; i < attr_function; ++i){
               if(node->children[0]->attr[i])
                  node->attr[i] = 1;
            }
         }
         break;
      case '.':
         node->attr[attr_vaddr];
         node->attr[attr_lval];
         for(size_t i=0; i < attr_function; ++i){
            if(node->children[0]->attr[i])
               node->attr[i] = 1;
            }
         break;

      case TOK_NEWSTRING:
         node->attr[attr_string] = 1;
         node->attr[attr_vreg] = 1; 
         break; 

      case TOK_NEWARRAY:
         node->attr[attr_array] = 1;
         node->attr[attr_vreg] = 1;
         // set base type to match? 

         if (node->children[0]->attr[attr_bool])
         {node->attr[attr_bool]= 1;}
         else if (node->children[0]->attr[attr_char]) 
         {node->attr[attr_char]=1;}
         else if (node->children[0]->attr[attr_int]) 
         {node->attr[attr_int]=1;}
         else if (node->children[0]->attr[attr_string]) 
         {node->attr[attr_string]=1;}
         else if (node->children[0]->attr[attr_struct]) 
         {node->attr[attr_struct]=1;}
         break;

      case TOK_INTCON:
         //if(!node->children.empty()) {
           node->attr[attr_int] = 1;
           node->attr[attr_const] = 1;
        // }
         break;
      case TOK_CHARCON:
         //if(!node.empty()) {
           node->attr[attr_char] = 1;
           node->attr[attr_const] = 1;
         //}
         break;
      case TOK_STRINGCON:
         //if(!node->children.empty()) {
           node->attr[attr_string] = 1;
           node->attr[attr_const] = 1;
         //}
         break;
      case TOK_FALSE:
         node->attr[attr_bool] = 1;
         node->attr[attr_const] = 1;
         break;
      case TOK_TRUE:
         node->attr[attr_bool] = 1;
         node->attr[attr_const] = 1;
         break;

      case TOK_VOID:
        if(!node->children.empty()) {
            node->children[0]->attr[attr_void] = 1;
         }break; 
      case TOK_IDENT:
         sym = s->find_ident(node);
         if(sym == nullptr)
            sym = sym_lookup(t, node);
         if(sym == nullptr){
            errprintf("Error %d %d %d: Reference to undefined "
               "variable %s\n", node->filenr, node->linenr,
               node->offset, node->lexinfo->c_str());
            break;
         }
         node->attr = sym->attr;
         break;

      case TOK_CALL: 
            sym = sym_lookup(s->sym_stack[0],  node->children.back());
            if(sym == nullptr){
               /*errprintf("Error %d %d %d:"
                  "No matching function %s\n",
                  node->filenr, node->linenr, node->offset, 
                  node->children.back()->lexinfo->c_str());*/
               break;
            }
            for(size_t i=0; i < attr_function; ++i){
               if(sym->attr[i])
                  node->attr[i] = 1;   
            }
            break;
      case TOK_FIELD: 
         node->attr[attr_field] = 1;
         if(!node->children.empty()) {
            node->children[0]->attr[attr_field] = 1;
            for(size_t i=0; i < attr_function; ++i){
               if(node->children[0]->attr[i])
                node->attr[i] = 1;   
            }
          }
         break;
      case TOK_VARDECL:
         if(!node->children.empty()) {
            node->children[0]->children[0]->attr[attr_lval] = 1;
            node->children[0]->children[0]->attr[attr_variable] = 1;
            for(size_t i=0; i < attr_bitset_size; ++i){
               if(node->children[0]->attr[i])
                  node->attr[i] = 1;
            }
            if(s->find_ident(node->children[0]->children[0])){
               errprintf("Error %d %d %d: Duplicate declaration %s\n",
                  node->filenr, node->linenr, node->offset, 
                  node->children[0]->children[0]->lexinfo->c_str());}
            s->define_ident(node->children[0]->children[0]);
        }break;
      case TOK_STRUCT:

         {
            node->children[0]->attr[attr_struct] = 1;
            sym_insert(t,  node->children[0]);            
            symbol* sym = sym_lookup(t, node->children[0]);
            sym->fields = new symbol_table;
            for(auto child = node->children.begin()+1;
               child != node->children.end(); ++child){
               sym_insert(sym->fields,  *child);
            }
          }  break;
      case TOK_TYPEID:
        if(!node->children.empty()) {
            node->children[0]->attr[attr_typeid] = 1;
            for(size_t i=0; i < attr_bitset_size; ++i){
               if(node->children[0]->attr[i])
                  node->attr[i] = 1;
            }
         }
         break;
      case TOK_INT:
         if(!node->children.empty()) {
            node->children[0]->attr[attr_int] = 1;
            for(size_t i=0; i < attr_function; ++i){
               if(node->children[0]->attr[i])
                  node->attr[i] = 1;   
            }
         }
         break;

      case TOK_STRING: 
         if(!node->children.empty()) {
            node->children[0]->attr[attr_string] = 1;
            for(size_t i=0; i < attr_function; ++i){
              if(node->children[0]->attr[i])
                  node->attr[i] = 1;
            }
         } 
         break;
      case TOK_BOOL:
         if(!node->children.empty()) {
            node->children[0]->attr[attr_bool] = 1;
            for(size_t i=0; i < attr_function; ++i){
              if(node->children[0]->attr[i])
                  node->attr[i] = 1;
            }
         }
         break;

      case TOK_NEW:
         for(size_t i=0; i < attr_bitset_size; ++i){
         if(node->children[0]->attr[i])
              node->attr[i] = 1;
         }break;

      case TOK_ARRAY:
         //if string
         node->attr[attr_array] = 1;
          if (node->attr[attr_array] ==1) {
          node->children[1]->attr[attr_array] = 1;
          if( node->children[0]->symbol == 274 ){
             node->children[1]->attr[attr_string] = 1;
          }
          if( node->children[0]->symbol == 272 ){
             node->children[1]->attr[attr_char] = 1;
          }
          if( node->children[0]->symbol == 273 ){
             node->children[1]->attr[attr_int] = 1;
          }
          if( node->children[0]->symbol == 277 ){
             node->children[1]->attr[attr_struct] = 1;
            }
         }
         break;
      case TOK_BLOCK:
         block_rec(node, s);
         s->leave_block();
         break;
      case TOK_FUNCTION:
         sym_function( node, s);
         s->leave_block();
         break;
      case TOK_PROTOTYPE:
         sym_proto( node, s);
         break;

   default:
      errprintf("Invalid symbol %s\n", 
      get_yytname(node->symbol));
   }
}

void typecheck_rec( astree* ast, symbol_stack* s,
 symbol_table* t, size_t depth){
   for (size_t child = 0; child < ast->children.size(); ++child) {
      typecheck_rec ( ast->children[child], s, t, depth+1);
   }
   typecheck_logic(ast, s, t);
}

void typecheck( astree* root, symbol_stack* s, symbol_table* t){
  typecheck_rec(root, s, t, 0);
   while(!s->sym_stack.empty())
      s->sym_stack.pop_back();
}

