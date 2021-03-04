(load "rt/test.scm")

(define *test-exception*
  '(definitely-an-exception))

(with-exception-handler 
  (lambda (exn)
    (display "Handler being called!")
    (newline)
    (assert (equal? exn #f)
            "did not catch exception"))
  (lambda ()
    (raise *test-exception*)))

(display "After the handler test") (newline)

(show-report)

