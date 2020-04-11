; vim: ft=scheme

(define *failed* 0)

(define (assert thing msg) 
  (if (not thing)
    (begin 
        (display "assertion failed: ")
        (display msg) 
        (newline)
        (set! *failed* (+ 1 *failed*)))
    #t))

(define (show-failed)
	(display "Failed: ")
	(display *failed*)
    (newline))

