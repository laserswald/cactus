(load "rt/test.scm")

(define id
  (lambda (x) x))

(assert (= (id 1) 1) "identity function")

(show-report)
