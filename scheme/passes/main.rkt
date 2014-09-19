#lang racket

;
; global data
;
(define types (make-hash))

;
; pass definitions
;

; take the top level module out of a program
(define (strip-program ast)
  (match ast [`(program ,top-level) top-level]))

; decide function to call for a statement
(define (handle-statement ast)
  (let ([stmt (car ast)])
    (case stmt
      ['bindfun (run-passes-helper ast bindfun-passes)]
      [else (string-append "bad statement: " (symbol->string stmt))])))

; initializes a module for a function definition
(define (initialize-module ast)
  ast)

;
; running the passes
;

; define the chains for the different passes
(define main-passes
  (list
   strip-program
   handle-statement))

(define bindfun-passes
  (list
   initialize-module
   ))

; receives an ast and runs the passes
(provide run-passes)
(define (run-passes ast)
  (run-passes-helper ast main-passes))

; recursively pipes the ast through passes
(define (run-passes-helper ast passes)
  (let* ([pass (car passes)]
         [rest (cdr passes)]
         [new-ast (pass ast)])
    (if (null? rest)
        new-ast
        (run-passes-helper new-ast rest))))
