#lang racket

(require "./passes/main.rkt")

; [y] = f(x::10:3, a::1){ y = x + a; }
(define ast
  '(program 
    (bindfun 
     (identifier f)
     (params 
      (identifier x (integer 10) (integer 3))
      (identifier a (integer 1))
      )
     (return 
      (identifier y (integer 1))
      )
     (statement 
      (bindvar 
       (identifier y)
       (add 
	(identifier x)
	(identifier a)
	)
       )
      )
     )
    )
  )

(run-passes ast)
