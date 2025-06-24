;;;; dump archetypes to a SQLite DB
(ql:quickload "sqlite")
(ql:quickload "str")

(defparameter +dbname+ "assets.db")
(defparameter +fields+
  (str:split " " "name title type monster level item_power weight food ac wc exp dam maxhp price"))

(defun dump-archs ()
  (let ((schema
          (concatenate 'string "create table arch (arch"
                       (format nil "~{,~a~}" +fields+)
                       ");"))
        (insert-query
          (concatenate 'string "insert into arch values (?"
                       (str:repeat (length +fields+) ",?")
                       ");")))
    (sqlite:with-open-database (db +dbname+)
      (sqlite:execute-non-query db "PRAGMA synchronous = 0;") ; speed up, this isn't important data
      (sqlite:execute-non-query db "drop table if exists arch;")
      (sqlite:execute-non-query db schema)
      (dolist (arch (all-archs))
        (let ((ob (arch-ob arch)))
          (sqlite:execute-non-query db insert-query (arch-name arch)
                                    (ob-name ob)
                                    (ob-field-title ob)
                                    (ob-field-type ob)
                                    (if (query-flag ob FLAG_MONSTER) 1 0)
                                    (ob-field-level ob)
                                    (ob-field-item_power ob)
                                    (ob-field-weight ob)
                                    (ob-field-stats.food ob)
                                    (ob-field-stats.ac ob)
                                    (ob-field-stats.wc ob)
                                    (ob-field-stats.exp ob)
                                    (ob-field-stats.dam ob)
                                    (ob-field-stats.maxhp ob)
                                    (price-base ob)
                                    ))))))

(eval-when (:execute) (dump-archs))
