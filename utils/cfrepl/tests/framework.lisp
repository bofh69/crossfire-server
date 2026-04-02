(defvar *tests-left* nil)

; dynamic variables for RUN-TEST
(defvar *tests-passed*)
(defvar *tests-failed*)

(defun assert-equal (a b)
  (assert (equalp a b)))

(defun run-tests ()
  (let ((*tests-passed* nil)
        (*tests-failed* nil))
    (dolist (test *tests-left*)
      (funcall test))
    (format t "Passed ~a/~a tests~%" (length *tests-passed*) (+ (length *tests-failed*) (length *tests-passed*)))
    (format t "Failed tests:~{ ~a~}" *tests-failed*))
  (setf *tests-left* nil))

(defmacro deftest ((name) &body body)
  `(push (lambda ()
           (handler-case
               (progn ,@body
                      (push ,name *tests-passed*))
             (simple-type-error (c)
               (push ,name *tests-failed*)))) *tests-left*))
