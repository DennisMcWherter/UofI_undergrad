open Mp9common;;

let const_to_val c =
  match c with
  | Bool(x) -> Boolval(x)
  | Int(x) -> Intval(x)
  | Float(x) -> Floatval(x)
  | String(x) -> Stringval(x)
  | Nil -> Listval []
  | Unit -> Unitval

let rec findTail lst =
  match lst with
  | x::xs -> if xs == [] then x else findTail xs

let monApply unop v = 
  match (unop, v) with
  | Head,Listval([]) -> raise (Failure "Empty list")
  | Tail,Listval([]) -> raise (Failure "Empty list")
  | Head,Listval(x::xs) -> x
  | Tail,Listval(x) -> findTail x
  | Fst,Pairval(x,_) -> x
  | Snd,Pairval(_,x) -> x
  | Neg,Intval(x) -> Intval(x * (-1))
  | Print,Intval(x) -> (print_int x); Unitval

let binApply binop (v1,v2) = 
  match binop,(v1,v2) with
  | Add,(Intval(x),Intval(y)) -> Intval (x + y)
  | Sub,(Intval(x),Intval(y)) -> Intval (x - y)
  | Mul,(Intval(x),Intval(y)) -> Intval (x * y)
  | Div,(Intval(x),Intval(0)) -> raise (Failure "Divide by 0")
  | Div,(Intval(x),Intval(y)) -> Intval (x / y)
  | FAdd,(Floatval(x),Floatval(y)) -> Floatval (x +. y)
  | FSub,(Floatval(x),Floatval(y)) -> Floatval (x -. y)
  | FMul,(Floatval(x),Floatval(y)) -> Floatval (x *. y)
  | FDiv,(Floatval(x),Floatval(0.)) -> raise (Failure "Divide by 0")
  | Exp,(Floatval(x),Floatval(y)) -> Floatval (x ** y)
  | Concat,(Stringval(x),Stringval(y)) -> Stringval (x ^ y)
  | Cons,(x,Listval(y)) -> Listval(x :: y)
  | Comma,(x,y) -> Pairval(x, y)
  | Eq,(x,y) -> Boolval(x = y)
  | Less,(x,y) -> Boolval(x < y)

let rec eval_exp (exp, m) = 
  match exp with
  | ConExp(x) -> const_to_val x
  | VarExp(x) -> (let v = lookup_mem m x in match v with
                  | Recvar(y, e, m') -> Closure (y, e, ins_mem m' x v) 
                  | _ -> v)
  | MonExp(x, y) -> monApply x (eval_exp (y, m))
  | BinExp(x, y, z) -> binApply x (eval_exp (y, m), eval_exp (z, m))
  | LetExp(x, y, z) -> let v1 = eval_exp (y, m) in
                      eval_exp (z, ins_mem m x v1)
  | FunExp(x, y) -> Closure (x, y, m)
  | AppExp(e1, e2) -> (let v' = eval_exp (e2, m) in
                      match eval_exp (e1, m) with
                      | Closure(x, e', m') -> eval_exp (e', ins_mem m' x v'))
  | IfExp(x, y, z) -> (match eval_exp (x, m) with
                      | Boolval(x) -> if x then eval_exp (y, m) else eval_exp (z, m))
  | RecExp(f, x, e1, e2) -> eval_exp (e2, ins_mem m f (Recvar(x, e1, m)))

let eval_dec (dec, m) =
  match dec with
  | Anon(x) -> ((None, eval_exp (x, m)), m)
  | TopLet(x, y) -> let v = eval_exp (y, m) in
                    ((Some x, v), (ins_mem m x v))
  | TopRec(f, x, e) -> let r = Recvar(x, e, m) in
                    ((Some f, r), (ins_mem m f r))


