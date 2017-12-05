make > /dev/null

PLAYER="MrRobb_v3"
ENEMY="mcasellas"

echo "\n$PLAYER vs $ENEMY\n"

echo "200 rounds";

for (( i = 0; i < 100; i++ )); do
    echo "$i with seed $i" && ./Game $PLAYER $ENEMY MrRobb_v2 Dummy --seed $i < default.cnf > default.out 2> cerr.out && grep "$PLAYER got top score" cerr.out;
done

rm -f cerr.out
