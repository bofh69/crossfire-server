#!/bin/sh

rm -f html/search_data.js html/search_index.js
./crossfire-mapper -build-search-data "$@"

echo
echo "Mapper completed, indexing search data..."

DIR=`mktemp -d`

cp html/search_data.js $DIR

cat > $DIR/build-index.js <<'EOF'
#!/usr/local/bin/node

var lunr = require('lunr'),
    ProgressBar = require('progress');

var inputBuffer = '';

process.stdin.resume();
process.stdin.setEncoding('utf8');

process.stdin.on('data', function (chunk) {
  inputBuffer = inputBuffer.concat(chunk);
});

process.stdin.on('end', function () {
  try {
    eval(inputBuffer);
  } catch (e) {
    console.error('invalid input JS');
    process.exit(1);
  }

  var documents = search_data;
  var bar = new ProgressBar ('  indexing [:bar] :percent :etas', {
    total: Object.keys(documents).length,
    width: 20,
    complete: '=',
    incomplete: ' ',
    stream: process.stderr
  });

  var idx = lunr(function () {
    this.ref('id');
    this.field('name');
    this.field('text', { boost: 0.8 });
    this.field('type', { boost: 0 });
    var lnr = this;

    for (const [key, value] of Object.entries(documents)) {
        var item = value;
        item["id"] = key;
        lnr.add(item);
        bar.tick(1);
    }
  });

  process.stdout.write(JSON.stringify(idx));
});

EOF

cat > $DIR/s.sh <<'EOF'
#!/bin/sh

npm install lunr progress
node build-index.js -f name -r id < search_data.js > index_tmp.json
{ echo -n "var search_index = " ; cat index_tmp.json ;  echo ";" ; } > search_index.js

EOF

chmod +x $DIR/s.sh

docker run -it --rm --name my-running-script -v "$DIR":/usr/src/app -w /usr/src/app node:alpine ./s.sh

cp $DIR/search_index.js html/search_index.js

echo
echo "Indexing done, resulting files:"

ls -lh html/search*.js
