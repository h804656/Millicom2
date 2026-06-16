// Error: `Cons` IS a registered FU, but `Nope` is not one of its methods. The dot
// resolves Cons and SubDowns into its method table; FindAnd("Nope") fails, so the
// MkWait `>{0}` else now raises the unknown-method error instead of silently
// recovering. `Cons.OutLn="ok"` (a real method) runs first to prove valid dispatch
// still works before the bad call stops the program.
Cons.OutLn="ok"
Cons.Nope="x"
