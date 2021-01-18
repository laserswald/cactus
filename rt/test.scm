; vim: ft=scheme

(define *failed* 0)

(define *passed* 0)

(define (assert thing msg) 
  (if (not thing)
    (begin 
      (newline)
      (display "assertion failed: ")
      (display msg) 
      (newline)
      (set! *failed* (+ 1 *failed*)))
    (begin
     (display ".")
     (set! *passed* (+ 1 *passed*)))))

(define (show-report)
  (newline)
  (display "passed: ")
  (display *passed*)
  (newline)
  (display "failed: ")
  (display *failed*)
  (newline))
  
(define (clear-report)
  (set! *failed* 0)
  (set! *passed* 0))
  