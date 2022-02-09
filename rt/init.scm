;;; Initialization file for Cactus.

(define id
  (lambda (x) x))

(define atom?
  (lambda (x)
    (not (pair? x))))

(define displayln
  (lambda (x)
    (display x)
    (newline)))

(define map
  (lambda (f l)
    (if (null? l)
      '()
      (cons (f (car l))
            (map f (cdr l))))))

(define list-ref 
  (lambda (l i)
    (if (and (eqv? i 0)
             (pair? l))
      (car l)
      (list-ref (cdr l) (- i 1)))))

(define list-tail
  (lambda (x k)
    (if (zero? k)
        x
        (list-tail (cdr x) (- k 1)))))

