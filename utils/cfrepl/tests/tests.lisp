(load "tests/framework")

;;; Treasures
(load "examples/treasure")

(deftest ("treasure list test")
  (assert-equal (mapcar #'ob-name (treasure-test "standard_old" :seed 32))
                '("wand" "small lightnings")))

;;; Items
(defun arch-name (arch)
  (ob-name (create-arch arch)))

(defun assert-streq (a b)
  (assert (string-equal a b)))

(deftest ("item name test")
  (assert-streq (arch-name "potion_heal") "healing potion"))

(run-tests)
