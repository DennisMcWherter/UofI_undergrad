{

open Mp7parse;;
open Mp7common;;

let line_count = ref 1
let char_count = ref 1

let cinc n = char_count := !char_count + n
let linc n = line_count := (char_count := 1; !line_count + n)

}

(* You can assign names to commonly-used regular expressions in this part
   of the code, to save the trouble of re-typing them each time they are used *)

let numeric = ['0' - '9']
let lowercase = ['a' - 'z']
let letter =['a' - 'z' 'A' - 'Z' '_']
let rest = ['0' - '9' 'a' - 'z' 'A' - 'Z' '_' '\'']
let first = ['0' - '2']
let last  = ['0' - '5']

rule token = parse
  | [' ' '\t' '\n'] { token lexbuf }  (* skip over whitespace *)
  | eof             { EOF } 
  | ";;"            { DBLSEMI }
  | '+'             { PLUS }
  | '-'             { MINUS }
  | '*'             { TIMES }
  | '/'             { DIV }
  | "+."            { DPLUS }
  | "-."            { DMINUS }
  | "/."            { DDIV }
  | '^'             { CARAT }
  | "**"            { EXP }
  | '<'             { LT }
  | '>'             { GT }
  | "<="            { LEQ }
  | ">="            { GEQ }
  | '='             { EQUALS }
  | "&&"            { AND }
  | "||"            { OR }
  | '|'             { PIPE }
  | "->"            { ARROW }
  | "::"            { DCOLON }
  | "let"           { LET }
  | "rec"           { REC }
  | ';'             { SEMI }
  | "in"            { IN }
  | "if"            { IF }
  | "then"          { THEN }
  | "else"          { ELSE }
  | "fun"           { FUN }
  | '['             { LBRAC }
  | ']'             { RBRAC }
  | '('             { LPAREN }
  | ')'             { RPAREN }
  | ','             { COMMA }
  | '_'             { UNDERSCORE }
  | numeric+ as x             { INT(int_of_string(x)) }
  | numeric+'.'numeric* as x  { FLOAT(float_of_string(x)) }
  | "true"          { BOOL true }
  | "false"         { BOOL false }
  | "()"            { UNIT }
  | lowercase+rest* as x      { IDENT(x) }
  | "//"            { eatline lexbuf }
  | "(*"            { comment 1 lexbuf }
  | '"'             { lexStr "" lexbuf }
and eatline = parse
  | ['\r' '\n']     { token lexbuf }
  | eof             { EOF }
  | _               { eatline lexbuf }
and comment depth = parse
  | "**)"           { token lexbuf }
  | "*)"            { if depth = 1 then token lexbuf else comment (depth - 1) lexbuf }
  | "(*"            { comment (depth + 1) lexbuf }
  | eof             { raise (Failure "unmatched comment" ) }
  | _               { comment depth lexbuf }
and lexStr str = parse
  | "\\\\"          { lexStr (str ^ (String.make 1 '\\')) lexbuf }
  | "\'"            { lexStr (str ^ (String.make 1 '\'')) lexbuf }
  | "\\\""          { lexStr (str ^ (String.make 1 '"')) lexbuf }
  | "\\t"            { lexStr (str ^ (String.make 1 '\t')) lexbuf }
  | "\\n"            { lexStr (str ^ (String.make 1 '\n')) lexbuf }
  | "\\r"            { lexStr (str ^ (String.make 1 '\r')) lexbuf }
  | "\\"(first last last as x) { lexStr (str ^ (String.make 1 (char_of_int (int_of_string x)))) lexbuf }
  | '"'             { STRING (str) }
  | _  as x         { lexStr (str ^ (String.make 1 x)) lexbuf }

(* do not modify this function: *)
{ let lextest s = token (Lexing.from_string s)

let opcom r = OPCOM(r.line_num,r.char_num)
let clcom r = CLCOM(r.line_num,r.char_num)
let sclcom r = SCLCOM(r.line_num,r.char_num)

  let get_all_tokens s =
      let _ = char_count := 1 in
      let _ = line_count := 1 in
      let b = Lexing.from_string (s^"\n") in
      let rec g () = 
      match token b with EOF -> []
      | t -> t :: g () in
      g ()

let try_get_all_tokens s =
    try Some (get_all_tokens s) with Failure "unmatched comment" -> None
    	     			      	 | OpenComm r -> None
    	     			      	 | CloseComm r -> None
    	     			      	 | SuperCloseComm r -> None
let try_comm_get_all_tokens s =
    try Some (get_all_tokens s) with Failure "unmatched comment" -> None
    	     			      	 | OpenComm r -> Some ([opcom r])
    	     			      	 | CloseComm r -> Some ([clcom r])
    	     			      	 | SuperCloseComm r -> Some ([sclcom r])

 }

