(ffi:clines "#include \"global.h\"")

(defun init ()
  (format t "Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
Copyright (c) 1992 Frank Tore Johansen

Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
welcome to redistribute it under certain conditions. For details, please
see COPYING and LICENSE.
")
  (ffi:c-inline () () :void "init_library()" :one-liner t))

(defun srand (seed)
  (ffi:c-inline (seed) (:unsigned-long) :void "cf_srandom(#0)" :one-liner t))

(defun handle-nullptr (x)
  "If X is a NULL pointer, return NIL. Otherwise, return X."
  (if (and (typep x 'si:foreign-data) (ffi:null-pointer-p x)) nil x))

;; Objects
(defun nrof (ob)
  (ffi:c-inline (ob) (:pointer-void) :int "NROF((object *)#0)" :one-liner t))

(defmacro ob-field (field ret)
  `(defun ,(read-from-string (concatenate 'string "ob-field-" field)) (ob)
     (handle-nullptr (ffi:c-inline (ob) (:pointer-void) ,ret ,(concatenate 'string "((object *)#0)->" field) :one-liner t))))

(ob-field "above" :pointer-void)
(ob-field "inv" :pointer-void)
(ob-field "name" :cstring)
(ob-field "slaying" :cstring)
(ob-field "type" :int)
(ob-field "title" :cstring)
(ob-field "level" :int)
(ob-field "item_power" :int)
(ob-field "weight" :int)
(ob-field "stats.food" :int)
(ob-field "stats.ac" :int)
(ob-field "stats.wc" :int)
(ob-field "stats.exp" :int)
(ob-field "stats.dam" :int)
(ob-field "stats.maxhp" :int)

(defun ob-above (ob)
  (ob-iter #'ob-field-above ob))

(defun ob-inv (ob)
  (ob-iter #'ob-field-inv (ob-field-inv ob)))

(defun ob-iter (by ob &optional vals)
  (if (or (null ob) (ffi:null-pointer-p ob)) (reverse vals)
    (let ((next (funcall by ob)))
      (ob-iter by next (cons ob vals)))))

(defun ob-name (x)
  (ffi:c-inline (x) (:pointer-void) :cstring "{
    char buf[MAX_BUF];
    query_name((const object *)#0, buf, MAX_BUF);
    @(return) = buf;
  }"))

(defconstant FLAG_ALIVE 0)
(defconstant FLAG_UNPAID 6)
(defconstant FLAG_NO_PICK 8)
(defconstant FLAG_MONSTER 14)
(defconstant FLAG_FRIENDLY 15)
(defconstant FLAG_IS_FLOOR 63)

(defun query-flag (ob flag)
  (if (zerop (ffi:c-inline (ob flag) (:pointer-void :int) :int "query_flag((const object *)#0, #1)" :one-liner t))
      nil T))

(defun price-base (ob)
  (ffi:c-inline (ob) (:pointer-void) :uint64-t "price_base((object *)#0)" :one-liner t))

(defun calc-item-enhancement (ob)
  (ffi:c-inline (ob) (:pointer-void) :int "calc_item_enhancement((object *)#0)" :one-liner t))

;; Archs
(ffi:clines "#include \"AssetsManager.h\"")
(ffi:clines "#include \"assets.h\"")

(defun first-arch ()
  (handle-nullptr (ffi:c-inline () () :pointer-void "getManager()->archetypes()->next(nullptr)" :one-liner t)))

(defun next-arch (a)
  (handle-nullptr (ffi:c-inline (a) (:pointer-void) :pointer-void "getManager()->archetypes()->next((archetype *)#0)" :one-liner t)))

(defun all-archs ()
  (loop for arch = (first-arch) then (next-arch arch) while arch collecting arch))

(defun arch-name (x)
  (ffi:c-inline (x) (:pointer-void) :cstring "((archetype *)#0)->name" :one-liner t))

(defun arch-ob (x)
  (ffi:c-inline (x) (:pointer-void) :pointer-void "&((archetype *)#0)->clone" :one-liner t))

(defun create-arch (name)
  (handle-nullptr (ffi:c-inline (name) (:cstring) :pointer-void "create_archetype(#0)" :one-liner t)))

;; Treasures
(defun create-treasure (name ob &key (flag 0) (difficulty 0))
  (ffi:c-inline (name ob flag difficulty) (:cstring :pointer-void :int :int) :void "{
  treasurelist *list = find_treasurelist(#0);
  create_treasure(list, (object *)#1, #2, #3, 0);
  }"))

;; Maps
(defconstant MAP_UNIQUE 2)

(defun ready-map (path &key (flags 0))
  (handle-nullptr (ffi:c-inline (path flags) (:cstring :int) :pointer-void "ready_map_name(#0, #1)" :one-liner t)))

(defun map-size (m)
  (ffi:c-inline (m) (:pointer-void) (values :int :int) "{
  mapstruct *m = (mapstruct *)#0;
  @(return 0) = m->width;
  @(return 1) = m->height;
  }"))

(defun get-map-ob (m x y)
  (ffi:c-inline (m x y) (:pointer-void :int :int) :pointer-void "GET_MAP_OB((const mapstruct *)#0, #1, #2)" :one-liner t))

(defun objects-at (m x y)
  (ob-above (get-map-ob m x y)))

(defmacro for-map ((x y m) &body body)
  `(multiple-value-bind (w h) (map-size ,m)
     (loop for ,y from 0 below h do
       (loop for ,x from 0 below w do
         ,@body))))

(defmacro for-map-objects ((ob m) &body body)
  `(for-map (x y ,m)
     (dolist (,ob (objects-at ,m x y))
       ,@body)))
