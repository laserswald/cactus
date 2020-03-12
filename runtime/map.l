(define (map l f)
	(if (null? l)
		'()
		(cons (f (car l))
			  (map (cdr l) f))))

