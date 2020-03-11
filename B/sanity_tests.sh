for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22
do
    echo P3B-test_$i.csv
    ./lab3b test_data/P3B-test_$i.csv > test_result/test_result_$i.log
done
