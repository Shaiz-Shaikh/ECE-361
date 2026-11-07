#!/bin/sh
# Runs ./replay on every scenario and compares the trace with the expected one.
fail=0
for s in scenarios/*.txt; do
    name=$(basename "$s")
    if ./replay "$s" | diff "expected/$name" - > /dev/null; then
        echo "PASS: $name"
    else
        echo "FAIL: $name"
        ./replay "$s" | diff "expected/$name" -
        fail=1
    fi
done
exit $fail
