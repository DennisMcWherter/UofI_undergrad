(*
 * File: mp6-skeleton.ml
 *)

(* Collaborated with: Jiageng Li (jli65) *)

open Mp6common

(* Problem 1*)
let asMonoTy1 () = TyConst ({name="->"; arity=2},
                  [
                    TyConst ({name="bool"; arity=0}, []);
                    TyConst ({name="list"; arity=1}, 
                    [
                      TyConst ({name="int"; arity=0}, [])
                    ])
                  ])
let asMonoTy2 () = TyConst ({name="->"; arity=2},
                  [
                    TyVar 4;
                    TyConst ({name="->"; arity=2},
                    [
                      TyVar 3;
                      TyConst ({name="->"; arity=2},
                      [
                        TyVar 2;
                        TyVar 1;
                      ])
                    ])
                  ])
let asMonoTy3 () = TyConst ({name="->"; arity=2},
                  [
                    TyVar 6;
                    TyConst ({name="list"; arity=1},
                    [
                      TyConst ({name="*"; arity=2},
                      [
                        TyVar 5;
                        TyConst ({name="int"; arity=0}, [])
                      ])
                    ])
                  ])
let asMonoTy4 () = TyConst ({name="*"; arity=2},
                  [
                    TyConst ({name="string"; arity=0}, []);
                    TyConst ({name="->"; arity=2},
                    [
                      TyConst ({name="list"; arity=1}, [TyVar 8]);
                      TyVar 7
                    ])
                  ])

(* Problem 2*)
let rec subst_fun subst m =
  match subst with
  | x::xs -> (
    match x with
    | (num, expr) -> if num = m then expr else (subst_fun xs m)
  )
  | [] -> TyVar m

(* Problem 3*)
let rec iterateList subst (ls : monoTy list) =
  match ls with
  | [] -> ([] : monoTy list)
  | x::xs -> (monoTy_lift_subst subst x) :: iterateList subst xs
and monoTy_lift_subst subst monoTy =
  match monoTy with
  | TyConst (ty, lst) -> (
      TyConst ({name = ty.name; arity = ty.arity}, (iterateList subst lst))
  )
  | TyVar x -> subst_fun subst x

(* Problem 4*)
let rec iterList target (lst : monoTy list) =
  match lst with
  | [] -> false
  | x::xs -> (occurs target x) || (iterList target xs)
and occurs x ty =
  match ty with
  | TyConst (ty, lst) -> iterList x lst
  | TyVar v -> if v = x then true else false

(* Problem 5*)
let rec decompose lst1 lst2 =
  match lst1, lst2 with
  | [],[] -> Some []
  | x::xs, y::ys -> ( (* Still have same number of args *)
      match decompose xs ys with
      | None -> None
      | Some lst -> Some (lst @ [(x,y)])
  )
  | _ -> None (* i.e. if |lst1| != |lst2| *)

let rec listLiftSubst inList lst =
  match lst with
  | [] -> []
  | (x,y)::xs -> (monoTy_lift_subst inList x, monoTy_lift_subst inList y) :: listLiftSubst inList xs

(* Order (for reference):
 * TyVar, TyVar - Try delete rule
 * TyConst, TyVar - Try orient
 * TyConst, TyConst - Try decompose
 * TyVar, TyConst - Try eliminate
 *)
let rec unify eqlst =
  match eqlst with
  | [] -> Some []
  | x::xs -> (
    match x with
    | (s, t) -> (
      match (s, t) with
      | (TyVar p, TyVar q) -> if p = q then unify xs else None
      | (TyConst (c, lst), TyVar (v)) -> unify ((TyVar v, TyConst (c, lst))::xs) 
      | (TyConst (c1, lst1), TyConst(c2, lst2)) -> 
        if not (c1 = c2) then None else (
          match decompose lst1 lst2 with
          | None -> None
          | Some lst -> unify (lst @ xs)
        )
      | (TyVar v, TyConst (c, lst)) -> (
        if occurs v (TyConst (c, lst)) then None else let res = (listLiftSubst [(v, TyConst (c, lst))] xs) in (
          match unify res with
          | None -> None
          | Some sub -> let theFun = monoTy_lift_subst sub in
                      Some ((v, theFun (TyConst (c, lst))) :: sub)
        )
      )
    )
    | _ -> None
  )

(* Extra Credit *)
let equiv_types x y = failwith "Not implemented"
