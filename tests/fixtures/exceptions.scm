(load "rt/test.scm")

(define *test-exception*
  '(definitely-an-exception))

(with-exception-handler 
  (lambda (exn)
    (assert (equal? exn *test-exception*) 
            "did not catch exception"))
  (lambda ()
    (raise *test-exception*)))
