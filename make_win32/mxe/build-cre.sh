#!/bin/sh

(
  cd ../../utils/cre &&
  rm -f release/cre.exe &&
  x86_64-w64-mingw32.static-qmake-qt5 &&
  make &&
  ( cd help/html && /usr/lib/x86_64-linux-gnu/qt5/bin/qcollectiongenerator cre.qhcp -o cre.qhc ) && # we need to generate the help files in case the user doesn't have Qt tools installed
  mv help/html/cre.qch help/html/cre.qhc . &&
  lrelease cre.pro
)
(
  cd .. &&
  makensis cre.nsi
)