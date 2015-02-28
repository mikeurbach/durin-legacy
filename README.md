Examples
--------

```
f = a[n], b -> a + b;
reverse = list[n] -> { list<i>{ rev[i] = list[n-i-1]; }; rev };
MM = a[m,n], b[m,n] -> { a<i, j>{ c[i,j] = a[i,j] * b[j,i]; }; c };
x = 2.1 ^ 8 - 4.1e-2 % 7 + g(123);
map = in, f -> { in<i>{ out = f(in[i]); } out };
foldl = in, init, f -> { out = init; in<i>{ out = f(out, in[i]); } out };
filter = in, f -> { in<i>{ out[i] = f(in[i]) ? in[i] : null; }; out };
2 + 2
```

Bison
-----

```
program            := statement-list | expression
statement-list     := statement-list statement | statement
statement          := enumeration | binding ';'
enumeration        := symbol '<' symbol-list '>' '{' statement-list '}'
binding            := formal-value '=' expression
expression         := application | lambda | math | ternary | value | literal
application        := symbol '(' actual-args ')'
actual-args        := expression ',' actual-args | expression
lambda             := formal-args '->' ('{' statement-list expression '}) | expression
formal-args        := formal-value ',' formal-args | formal-value
formal-value       := symbol | symbol '[' symbol-list ']
math               := expression binary-op expression | expression '.' binary-op expression
binary-op          := '+' | '-' | '*' | '/' | '^' | '%' | '.'
ternary            := boolean-expression ? expression : expression
boolean-expression := boolean-expression ('&&' | '||') boolean-expression | '!' boolean-expression | expression compare-op expression
compare-op         := '>' | '>=' | '<' | '<=' | '==' | '!='
value              := symbol | symbol '[' expression-list ']'
expression-list    := expression ',' expression-list | expression
```
Flex
----
literal            := 'null' | 'true' | 'false' | scalar-lit | matrix-lit
scalar-lit         := '-'? (integer | decimal | scientific)
integer            := nonzero* digit
decimal            := integer '.' digit+
scientific         := decimal 'e' '-'? integer
digit              := [0-9]
nonzero            := [1-9]
matrix-lit         := '[' literal-list ']'
literal-list       := literal ',' literal-list | literal
symbol-list        := symbol ',' symbol-list | symbol
symbol             := letter | letter ('_' | '-' | letter)
letter             := [a-zA-Z]
```
