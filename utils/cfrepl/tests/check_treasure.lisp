(load "tests/framework")
(load "examples/treasure")

(deftest ("treasure list test")
  (assert-equal (mapcar #'ob-name (treasure-test "standard_old" :seed 32))
                '("wand" "small lightnings")))

(deftest ("always fail")
  (assert-equal 1 2))

(run-tests)
