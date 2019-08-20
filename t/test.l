(display 'displays)
(newline)

(define displayln
	(lambda (x)
		(display x)
		(newline)))

(displayln 'displays-line)

(define assert 
	(lambda (x)
		(if x
			(progn (display x) (newline))
			(progn 
				(display 'failed) 
				(newline)))))

(assert (eq? 'a
			 (car (cons 'a 'b))))
