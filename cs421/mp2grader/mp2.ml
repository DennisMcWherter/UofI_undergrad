(* CS421 - Fall 2011
 * MP2 
 *
 * Please keep in mind that there may be more than one
 * way to solve a problem.  You will want to change how a number of these start.
 *)

open List

(* Private helpers *)
let rec split_first xs =
  match xs
  with h::t -> (match h
    with (x,y) -> [x] @ split_first t)
  | [] -> [];;

let rec split_second xs =
  match xs
  with h::t -> (match h
    with (x,y) -> [y] @ split_second t)
  | [] -> [];;

let rec tuplesYFastest x xs =
  match xs
  with h::t -> [(x, h)] @ tuplesYFastest x t
  | [] -> [];;

let rec freq_prob x fs =
  match fs
  with h::t -> [((float_of_int h) /. x)] @ freq_prob x t
  | [] -> [];;

let rec runDFA (qs,ss,d,q0,fs) i q =
  match i
  with h::t -> runDFA (qs,ss,d,q0,fs) t (d q h)
  | [] -> q;;

let rec inList fs q =
  match fs
  with h::t -> if h == q then true else inList t q
  | [] -> false;;

(* Problem 1 *)
let rat_eq ((a1,a2),(b1,b2)) =
  let a = a1 * b2 in
    let b = a2 * b1 in
      a == b;;

(* Problem 2 *)
let rec ackermann (m,n) = if m == 0 then n + 1 else
  if (m > 0 && n == 0) then ackermann (m-1, 1) else
    ackermann (m-1, (ackermann (m, n-1)));;

(* Problem 3 *)
let rec list_ex p xs = match xs
  with h::t -> if p h then true else list_ex p t
  | [] -> false;;

(* Problem 4 *)
let rec elem x xs =
  match xs
  with h::t -> if h == x then true else elem x t
  | [] -> false;;

(* Problem 5 *)
let rec to_float_list xs =
  match xs
  with h::t -> [float_of_int h] @ to_float_list t
  | [] -> [];;

(* Problem 6 *)
let rec sum fs =
  match fs
  with h::t -> h +. (sum t)
  | [] -> 0.0;;

(* Problem 7 *)
let rec zip xs ys =
  match xs
  with hx::tx -> (match ys
    with hy::ty -> [(hx, hy)] @ zip tx ty
    | [] -> [])
  | [] -> [];;


(* Problem 8 *)
let rec split xs =
  match xs
  with h::t -> (match h
    with (x,y) -> (([x] @ split_first t), ([y] @ split_second t)))
  | [] -> ([], []);;

(* Problem 9 *)
let rec cross xs ys = 
  match xs
  with h::t -> tuplesYFastest h ys @ cross t ys
  | [] -> [];;

(* Problem 10 *)
let rec freq_to_prob fs =
  let x = sum (to_float_list fs) in freq_prob x fs;;

(* Problem 11 *)
let rec expected xs =
  match xs
  with h::t -> (match h
    with (x, y) -> (x*. y) +. expected t)
  | [] -> 0.0;;

(* Problem 12 *)
let rec accept (qs,ss,d,q0,fs) i =
  let q = runDFA (qs,ss,d,q0,fs) i q0 in
    if inList fs q then true else false;;



(* Extra credit *)
(* Private helpers for DFA *)
let rec verifyTransition (qs,ss,d) q =
  match ss
  with h::t -> if not (inList qs (d q h)) then false else verifyTransition (qs,t,d) q
  | [] -> true;;

let rec verifyStates (qs,ss,d) s =
  match s
  with h::t -> if not (verifyTransition (qs,ss,d) h) then false else
    verifyStates (qs,ss,d) t
  | [] -> true;;

let rec isSubset fs qs =
  match fs
  with h::t -> if (inList qs h) then (isSubset qs t) else false
  | [] -> true;;

let rec valid_dfa (qs,ss,d,q0,fs) =
  if (inList qs q0) && (verifyStates (qs,ss,d) qs) && (isSubset fs qs) then true else false;;
