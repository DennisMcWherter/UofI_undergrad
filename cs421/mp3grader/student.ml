(* CS 421 Fall 2011 MP3 *)

open Mp3common

(* Helpers *)
let rec partial_sums_aux lst =
  match lst
  with h::t -> h +. partial_sums_aux t
  | [] -> 0.;;

let rec not_same_next_aux lst l =
  match lst
  with h::t -> (match l
    with hl::tl -> if hl = h then not_same_next_aux t l
      else h :: not_same_next_aux t [h]
    | [] -> h :: not_same_next_aux t [h])
  | [] -> [];;

let rec exists_pred_aux p lst ret =
  match lst with
    [] -> ret
  | h::t -> let x = p h in exists_pred_aux p t x;;

let rec count_if_aux acc p lst =
  match lst
  with h::t -> if p h then count_if_aux (acc+1) p t else count_if_aux acc p t
  | [] -> acc;;

let rec min_element_aux v i c lst =
  match lst
  with h::t -> if h >= v then min_element_aux v i (c+1) t
    else min_element_aux h c (c+1) t
  | [] -> i;;

let cross_prod_aux ys elem =
  List.map ((fun x -> fun y -> (x, y)) elem) ys;;

(* Problem 1 *)
let rec replace_if p v lst =
  match lst
  with h::t -> if p h then v :: replace_if p v t else h :: replace_if p v t
  | [] -> [];;

(* Problem 2 *)
let rec partial_sums lst =
  match lst
  with h::t -> h +. partial_sums_aux t :: partial_sums t
  | [] -> [];;

(* Problem 3 *)
let rec not_same_next lst = not_same_next_aux lst [];;

(* Problem 4 *)
let rec exists_pred p lst = exists_pred_aux p lst false

(* Problem 5 *)
let count_if p lst = count_if_aux 0 p lst;;

(* Problem 6 *)
let min_element lst =
  match lst
  with h::t -> min_element_aux h 0 1 t
  | [] -> -1;;

(* Problem 7 *)
let replace_if_base = [] (* This may need changing. *)
let replace_if_rec p v x r =
  let n = v::r in
    let m = x::r in
      if p x then n else m;;

(* Problem 8 *)
let not_same_next_base = [] (* This may need changing. *)
let not_same_next_rec x r =
  match r with
  h::t -> if h = x then r else let n = x::r in n
  | [] -> let n = x::r in n;;

(* Problem 9 *)
let exists_pred_base = false (* This may need changing. *)
let exists_pred_rec p r x =
  if p x then r || true else r || false;;

(* Problem 10 *)
let count_if_base = 0 (* This may need changing. *)
let count_if_rec p r x =
  if p x then r+1 else r;;

(* Problem 11 *)
let cross_prod xs ys =
  List.map (cross_prod_aux ys) xs;;

(* Problem 12 *)
let subk n m k = k (n - m);;
let consk h t k = k (h :: t);;
let addk x y k = k (x + y);;
let timesk a b k = k (a * b);;
let plusk x y k = k (x +. y);;
let multk x y k = k (x *. y);;
let lessk n m k = k (n < m);;
let eqk x y k = k (x = y);;

(* Problem 13 *)
let abcdk a b c d k = plusk a b (fun ab -> plusk c d (fun cd -> multk ab cd k));;

(* Problem 14 *)
let rec plus_listk lst k =
  match lst
  with [] -> k 0.
  | 0.::t -> plus_listk t k
  | h::t -> plus_listk t (fun r -> plusk h r k);;

let rec partial_sumsk lst k =
  match lst
  with h::t -> partial_sumsk t 
    (fun r -> plus_listk lst
      (fun x -> consk x r k)
    )
  | [] -> k [];;

(* Problem 15 *)
let rec count_ifk pk lst k =
  match lst
  with h::t ->
    let x = pk h (fun y -> if y then k 1 else k 0) in
    let z = k 1 in
      if x = z then
        count_ifk pk t (fun r -> addk r 1 k)
      else
        count_ifk pk t k
  | [] -> k 0;;

(* Problem 16 *)
let min_elementk lst k =
	raise(Failure "Function not implemented yet.")

