(load "rt/test.scm")

(assert (bound? 'cons) "cons exists")
(assert (bound? 'pair?) "pair? exists")
(assert (bound? 'car) "car exists")
(assert (bound? 'cdr) "cdr exists")

(assert (pair? (cons 'a 'b)) "cons returns a pair")
(assert (not (pair? '())) "the empty list is not a pair")

(assert (eqv? (car (cons 1 2)) 1) "car")
(assert (eqv? (cdr (cons 1 2)) 2) "cdr")

(define l '(a b c d))

(assert (bound? 'list-ref) "list-ref exists")
(assert (eqv? 'a (list-ref l 0)) "list-ref 0")
(display (list-ref l 1))
(newline)
(assert (eqv? 'b (list-ref l 1)) "list-ref 1")
         
