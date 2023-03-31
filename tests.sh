#!/bin/sh
if [ $# -eq 0 ]
  then
total=0
good=0
touch "bad.o"
echo "" > "bad.o"
for test in tests/testFile/*; do
    if [ -f "$test" ]; then
        # ./bin/http_parse $test mot > /dev/null 2>&1
        ./bin/http_parse $test mot > /tmp/rs 2>&1
        if [ "$?" -eq 1 ]; then
            good=$(($good+1))
        else
            echo "./bin/http_parse $test mot"
            echo "$test    Not Valid"
            tail -n 5 /tmp/rs | tr '\n' ' '| awk -F'>[0-9]+' ' { print $NF } ' | awk -F'rep:0' ' {print $1} '
            echo "$test" >> "bad.o"
        fi
        total=$(($total+1))
    fi
done

echo "Results: $good/$total tests."
else
    ./bin/http_parse "tests/testFile/test$1.txt" mot
fi
