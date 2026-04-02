(defun treasure-test (treasure &key (difficulty 1) (seed nil))
  (when seed
    (srand seed))
  (let ((container (create-arch "empty_archetype")))
    (create-treasure treasure container :difficulty difficulty)
    (ob-inv container)))
