(load "rt/test.scm")
(define x 0)
(for-each (lambda (i)
            (set! x (+ i x)))
          '(1 2 3 4))
(assert (= x 10) "For-each and set")
(show-report)
