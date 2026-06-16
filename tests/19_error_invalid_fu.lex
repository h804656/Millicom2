// Error: method call on an UNREGISTERED FU. `Foo` was never declared via NewFU,
// so when the dot is seen the name before it cannot be resolved to a real FU and
// the Root `>{Sep="."}` unresolved-FU handler fires (Lex.Stop + ProgStopAll).
Foo.Bar="x"
