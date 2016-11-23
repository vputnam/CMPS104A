// emitter.n/cpp



#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "astree.h"

#include "emitter.h"

#include "auxlib.h"

#include "lyutils.h"



void emit (FILE* file,astree* root);



void emit_insn (FILE* file,const char* opcode, const char* operand) {

   fprintf (file, "%s%s ",

            opcode, operand);

}



void postorder (FILE* file,astree* tree) {

   assert (tree != NULL);

   for (size_t child = 0; child < tree->children.size(); ++child) {

      emit (file,tree->children.at(child));

   }

}







void postorder_emit_struct(FILE* file,astree* tree){

   // print structhead

   fprintf(file,"struct %s {\n", tree->children[0]->lexinfo->c_str());

   // print all children

   for (size_t child = 1; child < tree->children.size(); ++ child ) {

      fprintf(file,"%8s", "");

      emit(file,tree->children.at(child));

   }

   // print struct end bracket

   fprintf(file,"\n%s\n", "}");

}







void postorder_emit_field(FILE* file,astree* fieldchild) {

 // printf("I am here"); //debug 

  //for each field declaration of a struct, print 

  fprintf(file," %s\n", fieldchild->lexinfo->c_str());

}







void emit_exprs(FILE* file,astree* node, const char* opcode) {

   if (!node->children.empty()) {

      emit(file,node->children[0]);

      fprintf(file,"%s ", opcode);

      emit(file,node->children[1]);

      fprintf(file,"\n");

   }

}







void postorder_emit_stmts (FILE* file,astree* tree) {

   postorder (file,tree);

}



//void postorder_emit_oper (astree* tree, const char* opcode) {



  // postorder (tree);



   //emit_if");nsn (opcode, "", tree);



//}



void postorder_emit_semi (FILE* file,astree* tree) {

   postorder (file,tree);

   emit_insn (file,"", "");

}



void emit_push (FILE* file,astree* tree, const char* opcode) {

   emit_insn (file, opcode, tree->lexinfo->c_str());

}



void emit_assign (FILE* file,astree* tree) {

   assert (tree->children.size() == 2);

   astree* left = tree->children.at(0);

   emit (file,tree->children.at(1));

   if (left->symbol != TOK_IDENT) {

      // this used to be an error message, 
      // you can put it back if you'd like 

      errprintf ("%s\n",

                    "left operand of '=' not an identifier");

   }else{

      emit_insn (file, "popvar", left->lexinfo->c_str());

   }

}





void emit_self(FILE* file,astree* node) {

   // print out self as per usual

   fprintf(file,"%s ", node->lexinfo->c_str());

   if (!node->children.empty()) {

      emit(file,node->children.at(0));

   }

}







void emit_decl(FILE* file,astree* eqnode) {

   emit(file,eqnode->children.at(0));

   fprintf(file,"%s ", eqnode->lexinfo->c_str());

   emit(file, eqnode->children.at(1));

   fprintf(file,"\n");

}







/*void emit_array(FILE* file,astree* arrnode){

   // somewhere this will be helpful

}*/







void emit_function(FILE* file,astree* funcroot) {

   for (size_t child = 0; 
       child < funcroot->children.size(); ++ child ) {

      emit(file,funcroot->children.at(child));

   }

}



void emit_while(FILE* file,astree* node){

      fprintf(file,"while_%zu_%zu_%zu:;\n",

      node->filenr, node->linenr, node->offset);

   switch(node->children[0]->symbol){

      case TOK_IDENT:

      case TOK_CHARCON:

      case TOK_INTCON:

         break;

      default:

         fprintf(file,"        ");

         emit_exprs(file, node->children[0], " ");

   }

   fprintf(file,"        ");

   fprintf(file,

      "if (!%s) goto break_%zu_%zu_%zu;\n", 

      node->children[0]->vreg.c_str(), 

      node->children[1]->filenr,

      node->children[1]->linenr,

      node->children[1]->offset);

   emit(file, node->children[1]);

   fprintf(file,"        ");

   fprintf(file,

      "goto while_%zu_%zu_%zu;\n", 

      node->filenr, 

      node->linenr, 

      node->offset);

   fprintf(file,

      "break_%zu_%zu_%zu):;\n",

      node->children[1]->filenr,

      node->children[1]->linenr,

      node->children[1]->offset);

}



void emit_param(FILE* file,astree* node) {

   fprintf(file,"%s\n%8s", "(", "");

   emit(file,node->children[0]);

   for (size_t child = 1; child < node->children.size(); ++ child ) { 

      fprintf(file,",\n%8s", "");

      emit(file,node->children[child]);

   }

   fprintf(file,"%s\n", ")");

}







void emit_block(FILE* file,astree* curblock) {

   //printf("{\n%8s", "");

   fprintf(file,"{\n");

   for (size_t child = 0; 
        child < curblock->children.size(); ++ child ) {

      emit(file,curblock->children[child]);

      //printf("\n%8s", "");

   }

   fprintf(file, "\n}\n");

}


void emit_call(FILE* file,astree* node) {
   if(!(strcmp( node->children[0]->lexinfo->c_str(), "void") == 0 ||
      strcmp( node->children[0]->lexinfo->c_str(), "int") == 0 ||
      strcmp( node->children[0]->lexinfo->c_str(), "char") == 0 || 
      strcmp( node->children[0]->lexinfo->c_str(), "string") == 0 || 
      strcmp( node->children[0]->lexinfo->c_str(), "[]") == 0 
   ))
   fprintf(file, "%s(", node->children[0]->lexinfo->c_str());
   
   for (size_t child = 1; child<node->children.size(); ++child) {
      fprintf(file, "%s", node->children[child]->lexinfo->c_str());
   }fprintf(file, ");\n");
   
}


void emit (FILE* file,astree* tree) {

   switch (tree->symbol) {

      case TOK_ROOT  : 

         postorder_emit_stmts (file,tree);

         break;

      case TOK_STRUCT:  

         postorder_emit_struct(file,tree);

         break;

      case TOK_INT:

         emit_self(file,tree);

         break;

      case TOK_BOOL:

         fprintf(file, "%s ", "char");

         if (!tree->children.empty()) {

            emit(file,tree->children.at(0));

         }

         break;

      case TOK_CHAR:

         emit_self(file,tree);

         break;

      case TOK_STRING:

         fprintf(file,"%s ", "char*");

         if (!tree->children.empty()) {

            emit(file,tree->children.at(0));

         }

         break;

      case TOK_FIELD:

         postorder_emit_field(file,tree);

         break;

      case TOK_TYPEID:

         fprintf(file,"struct %s ", tree->lexinfo->c_str());

         break;

      case TOK_ARRAY: //???

      case TOK_FUNCTION:

         emit_function(file,tree);

         break;

      case TOK_BLOCK:

         emit_block(file,tree);

         break;

      case TOK_PARAMLIST:

         emit_param(file,tree);

         break;

      case TOK_IF: 

      case TOK_WHILE:

         emit_while(file,tree);

      case TOK_IFELSE:

      case TOK_DECLID:

         emit_self(file,tree);

         break;

      case TOK_VARDECL:

         emit_decl(file,tree);

         break;      

      case TOK_RETURN:

      case TOK_PROTOTYPE:

      case TOK_CALL:
         emit_call(file,tree);
         break;
      case ';'   : 

         //postorder_emit_semi (tree);

         break;

      case '='   :

         emit_assign (file,tree);

         break;

      case '+'   :

         emit_exprs (file,tree, "+");

         break;

      case '-'   : 

         emit_exprs (file,tree, "-");

         break;



      case '*'   :

         emit_exprs (file,tree, "*");

         break;

      case '/'   :

         emit_exprs (file,tree, "/");

         break;

      case TOK_POS   :

         //postorder_emit_oper (tree, "pos");

         break;

      case TOK_NEG   :

         //postorder_emit_oper (tree, "neg");

         break;

      case TOK_IDENT :

         emit_push (file,tree, "");       

         break;

      case TOK_EQ:
       emit_self(file,tree);
       break;

      case TOK_NE:

      case TOK_LE:

      case TOK_GE:

      case TOK_LT:

      case TOK_GT:

      case TOK_INTCON:

         emit_push(file,tree, "pushnum");

         break;

      case TOK_CHARCON:

      case TOK_STRINGCON: 

         emit_self(file,tree);

         break;

      //case TOK_NUMBER: emit_push (tree, "pushnum");       break;

      default    :

         errprintf("Invalid symbol %s\n",

           get_yytname(tree->symbol));

   }

}



string mangle_ident(astree* node){

   string builder = *node->lexinfo + " ";

   astree* child = nullptr;

   if (!node->children.empty())

      child = node->children[0];

   else 

      child = node;

   //global

   if (node->blocknr == 0){

      builder += "__";

      builder += *child->lexinfo;

   }

   //local

   else{

      builder += "_";

      builder += child->blocknr;

      builder +="_";

      builder += *child->lexinfo;

   }

   builder += ";\n";

   return builder;

}





void emit_sm_code (FILE* file, astree* tree) {

   if (tree) {

      //struct first

      for (size_t child = 0; child<tree->children.size(); ++child) {

         if (tree->children[child]->symbol == TOK_STRUCT) {

            emit(file,tree);  

         }

      } 

      //then string constants

      for (size_t child = 0; child<tree->children.size(); ++child) {

         if (tree->children[child]->symbol == TOK_STRINGCON) {

            emit(file,tree);

         }

      } 

      //global variable decls

      for (size_t child = 0; child<tree->children.size(); ++child) {

         if (tree->children[child]->symbol == TOK_VARDECL) {

            fprintf(file, "        ");

            tree->children[child]->vreg = 
            mangle_ident(tree->children[child]->children[0]);

            fprintf(file, tree->children[child]->vreg.c_str());

            //fprintf(file, "in if");

         }

      } emit(file, tree);

      /*   else if (tree->children[child]->symbol != TOK_FUNCTION &&

                  tree->children[child]->symbol != TOK_PROTOTYPE){

            //rest of expr and stmt in global scope

            fprintf(file, "in elseif");

            emit(file, tree);

         }

      }

      //function with one param per line

      for (size_t child = 0; child<tree->children.size(); ++child) {

         if (tree->children[child]->symbol == TOK_FUNCTION) {

            emit(file,tree);

        }

      }*/

   }

}
