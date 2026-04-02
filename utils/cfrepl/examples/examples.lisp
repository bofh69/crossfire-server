(load "examples/itemdump")
; dump all player-sold unpaid items in the Scorn Sale Shop
(itemdump "/scorn/shops/scorn.sale2" :with FLAG_UNPAID)

(load "examples/treasure")
; generate the chaos knight treasure list 10 times and show the loot
(dotimes (n 10) (print (mapcar #'ob-name (treasure-test "c_knight"))))
