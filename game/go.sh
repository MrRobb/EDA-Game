make > /dev/null

PLAYER="MrRobb_v6"
ENEMY="MrRobb_v5"

echo "\n$PLAYER vs $ENEMY\n"

echo "200 rounds";

for (( i = 0; i < 100; i++ )); do
    RNUM=$(od -An -N4 -tu2 /dev/urandom | awk '{print $1}')
	echo "$i with seed $RNUM";
	./Game $PLAYER $ENEMY $ENEMY $ENEMY --seed $RNUM < default.cnf > default.out 2> cerr.out && grep -e "top score" cerr.out;
	./Game $ENEMY $PLAYER $ENEMY $ENEMY --seed $RNUM < default.cnf > default.out 2> cerr.out && grep -e "top score" cerr.out;
	./Game $ENEMY $ENEMY $PLAYER $ENEMY --seed $RNUM < default.cnf > default.out 2> cerr.out && grep -e "top score" cerr.out;
	./Game $ENEMY $ENEMY $ENEMY $PLAYER --seed $RNUM < default.cnf > default.out 2> cerr.out && grep -e "top score" cerr.out;
done

rm -f cerr.out
