(load "rt/test.scm")

(define factorial
  (lambda (n)
    (if (equal? n 1)
        1
        (* n (factorial (- n 1))))))

(assert (equal? (factorial 5) 120) "factorial 5")
(assert (equal? (factorial 10) 3628800) "factorial 10")

(show-report)
