
(define (factorial n)
  (if (equal? n 1)
      1
      (* n (factorial (- n 1)))))

(display (factorial 5)) (newline)
(display (factorial 10)) (newline)
