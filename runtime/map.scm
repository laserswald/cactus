(define (map f l)
	(if (null? l)
		'()
		(cons (f (car l))
			  (map f (cdr l)))))

(display (map (lambda (x) (+ x x)) '(1 2 3 4)))

