make > /dev/null

PLAYER="MrRobb_v5"
ENEMY="mcasellas"

echo "\n$PLAYER vs $ENEMY\n"

echo "200 rounds";

for (( i = 0; i < 100; i++ )); do
    echo "$i with seed $i" && ./Game $PLAYER $ENEMY $ENEMY $ENEMY --seed $i < default.cnf > default.out 2> cerr.out && grep "$ENEMY got top score" cerr.out;
done

rm -f cerr.out
