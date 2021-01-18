(load "rt/test.scm")

(assert (bound? 'and) "and is not bound")
; (assert (eq? (and) #t) "and with no arguments should be #t")
