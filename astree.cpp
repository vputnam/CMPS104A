// Rashmi Sehgal rasehgal
// Vanessa Putnam vputnam 
// 5/9/16
// Assignment 4


#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <set>
#include "astree.h"
#include "stringset.h"
#include "lyutils.h"



// need to define new elements in data structure 
astree* new_astree (int symbol, int filenr, 
int linenr,int offset, const char* lexinfo){
   astree* tree = new astree();
   tree->symbol = symbol;
   tree->filenr = filenr;
   tree->linenr = linenr;
   tree->offset = offset;
   tree->lexinfo = intern_stringset (lexinfo);
   tree->blocknr = 0;
   tree->attr = 0;
  DEBUGF ('f', "astree %p->{%d:%d.%d: %s: \"%s\"}\n",
           tree, tree->filenr, tree->linenr, tree->offset,
           get_yytname (tree->symbol), tree->lexinfo->c_str());
  return tree;
}

astree* adopt1 (astree* root, astree* child) {
   root->children.push_back (child);
   DEBUGF ('a', "%p (%s) adopting %p (%s)\n",
           root, root->lexinfo->c_str(),
           child, child->lexinfo->c_str());
   return root;
}

astree* adopt2 (astree* root, astree* left, astree* right) {
   adopt1 (root, left);
   adopt1 (root, right);
   return root;
}

astree* create_func (int symbol, astree* left,
 astree* mid, astree* right) {
   astree* root = new_astree( symbol, 
left->filenr, left->linenr, left->offset, "");
   adopt1(root, left);
   adopt2(root, mid, right);
   return root;
}

astree* create_proto (int symbol, 
astree* left, astree* right) {
   astree* root = new_astree( symbol, 
left->filenr, left->linenr, left->offset, "");
   adopt2(root, left, right);
   return root;
}


static string enum_tostring(size_t i){
   switch(i){
   case 0: return "void";
   case 1: return "bool";
   case 2: return "char";
   case 3: return "int";
   case 4: return "null";
   case 5: return "string";
   case 6: return "struct";
   case 7: return "array";
   case 8: return "function";
   case 9: return "variable";
   case 10: return "field";
   case 11: return "typeid";
   case 12: return "param";
   case 13: return "lval";
   case 14: return "const";
   case 15: return "vreg";
   case 16: return "vaddr";
   case 17: return "bitset_size";
   }
   return "invalid_enum";
}

string enum_bitset(attr_bitset a){
   string buf;
   for(int i=0; i<attr_bitset_size; ++i){
      if(a[i]){
         buf += enum_tostring(i);
         buf += " ";
      }
   }
   return buf;
}

static void dump_sym (FILE* outfile, astree* node) {
   const char* tname = get_yytname(node->symbol);
   if(strstr(tname, "TOK_") == tname) tname += 4;

   /*if(node->blocknr > 0){
         { fprintf (outfile, ""); }
   }*/
   /*astree* last_struct = nullptr;
   if( node->attr[attr_struct]){
     last_struct = node;
   }*/

   if( strcmp( tname, "DECLID") == 0 ){
   fprintf (outfile, " %s %s %zu.%zu.%zu {%zu} %s \n",
            tname,
            node->lexinfo->c_str(),
            node->filenr,
            node->linenr,
            node->offset,
            node->blocknr,
            enum_bitset(node->attr).c_str());
   }
   if(strcmp( tname, "TYPEID") == 0 ) {
      fprintf (outfile, " %s %s %zu.%zu.%zu {%zu} {%s} %s \n",
            tname,
            node->lexinfo->c_str(),
            node->filenr,
            node->linenr,
            node->offset,
            node->blocknr,
            node->lexinfo->c_str(),
            enum_bitset(node->attr).c_str());
   }

   if(strcmp( tname, "FIELD") == 0 ) {
   fprintf (outfile, " %s %s %zu.%zu.%zu {%zu} \"%s\"  %s \n",
            tname,
            node->lexinfo->c_str(),
            node->filenr,
            node->linenr,
            node->offset,
            node->blocknr,
            node->lexinfo->c_str(),
            enum_bitset(node->attr).c_str());

   }


}

static void dump_symtbl_rec (FILE* outfile, astree* root,
                             int depth) {
   if (root == NULL) return;
   //for (size_t i=1; i<= root->blocknr ; i++) fprintf(outfile, " ");
   dump_sym (outfile, root);
   for (size_t child = 0; child < root->children.size(); ++child) {
      dump_symtbl_rec (outfile, root->children[child],
                       depth + 1);
   }
}

void dump_symtbl (FILE* outfile, astree* root) {
   dump_symtbl_rec (outfile, root, 0);
   fflush (NULL);
}




static void dump_node (FILE* outfile, astree* node) {
   const char* tname = get_yytname(node->symbol);
   if(strstr(tname, "TOK_") == tname) tname += 4;
   fprintf (outfile, "%s \"%s\"  %zu.%zu.%zu {%zu} %s",
            tname, 
            node->lexinfo->c_str(),
            node->filenr, 
            node->linenr, 
            node->offset,
            node->blocknr,
            enum_bitset(node->attr).c_str());
      if(node->symbol == TOK_IDENT){
         fprintf(outfile, "(%zu.%zu.%zu)",
         node->deffilenr,
         node->deflinenr,
         node->defoffset);

      }
   bool need_space = false;
   for (size_t child = 0; child < node->children.size();
        ++child) {
      if (need_space) fprintf (outfile, " ");
      need_space = true;
   }
}

static void dump_astree_rec (FILE* outfile, astree* root,
                             int depth) {
   if (root == NULL) return;
   for (int i=1; i<=depth; i++) fprintf(outfile, "|  ");
   dump_node (outfile, root);
   fprintf (outfile, "\n");
   for (size_t child = 0; child < root->children.size(); ++child) {
      dump_astree_rec (outfile, root->children[child],
                       depth + 1);
   }
}

void dump_astree (FILE* outfile, astree* root) {
   dump_astree_rec (outfile, root, 0);
   fflush (NULL);
}

void yyprint (FILE* outfile, unsigned short toknum,
              astree* yyvaluep) {
   if (is_defined_token (toknum)) {
      dump_node (outfile, yyvaluep);
   }else {
      fprintf (outfile, "%s(%d)\n",
               get_yytname (toknum), toknum);
   }
   fflush (NULL);
}


void free_ast (astree* root) {
   while (not root->children.empty()) {
      astree* child = root->children.back();
      root->children.pop_back();
      free_ast (child);
   }
   DEBUGF ('f', "free [%p]-> %d:%d.%d: %s: \"%s\")\n",
           root, root->filenr, root->linenr, root->offset,
           get_yytname (root->symbol), root->lexinfo->c_str());
   delete root;
}

void free_ast2 (astree* tree1, astree* tree2) {
   free_ast (tree1);
   free_ast (tree2);
}




