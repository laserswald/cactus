
(load "tests/fixtures/test.l")

(assert (not (eq? 'a 'b)) "a is not b")

(show-failed)
