open Mp5common

let rec gather_ty_substitution judgement =
	let {gamma = gamma; exp = exp; monoTy = tau} = judgement in
	match exp with
	| ConExp c -> (
		let tau' = const_signature c in
		match unify [(tau, freshInstance tau')] with
		| Some sigma ->
			Some ({antecedents = []; conclusion = judgement}, sigma)
		| _ -> None
	)
(* You add clauses here *)
  | VarExp v -> (
    let var = lookup_env gamma v in
    match var with
    | None -> None
    | Some tau' -> (
      match unify [(tau, freshInstance tau')] with
      | Some sigma -> Some ({antecedents = []; conclusion = judgement}, sigma)
      | _ -> None
    )
  )
  | MonExp (mp, e) -> (
    let tau' = monop_signature mp in
    let tau1 = fresh () in
    let res  = gather_ty_substitution {gamma = gamma; exp = e; monoTy = tau1} in (
      match res with
      | None -> None
      | Some (proof, sigma) -> (
        let n = unify [((monoTy_lift_subst sigma) (mk_fun_ty tau1 tau), freshInstance tau')] in (
          match n with
          | None -> None
          | Some x -> Some ({antecedents = [proof]; conclusion = judgement}, subst_compose x sigma)
        )
      )
    )
  )
  | BinExp (bp, e1, e2) -> (
    let tau' = binop_signature bp in
    let tau1 = fresh() in
    let tau2 = fresh() in
    let subst1 = gather_ty_substitution {gamma = gamma; exp = e1; monoTy = tau1} in
      match subst1 with
      | None -> None
      | Some (proof1, sigma1) -> (
        let ngamma = env_lift_subst sigma1 gamma in
        let subst2 = gather_ty_substitution {gamma = ngamma; exp = e2; monoTy = tau2} in (
          match subst2 with
          | None -> None
          | Some (proof2, sigma2) ->
            let compsig  = subst_compose sigma2 sigma1 in
            let relation = mk_fun_ty tau1 (mk_fun_ty tau2 tau) in
            let uni = unify [(monoTy_lift_subst compsig relation, freshInstance tau')] in (
              match uni with
              | None -> None
              | Some sigmaf -> Some ({antecedents = [proof1; proof2]; conclusion = judgement},
                subst_compose sigmaf (subst_compose sigma2 sigma1))
        
          )
        )
      )
    )
    | IfExp (e1, e2, e3) -> (
      let tau1 = bool_ty in
      let subst1 = gather_ty_substitution {gamma = gamma; exp = e1; monoTy = tau1} in (
        match subst1 with
        | None -> None
        | Some (proof1, sigma1) -> (
          let ngamma = env_lift_subst sigma1 gamma in
          let nmt = monoTy_lift_subst sigma1 tau in
          let subst2 = gather_ty_substitution {gamma = ngamma; exp = e2; monoTy = nmt} in (
            match subst2 with
            | None -> None
            | Some (proof2, sigma2) -> (
              let s1comps2 = subst_compose sigma2 sigma1 in
              let ngamma2 = env_lift_subst s1comps2 gamma in
              let nmt2 = monoTy_lift_subst s1comps2 tau in
              let subst3 = gather_ty_substitution {gamma = ngamma2; exp = e3; monoTy = nmt2} in (
                match subst3 with
                | None -> None
                | Some (proof3, sigma3) -> Some ({antecedents = [proof1; proof2; proof3]; conclusion = judgement},
                  subst_compose sigma3 (subst_compose sigma2 sigma1))
              )
            )
          )
        )
      )
    )
    | FunExp(str, e) -> (
      let tau1 = fresh () in
      let tau2 = fresh () in
      let gam  = ins_env gamma str ([], tau1) in
      let subst = gather_ty_substitution {gamma = gam; exp = e; monoTy = tau2} in (
        match subst with
        | None -> None
        | Some (proof1, sigma1) -> (
          let sigt = monoTy_lift_subst sigma1 tau in
          let sigt1t2 = monoTy_lift_subst sigma1 (mk_fun_ty tau1 tau2) in
          let uni = unify [(sigt, sigt1t2)] in
          match uni with
          | None -> None
          | Some sigma2 ->
              Some ({antecedents = [proof1]; conclusion = judgement},
                subst_compose sigma2 sigma1)
        )
      )
    )
    | AppExp(e1, e2) -> (
      let tau1 = fresh () in
      let e1ty = mk_fun_ty tau1 tau in
      let subst1 = gather_ty_substitution {gamma = gamma; exp = e1; monoTy = e1ty} in (
        match subst1 with
        | None -> None
        | Some (proof1, sigma1) -> (
          let ngamma = env_lift_subst sigma1 gamma in
          let s2t1 = monoTy_lift_subst sigma1 tau1 in
          let subst2 = gather_ty_substitution {gamma = ngamma; exp = e2; monoTy = s2t1} in (
            match subst2 with
            | None -> None
            | Some (proof2, sigma2) -> Some ({antecedents = [proof1; proof2]; conclusion = judgement},
              subst_compose sigma2 sigma1)
          )
        )
      )
    )
    | LetExp(s, e1, e2) -> (
      let tau1 = fresh () in
      let subst1 = gather_ty_substitution {gamma = gamma; exp = e1; monoTy = tau1} in (
        match subst1 with
        | None -> None
        | Some (proof1, sigma1) -> (
          let oenv = env_lift_subst sigma1 gamma in
          let omon = monoTy_lift_subst sigma1 tau1 in
          let genv = gen oenv omon in
          let ngamma = ins_env oenv s genv in
          let ot = monoTy_lift_subst sigma1 tau in
          let subst2 = gather_ty_substitution {gamma = ngamma; exp = e2; monoTy = ot} in (
            match subst2 with
            | None -> None
            | Some (proof2, sigma2) ->
                Some ({antecedents = [proof1; proof2]; conclusion = judgement},
                  subst_compose sigma2 sigma1)
          )
        )
      )
    )
    | RecExp(f, x, e1, e2) -> (
      let tau1 = fresh () in
      let tau2 = fresh () in
      let ngamma = ins_env (ins_env gamma x ([], tau1)) f ([], (mk_fun_ty tau1 tau2)) in
      let subst1 = gather_ty_substitution {gamma = ngamma; exp = e1; monoTy = tau2 } in (
        match subst1 with
        | None -> None
        | Some (proof1, sigma1) -> (
          let s1gamma = env_lift_subst sigma1 gamma in
          let monoty = monoTy_lift_subst sigma1 (mk_fun_ty tau1 tau2) in
          let genv = gen s1gamma monoty in
          let ngamma2 = ins_env s1gamma f genv in
          let mf = monoTy_lift_subst sigma1 tau in
          let subst2 = gather_ty_substitution {gamma = ngamma2; exp = e2; monoTy = mf} in 
          match subst2 with
          | None -> None
          | Some (proof2, sigma2) ->
            Some ({antecedents = [proof1; proof2]; conclusion = judgement},
              subst_compose sigma2 sigma1)
        )
      )
    )
    | RaiseExp(e) -> (
      let ety = int_ty in
      let subst1 = gather_ty_substitution {gamma = gamma; exp = e; monoTy = ety} in (
        match subst1 with
        | None -> None
        | Some (proof, sigma) ->
          Some ({antecedents = [proof]; conclusion = judgement}, sigma)
      )
    )


