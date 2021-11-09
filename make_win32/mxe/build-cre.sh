#!/bin/sh

(
  cd ../../utils/cre &&
  rm -f release/cre.exe &&
  x86_64-w64-mingw32.static-qmake-qt5 &&
  make
)
(
  cd .. &&
  makensis cre.nsi
)