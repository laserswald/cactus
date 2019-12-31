
(define (assert thing msg) 
  (if (not thing)
    (begin (display msg) (newline))
    '()))

