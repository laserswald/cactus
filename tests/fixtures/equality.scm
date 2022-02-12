(load "rt/test.scm")

; Eqv? tests

(assert (eqv? 'a 'a) "eqv? same symbols")
(assert (not (eqv? 'a 'b)) "eqv? different symbols")
(assert (eqv? 2 2) "eqv? same number")
(assert (eqv? '() '()) "eqv? null and null")
(assert (not (eqv? (cons 1 2) (cons 1 2))) "eqv? cons and cons")
(assert (not (eqv? (lambda () 1)
                   (lambda () 2)))
        "eqv? two lambdas")

; (eqv? 100000000 100000000)=⇒#t
; (assert (not (eqv? 2 2.0)) "eqv? exact and inexact")
; (eqv? 0.0 +nan.0)=⇒#f
; (let ((p (lambda (x) x)))(eqv? p p))=⇒#t
; (eqv? #f 'nil)=⇒#f

; Eq? tests.

; Booleans
(assert (eq? #t #t) "#t and #t")
(assert (eq? #f #f) "#f and #f")
(assert (not (eq? #t #f)) "#t and #f")

; Symbols
(assert (eq? 'a 'a) "same symbol eq?")
(assert (not (eq? 'a 'b)) "different symbol eq?")

; Null.
(assert (eq? '() '()) "null eq?")

(show-report)
