;;;; Tests for numbers.

(load "rt/test.scm")

(assert (number? 0) "Zero is a number")
(assert (number? 1) "Zero is a number")

;; Plus
(assert (eqv? (+ 1 2) 3) "One plus two equals three")
(assert (eqv? (+) 0) "Plus nothing is zero")

;; Times
(assert (eqv? (*) 1) "Times nothing is one")

(show-report)
