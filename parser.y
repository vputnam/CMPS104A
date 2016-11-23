%{
//Vanessa Putnam vputnam
//Rashmi Sehgal rasehgal
//5/9/16
//Assignment 4

%}

%{

// PARSER.Y for ASG3.

#include <cassert>


#include "lyutils.h"
#include "astree.h"

%}

%debug
%defines
%error-verbose
%token-table
%verbose

%token TOK_ROOT TOK_DECLID TOK_TYPEID TOK_INDEX
%token TOK_NEWSTRING
%token TOK_IFELSE TOK_RETURNVOID TOK_BLOCK TOK_VARDECL
%token TOK_CALL TOK_PARAMLIST TOK_PROTOTYPE

%token TOK_VOID TOK_BOOL TOK_CHAR TOK_INT TOK_STRING
%token TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_FALSE TOK_TRUE TOK_NULL TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON
%token TOK_INITDECL

%right TOK_IF TOK_ELSE
%right '='
%left TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left '+' '-'
%left '*' '/' '%'
%right TOK_POS TOK_NEG '!' TOK_ORD TOK_CHR
%left TOK_NEWARRAY '[' '.' TOK_FIELD TOK_FUNCTION
%nonassoc TOK_NEW
%nonassoc TOK_PAREN ')'

%start start

%%



start           : program               {yyparse_astree = $1; }
                ;

program         : program structdef     { $$ = adopt1 ($1, $2); }
                | program function      { $$ = adopt1 ($1, $2); }
                | program statement     { $$ = adopt1 ($1, $2); }
                | program error '}'     { $$ = $1; }
                | program error ';'     { $$ = $1; }
                |                       { $$ = new_parseroot (); }
                ;


structdef       : structhead '}'          {$$ = $1;}


structhead      :  structhead fielddecl ';' {$$ = adopt1($1, $2);} 
                | TOK_STRUCT TOK_IDENT '{'
                { $2->symbol = TOK_TYPEID;
                $$ = adopt1($1, $2); }
                ;

fielddecl       : basetype TOK_ARRAY TOK_IDENT
                { $3->symbol = TOK_FIELD; $$ = adopt2($2, $1, $3); }
                | basetype TOK_IDENT
                { $2->symbol = TOK_FIELD; $$ = adopt1($1, $2);}
                ;
basetype        : TOK_VOID      { $$ = $1; }
                | TOK_BOOL      { $$ = $1; }
                | TOK_CHAR      { $$ = $1; }
                | TOK_INT       { $$ = $1; }
                | TOK_STRING    { $$ = $1; }
                | TOK_IDENT     { $1->symbol = TOK_TYPEID, $$ = $1; }
                ;
function        : identdecl function_helper ')' block
                { $$= create_func(TOK_FUNCTION, $1, $2, $4);
                free_ast($3); }
                | identdecl function_helper  ')' ';'
                {$4->symbol = TOK_BLOCK;
                $$= create_proto(TOK_PROTOTYPE, $1, $2);
                free_ast($3); }
                ;
function_helper : function_helper ',' identdecl
                { $$ = adopt1($1, $3); free_ast($2);}
                | '(' identdecl
                {$1->symbol = TOK_PARAMLIST; $$ = adopt1($1, $2);}
                | '(' {$1->symbol = TOK_PARAMLIST; $$ = $1;}
                ;
identdecl       : basetype TOK_ARRAY TOK_IDENT
                { $3->symbol = TOK_DECLID; $$ = adopt2($2,$1,$3); }
                | basetype TOK_IDENT
                { $2->symbol = TOK_DECLID; $$ = adopt1($1,$2); }
                ;


blockhead       : blockhead statement {$$ = adopt1($1, $2);}
                | '{' statement {$$ = adopt1($1, $2);}
                ;

block           : blockhead '}' {$1->symbol = TOK_BLOCK; free_ast($2);}
                | '{' '}' { $1->symbol = TOK_BLOCK; free_ast($2);}

statement       : block { $$ = $1; }
                | vardecl { $$ = $1; }
                | while { $$ = $1; }
                | ifelse { $$ = $1; }
                | return { $$ = $1; }
                | expr ';' { $$ = $1; }
                ;
vardecl         : identdecl '=' expr ';'
                { $2->symbol = TOK_VARDECL;
                $$ = adopt2($2, $1, $3); free_ast($4);}
                ;
while           : TOK_WHILE '(' expr ')' statement
                { $$ = adopt2($1,$3,$5); }
                ;
ifelse          : TOK_IF '(' expr ')' statement TOK_ELSE statement
                {$1->symbol = TOK_IFELSE;
                $$ = adopt2($1, $3, $5);
                adopt1($1,$7); free_ast2($2,$4);}
                | TOK_IF '(' expr ')' statement %prec TOK_IF
                { $$ = adopt2($1,$3,$5); free_ast2($2,$4); }
                ;
return          : TOK_RETURN expr ';'
                { $$ = adopt1($1,$2); free_ast($3); }
                | TOK_RETURN ';'
                { $1->symbol = TOK_RETURNVOID; $$ = $1; free_ast($2);}
                ;
exprs           : exprs ',' expr  { $$ = adopt1($1,$3); free_ast($2);}
                | expr { $$ = $1; }
                ;
expr            : binop { $$ = $1; }
                | unop { $$ = $1; }
                | allocator { $$ = $1; }
                | call { $$ = $1; }
                | '(' expr ')' { $$ = $2; free_ast2($1,$3);}
                | variable { $$ = $1; }
                | constant { $$ = $1; }
                ;
binop           :expr '=' expr    { $$ = adopt2($2, $1, $3);}
                |expr TOK_EQ expr { $$ = adopt2($2, $1, $3);}
                |expr TOK_NE expr { $$ = adopt2($2, $1, $3);}
                |expr TOK_LT expr { $$ = adopt2($2, $1, $3);}
                |expr TOK_LE expr { $$ = adopt2($2, $1, $3);}
                |expr TOK_GT expr { $$ = adopt2($2, $1, $3);}
                |expr TOK_GE expr { $$ = adopt2($2, $1, $3);}
                |expr '+' expr { $$ = adopt2($2, $1, $3);}
                |expr '-' expr { $$ = adopt2($2, $1, $3);}
                |expr '*' expr { $$ = adopt2($2, $1, $3);}
                |expr '/' expr { $$ = adopt2($2, $1, $3);}
                |expr '%' expr { $$ = adopt2($2, $1, $3);}
                ;
unop            :'+' expr %prec TOK_POS
                {$1->symbol = TOK_POS; $$ = adopt1($1, $2);}
                |'-' expr %prec TOK_NEG
                {$1->symbol = TOK_NEG; $$ = adopt1($1, $2);}
                |'!' expr {$$ = adopt1($1, $2);}
                | TOK_ORD expr {$$ = adopt1($1, $2);}
                | TOK_CHR expr {$$ = adopt1($1, $2);}
                ;
allocator       : TOK_NEW TOK_IDENT '(' ')'
                { $2->symbol = TOK_TYPEID;
                $$ = adopt1($1,$2); free_ast2($3,$4); }
                | TOK_NEW TOK_STRING '(' expr ')'
                { $2->symbol = TOK_NEWSTRING;
                $$ = adopt1($2,$4); free_ast2($3,$5);}
                | TOK_NEW basetype '[' expr ']'
                { $1->symbol = TOK_NEWARRAY;
                $$ = adopt2($1,$2,$4); free_ast2($3,$5); }
                ;
call            : TOK_IDENT '(' exprs ')'
                { $2->symbol = TOK_CALL;
                $$ = adopt2($2, $1, $3); free_ast($4);}
                | TOK_IDENT '(' ')'
                {$2->symbol = TOK_CALL;
                $$ = adopt1($2, $1); free_ast($3);}
                ;
variable        : TOK_IDENT {$$ = $1;}
                | expr '[' expr ']'
                {$2->symbol = TOK_INDEX;
                $$ = adopt2($2, $1, $3); free_ast($4);}
                | expr '.' TOK_IDENT
                {$3->symbol = TOK_FIELD;
                $$ = adopt2($2, $1, $3);}
                ;
constant        : TOK_INTCON { $$ = $1; }
                | TOK_CHARCON { $$ = $1; }
                | TOK_STRINGCON { $$ = $1; }
                | TOK_FALSE  { $$ = $1; }
                | TOK_TRUE { $$ = $1; }
                | TOK_NULL { $$ = $1; }
                ;
%%
const char *get_yytname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}
bool is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}
