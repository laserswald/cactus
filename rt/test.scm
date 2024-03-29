; vim: ft=scheme

(define *failed* 0)
(define *passed* 0)
(define *tests-loud* #f)

(define assert
  (lambda (thing msg)
    (if (not thing)
      (begin 
        (newline)
        (display "assertion failed: ")
        (display msg) 
        (newline)
        (set! *failed* (+ 1 *failed*)))
      (begin
       (if *tests-loud*
         (begin
           (display "PASS: ")
           (displayln msg))
         (display "."))
       (set! *passed* (+ 1 *passed*))))))

(define show-report
  (lambda ()
    (newline)
    (display "  passed: ")
    (display *passed*)
    (newline)
    (display "  failed: ")
    (display *failed*)
    (newline)))
  
(define clear-report
  (lambda ()
    (set! *failed* 0)
    (set! *passed* 0)))
  
