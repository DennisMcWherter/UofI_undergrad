open Mp4common

(* Helpers *)
let rec inList elem lst =
  match lst with
  | x::xs -> if x = elem then true else inList elem xs
  | [] -> false;;

let rec import_list lst =
  match lst with
    [] -> ConExp Nil
  | h::t -> BinExp(Cons, ConExp(Int h), import_list t);;

let elem =
  RecExp("elem", "e", FunExp("xs", IfExp(BinExp(Eq, VarExp "xs", ConExp Nil),
    ConExp(Bool(false)), IfExp(BinExp(Eq, MonExp(Head, VarExp "xs"), VarExp "e"),
      ConExp(Bool(true)), AppExp(AppExp(VarExp "elem", VarExp "e"),
        MonExp(Tail, VarExp "xs"))))), VarExp "elem");;

let rec num_of_consts expression =
  match expression with
  | ConExp(_) -> 1
  | VarExp(_) -> 0
  | OAppExp(x, y) -> num_of_consts x + num_of_consts y
  | RecExp(_, _, x, y) ->  num_of_consts x + num_of_consts y
  | LetExp(_, x, y) -> num_of_consts x + num_of_consts y
  | FunExp(_, x) -> num_of_consts x
  | MonExp(_, x) -> num_of_consts x
  | BinExp(_, x, y) -> num_of_consts x + num_of_consts y
  | AppExp(x, y) -> num_of_consts x + num_of_consts y
  | IfExp(x, y, z) -> num_of_consts x + num_of_consts y + num_of_consts z
  | OAppExp(x, y) -> num_of_consts x + num_of_consts y

let rec freeVars_aux exp lst =
  match exp with
  | VarExp(x) -> if inList x lst then [] else [x]
  | ConExp(_) -> []
  | IfExp(x, y, z) -> freeVars_aux x lst @ freeVars_aux y lst @ freeVars_aux z lst
  | FunExp(x, y) -> freeVars_aux y (x :: lst)
  | LetExp(x, y, z) -> let l = x :: lst in freeVars_aux y l @ freeVars_aux z l
  | RecExp(w, x, y, z) -> let l = w :: lst in freeVars_aux y (x :: l) @ freeVars_aux z l
  | MonExp(_, x) -> freeVars_aux x lst
  | BinExp(_, x, y) -> freeVars_aux x lst @ freeVars_aux y lst
  | AppExp(x, y) -> freeVars_aux x lst @ freeVars_aux y lst
  | OAppExp(x, y) -> freeVars_aux x lst @ freeVars_aux y lst;;

let rec freeVars expression = freeVars_aux expression []

let rec usedVars exp =
  match exp with
  | IfExp(x, y, z) -> usedVars x @ freeVars y @ freeVars z
  | FunExp(x, y) -> x :: (usedVars y)
  | LetExp(x, y, z) -> x :: (usedVars y @ usedVars z)
  | RecExp(w, x, y, z) -> x :: (w :: (usedVars y @ usedVars z))
  | MonExp(_, x) -> usedVars x
  | BinExp(_, x, y) -> usedVars x @ usedVars y
  | AppExp(x, y) -> usedVars x @ usedVars y
  | OAppExp(x, y) -> usedVars x @ usedVars y
  | ConExp(_) -> []
  | VarExp(_) -> [];;

let rec cps_aux exp cont lst =
  match exp with
  | VarExp(x) -> AppExp(cont, exp)
  
  | ConExp(x) -> AppExp(cont, exp)
  
  | IfExp(x, y, z) -> let lt = (lst @ usedVars y @ usedVars z @ usedVars cont) in
      let f = freshFor lt in let l = f :: lt in
      cps_aux x (FunExp(f, IfExp(VarExp f, (cps_aux y cont l),
        (cps_aux z cont l)))) l
  
  | AppExp(x, y) -> let lt = (usedVars y @ usedVars cont @ lst) in
      let f = freshFor lt in let l = f :: lt in
      let p = freshFor l in let t = p :: l in
      cps_aux x (FunExp(f,
        (cps_aux y (FunExp(p, AppExp(AppExp(VarExp f, VarExp p), cont))) t))) l
  
  | BinExp(x, y, z) -> let lt = (lst @ usedVars y @ usedVars z @ usedVars cont) in
      let f = freshFor lt in let l = f :: lt in
      let p = freshFor l in let t = p :: l in
      cps_aux y (FunExp(f, (cps_aux z (FunExp(p,
        AppExp(cont, BinExp(x, VarExp f, VarExp p)))) t))) l
  
  | MonExp(x, y) -> let lt = (usedVars cont @ lst) in
      let f = freshFor lt in let l = f :: lt in
      cps_aux y (FunExp(f, AppExp(cont, MonExp(x, VarExp f)))) l

  | FunExp(x, y) -> let lt = (x :: (usedVars y @ lst)) in
      let f = freshFor lt in let l = (f :: lt) in
      AppExp(cont, FunExp(x, FunExp(f, (cps_aux y (VarExp f) l))))
  
  | LetExp(x, y, z) -> let l = x :: lst in (cps_aux y (FunExp(x, (cps_aux z cont l))) lst)
  
  | RecExp(w, x, y, z) -> let lt = w :: (x :: (usedVars y @ lst)) in
      let f = freshFor lt in
      let l = f :: lt in
      RecExp(w, x, FunExp(f, (cps_aux y (VarExp f) l)), (cps_aux z cont l))
  
  | OAppExp(_) -> ConExp Nil

let rec cps expression cont = cps_aux expression cont [];;


