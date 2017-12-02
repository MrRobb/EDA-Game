make

PLAYER="MrRobb_v3"
ENEMY="Dummy"

echo "Submission 14"

echo "1 with seed 761188" && ./Game $PLAYER $PLAYER $PLAYER $PLAYER --seed 761188 < default.cnf > default.out 2> cerr.out && grep "score" cerr.out && echo "\n"
echo "2 with seed 463092" && ./Game $PLAYER $ENEMY $ENEMY $ENEMY --seed 463092 < default.cnf > default.out 2> cerr.out && grep "score" cerr.out && echo "\n"
echo "3 with seed 209390" && ./Game $PLAYER $ENEMY $ENEMY $ENEMY --seed 209390 < default.cnf > default.out 2> cerr.out && grep "score" cerr.out && echo "\n"
echo "4 with seed 905326" && ./Game $PLAYER $ENEMY $ENEMY $ENEMY --seed 905326 < default.cnf > default.out 2> cerr.out && grep "score" cerr.out && echo "\n"
echo "5 with seed 709595" && ./Game $PLAYER $ENEMY $ENEMY $ENEMY --seed 709595 < default.cnf > default.out 2> cerr.out && grep "score" cerr.out && echo "\n"
echo "6 with seed 916538" && ./Game $PLAYER $ENEMY $ENEMY $ENEMY --seed 709595 < default.cnf > default.out 2> cerr.out && grep "score" cerr.out && echo "\n"
