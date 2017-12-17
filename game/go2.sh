make > /dev/null

PLAYER="MrRobb_v7"
ENEMY="MrRobb_v5"

echo "\n$PLAYER vs $ENEMY\n"

echo "200 rounds";

for (( i = 0; i < 100; i++ )); do
	echo "$i with seed $i";
	./Game $PLAYER $ENEMY $ENEMY $ENEMY --seed $i < default.cnf > default2.out 2> cerr2.out && grep "top score" cerr2.out;
	./Game $ENEMY $PLAYER $ENEMY $ENEMY --seed $i < default.cnf > default2.out 2> cerr2.out && grep "top score" cerr2.out;
	./Game $ENEMY $ENEMY $PLAYER $ENEMY --seed $i < default.cnf > default2.out 2> cerr2.out && grep "top score" cerr2.out;
	./Game $ENEMY $ENEMY $ENEMY $PLAYER --seed $i < default.cnf > default2.out 2> cerr2.out && grep "top score" cerr2.out;
done

rm -f cerr2.out
