#!/bin/sh

total=0
success=0
for test in tests/better/*; do
    if [ -f "$test" ]; then
        echo "./bin/http_parse $test mot"
        ./bin/http_parse $test mot > /dev/null 2>&1
        if [ "$?" -eq 1 ]; then
            success=$(($success+1))
        else
            echo "$test    Not Valid"
        fi
        total=$(($total+1))
    fi
done

echo "Succeded $success/$total tests."
