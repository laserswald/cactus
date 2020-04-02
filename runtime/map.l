(define (map l f)
	(if (null? l)
		'()
		(cons (f (car l))
			  (map (cdr l) f))))

(display (map (lambda (x) (+ x x)) '(1 2 3 4)))

