/* Use the expression datatype defined in expressions.ml: */
%{
    open Mp8common
%}

/* Define the tokens of the language: */
%token <int> INT
%token <float> FLOAT
%token <bool> BOOL
%token <string> STRING IDENT
%token <(int*int)> OPCOM CLCOM SCLCOM
%token DBLSEMI PLUS MINUS TIMES DIV DPLUS DMINUS DTIMES DDIV CARAT EXP LT GEQ LEQ GT
       EQUALS AND OR PIPE ARROW DCOLON LET REC SEMI IN IF THEN ELSE FUN
       LBRAC RBRAC LPAREN RPAREN COMMA
       UNIT ERROR EOF
       TRY WITH UNDERSCORE RAISE 
       HEAD TAIL PRINT NEG FST SND

/* Define the "goal" nonterminal of the grammar: */
%start main
%type <Mp8common.toplvl> main

%%

pat:
  | UNDERSCORE	{ None }
  | INT		{ Some $1 }


main:
    expression DBLSEMI      			{ (Anon $1) }
  | LET IDENT EQUALS expression	DBLSEMI 	{ TopLet ($2,$4) }
  | LET REC IDENT IDENT EQUALS expression DBLSEMI  	{ (TopRec ($3, $4, $6)) }

expression: 
  | rec_expression { $1 }

atomic_expression:
  | IDENT			{ VarExp $1 }

const_expression:
  | INT             { ConExp (Int $1) }
  | UNIT            { ConExp (Unit) }
  | BOOL            { ConExp (Bool $1) }
  | FLOAT           { ConExp (Float $1) }
  | STRING          { ConExp (String $1) }

paren_expression:
  | LPAREN expression COMMA expression RPAREN { BinExp (Comma, $2, $4) }
  | LPAREN expression RPAREN { $2 }

noprec_exp:
  | const_expression { $1 }
  | atomic_expression { $1 }
  | monop_expression { $1 }
  | paren_expression { $1 }

app_expression:
  | expression expression { AppExp($1, $2) }
  | paren_expression const_expression { AppExp($1, $2) }
  | noprec_exp { $1 }
  
monop_expression:
  | HEAD expression { MonExp (Head, $2)  }
  | TAIL expression { MonExp (Tail, $2) }
  | PRINT expression  { MonExp (Print, $2) }
  | NEG expression  { MonExp (Neg, $2) }
  | FST expression { MonExp (Fst, $2) }
  | SND expression { MonExp (Snd, $2) }

infix_expression:
  | infix_expression PLUS muldiv_exp { BinExp (Add, $1, $3) } 
  | infix_expression DPLUS muldiv_exp { BinExp (FAdd, $1, $3) }
  | infix_expression MINUS muldiv_exp { BinExp (Sub, $1, $3) }
  | infix_expression DMINUS muldiv_exp { BinExp (FSub, $1, $3) }
  | expression CARAT muldiv_exp { BinExp (Concat, $1, $3) }
  | muldiv_exp { $1 }

muldiv_exp:
  | muldiv_exp TIMES raise_expression { BinExp (Mul, $1, $3) }
  | muldiv_exp DTIMES raise_expression { BinExp (FMul, $1, $3) }
  | muldiv_exp DIV raise_expression { BinExp (Div, $1, $3) }
  | muldiv_exp DDIV raise_expression { BinExp (Div, $1, $3) }
  | raise_expression { $1 }

bool_expression:
  | cons_expression GT cons_expression { BinExp (Less, $3, $1) }
  | cons_expression LT cons_expression { BinExp (Less, $1, $3) }
  | cons_expression EQUALS cons_expression { BinExp (Eq, $1, $3) }
  | cons_expression GEQ cons_expression { IfExp (BinExp(Less, $3, $1), ConExp(Bool true), BinExp(Eq, $1, $3)) }
  | cons_expression LEQ cons_expression { IfExp (BinExp(Less, $1, $3), ConExp(Bool true), BinExp(Eq, $1, $3)) }
  | cons_expression { $1 }

cons_expression:
  | infix_expression DCOLON cons_expression { BinExp(Cons, $1, $3) }
  | LBRAC RBRAC { ConExp (Nil) }
  | LBRAC infix_expression RBRAC { BinExp(Cons, $2, ConExp(Nil)) }
  | infix_expression { $1 }

let_expression:
  | LET IDENT EQUALS fun_expression IN fun_expression { LetExp($2, $4, $6) }
  | fun_expression { $1 }

rec_expression:
  | LET REC IDENT IDENT EQUALS let_expression IN expression { RecExp($3, $4, $6, $8) }
  | let_expression { $1 }

fun_expression:
  | FUN IDENT ARROW if_expression { FunExp($2, $4) }
  | if_expression { $1 }

if_expression:
  | IF or_expression THEN expression ELSE expression { IfExp($2, $4, $6) }
  | or_expression { $1 }

and_expression:
  | and_expression AND bool_expression { IfExp($1, $3, ConExp(Bool false)) }
  | bool_expression AND bool_expression { IfExp($1, $3, ConExp(Bool false)) }
  | bool_expression { $1 }

or_expression:
  | or_expression OR expression { IfExp($1, ConExp(Bool true), $3) }
  | and_expression OR expression { IfExp($1, ConExp(Bool true), $3) }
  | and_expression { $1 }

exp_expression:
  | noprec_exp EXP exp_expression { BinExp(Exp, $1, $3) }
  | noprec_exp { $1 }

raise_expression:
  | RAISE app_expression { RaiseExp($2) }
  | app_expression { $1 }
  | exp_expression { $1 }

