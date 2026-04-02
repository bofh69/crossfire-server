(defun itemdump (path &key with without)
  "Dump items on a map at PATH with flags WITH and without flags WITHOUT to CSV on standard output."
  (labels ((filter (ob) (and (if with (query-flag ob with) T)
                             (if without (not (query-flag ob without)) T))))
    (for-map-objects (ob (ready-map path))
      (when (filter ob)
        (format t "~a,\"~a\",~a,~a~%" (nrof ob) (ob-name ob) (calc-item-enhancement ob) (price-base ob))))))
