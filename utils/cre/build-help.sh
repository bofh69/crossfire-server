#!/bin/sh

rm -f cre.qhc cre.qch help/html/*.html

for f in help/*.md ; do
  ON=`echo $f | sed -E 's|help/(.*)md|help/html/\1html|'`
  docker run --rm --volume "`pwd`:/data" --user `id -u`:`id -g` pandoc/core:2.16.2 -s -f markdown -t html --css help/html/style.css --self-contained -o $ON $f
done

( cd help/html && qcollectiongenerator cre.qhcp -o cre.qhc )
mv help/html/cre.qch help/html/cre.qhc .
