;;; Initialization file for Cactus.

(define (id x) x)

(define (atom? x) 
  (not (pair? x)))

(define (displayln x)
  (display x)
  (newline))

(define (map f l)
  (if (null? l)
    '()
    (cons (f (car l))
          (map f (cdr l)))))

(define (list-ref l i)
  (if (and (eqv? i 0)
           (pair? l))
    (car l)
    (list-ref (cdr l) (- i 1))))

(define (list-tail x k)
  (if (zero? k)
      x
      (list-tail (cdr x) (- k 1))))

