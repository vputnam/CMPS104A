// Rashmi Sehgal rasehgal
// Vanessa Putnam vputnam 
// 5/9/16
// Assignment 4

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
#include <bitset>
#include <unordered_map>
using namespace std;

#include "auxlib.h"

enum { attr_void, attr_bool, attr_char, attr_int, attr_null,
attr_string, attr_struct, attr_array, attr_function,
attr_variable, attr_field, attr_typeid, attr_param, attr_lval,
attr_const, attr_vreg, attr_vaddr, attr_bitset_size,
};

using attr_bitset = bitset<attr_bitset_size>;

// need to fix astree
struct astree {
   int symbol;               // token code
   size_t filenr;            // index into filename stack
   size_t linenr;            // line number from source code
   size_t offset;            // offset of token with current line
   const string* lexinfo;    // pointer to lexical information
   vector<astree*> children; // children of this n-way node
   attr_bitset attr;
   size_t blocknr; 
   size_t deffilenr, deflinenr, defoffset;
   string vreg;
};

astree* new_astree (int symbol, int filenr, 
int linenr,int offset, const char* lexinfo);

// Append one child to the vector of children.
astree* adopt1 (astree* root, astree* child);

// Append two children to the vector of children.
astree* adopt2 (astree* root, astree* left, astree* right);

astree* create_func (int symbol, astree* left,
astree* mid, astree* right);
astree* create_proto (int symbol, astree* left, astree* right);

void dump_symtbl (FILE* outfile, astree* root);

// Dump an astree to a FILE.
void dump_astree (FILE* outfile, astree* root);

// Debug print an astree.
void yyprint (FILE* outfile, unsigned short toknum,
              astree* yyvaluep);

// Recursively free an astree.
void free_ast (astree* tree);

// Recursively free two astrees.
void free_ast2 (astree* tree1, astree* tree2);


#endif


