
(define (factorial n)
  (if (equal? n 1)
      1
      (* n (factorial (- n 1)))))

(collect-garbage)

(display (factorial 5))
(newline)
