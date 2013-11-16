(* CS421 - Fall 2011
 * MP1 
 *
 * Please keep in mind that there may be more than one
 * way to solve a problem.  You will want to change how a number of these start.
 *)

open Mp1common

(* Problem 1 *)
let x = 32.7;;  (* You will want to change this. *)

(* Problem 2 *)
let salute = "Greetings, my friend!";;

(* Problem 3 *)
let times_13 n = n * 13;;

(* Problem 4 *)
let square_plus_x y = let z = (y *. y) in z +. x;;

(* Problem 5 *)
let hail name = match name
  with "Elsa" -> print_string "Wayell, hah theya, Ayelsa!"
  | _ -> print_string ("Dear, "^name^". I wish you the best in CS421.\n");;

(* Problem 6 *)
let has_smallest_abs m n = let x = (if m < 0 then m * -1 else m) in
  let y = (if n < 0 then n * -1 else n) in
    if (x < y) then m else n;;

(* Problem 7 *)
let swap_pair (x,y) = (y,x);;

(*Problem 8 *)
let pair_app (f,g) x = (f(x), g(x));;

