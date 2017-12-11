make > /dev/null

PLAYER="MrRobb_v4"
ENEMY="mcasellas"

echo "\n$PLAYER vs $ENEMY\n"

echo "200 rounds";

for (( i = 0; i < 100; i++ )); do
    RNUM=$(od -An -N4 -tu2 /dev/urandom | awk '{print $1}')
    echo "$i with seed $RNUM" && ./Game $PLAYER mcasellas Null Null --seed $RNUM < default.cnf > default.out 2> cerr.out && grep -e  "mcasellas" -e "MrRobb_v4" cerr.out | grep -e "score";
done

echo "400 rounds";

for (( i = 0; i < 100; i++ )); do
    RNUM=$(od -An -N4 -tu2 /dev/urandom | awk '{print $1}')
    echo "$i with seed $RNUM" && ./Game $PLAYER mcasellas Null Null --seed $RNUM < default400.cnf > default.out 2> cerr.out && grep "got top score" cerr.out;
done

echo "1000 rounds";

for (( i = 0; i < 100; i++ )); do
    RNUM=$(od -An -N4 -tu2 /dev/urandom | awk '{print $1}')
    echo "$i with seed $RNUM" && ./Game $PLAYER mcasellas Null Null --seed $RNUM < default1000.cnf > default.out 2> cerr.out && grep "got top score" cerr.out;
done

rm -f cerr.out
