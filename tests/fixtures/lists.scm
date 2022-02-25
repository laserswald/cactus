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
(assert (eqv? 'b (list-ref l 1)) "list-ref 1")
(assert (eqv? 'c (list-ref l 2)) "list-ref 2")
(assert (eqv? 'd (list-ref l 3)) "list-ref 3")
(define x 0)
(for-each (lambda (i)
            (set! x (+ i x)))
          '(1 2 3 4))
(assert (= x 10) "For-each and set")

(show-report)

