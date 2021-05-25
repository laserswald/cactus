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
  (if (null? l)
    (error "list-ref: list not long enough")
    (if (eqv? i 0)
        (pair? l)
      (car l)
      (list-ref (cdr l) (- i 1)))))

(define (acons key val alist)
  (cons (cons key val) alist))

(define (assoc key alist)
  (if (null? alist)
    #f
    (if (equal? key (car (car alist)))
      (car alist)
      (assoc key (cdr alist)))))

(define (list-tail x k)
  (if (zero? k)
      x
      (list-tail (cdr x) (- k 1))))

