#!/bin/sh
# Runs ./balance on every file in inputs/ and compares its output and exit
# status with the file of the same name in expected/. Prints PASS or FAIL
# per file, and exits with 1 if any file fails.

fails=0
total=0

for f in inputs/*.txt; do
    name=$(basename "$f" .txt)
    ./balance < "$f" > actual.tmp
    echo "exit status: $?" >> actual.tmp
    total=$((total + 1))
    if diff "expected/$name.txt" actual.tmp > /dev/null; then
        echo "PASS $name"
    else
        echo "FAIL $name"
        diff "expected/$name.txt" actual.tmp
        fails=$((fails + 1))
    fi
done

rm -f actual.tmp
echo "$total inputs, $fails failed"
if [ "$fails" -ne 0 ]; then
    exit 1
fi
