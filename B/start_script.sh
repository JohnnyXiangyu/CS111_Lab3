#! /bin/bash
if [ $# -ne 1 ]; then
    echo Error should provide exactly one argument! >&2
    exit 1
fi
python3 init.py $1
