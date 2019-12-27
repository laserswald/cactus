
(define (assert expr? msg) 
  (if (not expr?)
    (begin (display msg) (newline))
    '()))

