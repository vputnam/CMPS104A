// $Id: cppstrtok.cpp,v 1.4 2016-03-24 14:34:10-07 - - $

// Rashmi Sehgal rasehgal
// // Vanessa Putnam vputnam 
// // 5/23/16
// // Assignment 4

#include <string>
using namespace std;

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <fstream>
#include <assert.h>
#include <errno.h>

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "auxlib.h"
#include "stringset.h"
#include "lyutils.h"
#include "yyparse.h"
#include "typecheck.h"
#include "astree.h"
#include "emitter.h"

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;
string cpp_command;

//int yflag = 0;
//int lflag = 0;
int Dflag = 0;
char *atvalue = NULL;
char *Dvalue = NULL;

FILE* tokfile;
FILE* astfile;
FILE* symfile;
FILE* oilfile;

// Chomp the last character from a buffer if it is delim.

void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

// Run cpp against the lines of the file.

void cpplines (FILE* pipe, char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy (inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == NULL) break;
      chomp (buffer, '\n');
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, filename);
      if (sscanf_rc == 2) {
         continue;
      }
      char* savepos = NULL;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if (token == NULL) break;
         intern_stringset (token);
      }
      ++linenr;
   }
}



void scan_opt(int argc, char **argv) 
{
  int c;
  opterr = 0;

  while ((c = getopt (argc, argv, "ly@:D:")) != -1){
    switch (c)
      {
      case 'y':
        yydebug = 1;
        break;
      case 'l':
        yy_flex_debug = 1;
        break;
      case '@':
        atvalue =  optarg;
        break;
      case 'D':
        Dflag = 1;
        Dvalue = optarg;
        break;
      default:
        abort ();
      }
  }
}

// Open a pipe from the C preprocessor.
// Exit failure if can't.
// Assigns opened pipe to FILE* yyin.
void cpp_popen (char* filename) {
   cpp_command = CPP + " " + filename;
   yyin = popen (cpp_command.c_str(), "r");
   if (yyin == NULL) {
      syserrprintf (cpp_command.c_str());
      exit (get_exitstatus());
   }else {
      cpplines (yyin, filename);
      if (yy_flex_debug) {
        /*fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
                  cpp_command.c_str(), fileno (yyin));*/
      }
      lexer_newfilename (cpp_command.c_str());
   }
   fclose(yyin);
   yyin = popen (cpp_command.c_str(), "r");
}

void cpp_pclose() {
   int pclose_rc = pclose (yyin);
   eprint_status (cpp_command.c_str(), pclose_rc);
   if (pclose_rc != 0) set_exitstatus (EXIT_FAILURE);
}

int main (int argc, char** argv) {
   set_execname (argv[0]);
   scan_opt(argc, argv);


   if(atvalue != NULL){
      set_debugflags(atvalue);
   }

   char* filename = argv[argc -1];

   if (!strstr(filename, ".oc")){
      fprintf(stderr, "File %s must be an .oc file\n", filename);
        set_exitstatus(1);
   }

   FILE* tmp = fopen(filename, "r");
   if (tmp == NULL){
      fprintf(stderr, "File not found\n");
      exit(1);
   }
   fclose(tmp);
   
   // create .str and .tok files
   string file_string = filename;
   int end = file_string.find_last_of(".");

   string file_rawname = file_string.substr(0, end);
   file_rawname += ".str";
   string file_out = file_rawname;
   std::ofstream out (file_out, std::ofstream::out);

   string file_rawname2 = file_string.substr(0, end);
   file_rawname2 += ".tok";
   string file_out2 = file_rawname2;
   tokfile = fopen(file_rawname2.c_str(), "w");

   string file_rawname3 = file_string.substr(0, end);
   file_rawname3 += ".ast";
   string file_out3 = file_rawname3;
   astfile = fopen(file_rawname3.c_str(), "w");

   string file_rawname4 = file_string.substr(0, end);
   file_rawname4 += ".sym";
   string file_out4 = file_rawname4;
   symfile = fopen(file_rawname4.c_str(), "w");

   string file_rawname5 = file_string.substr(0, end);
   file_rawname5 += ".oil";
   string file_out5 = file_rawname5;
   oilfile = fopen(file_rawname5.c_str(), "w");

   cpp_popen(filename);
       
   yyparse();
   //while(yylex() != YYEOF);
   fclose(tokfile);

   // dump
   dump_stringset(out);

   symbol_stack* s = new symbol_stack;
   symbol_table* t = new symbol_table;
   s->sym_stack.push_back(new symbol_table);
   typecheck( yyparse_astree, s, t);

   dump_symtbl(symfile, yyparse_astree);
   dump_astree (astfile, yyparse_astree);
   emit_sm_code (oilfile, yyparse_astree);
   cpp_pclose();
   return EXIT_SUCCESS;
}
          
