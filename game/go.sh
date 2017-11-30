make

echo "200 rounds";

for (( i = 0; i < 100; i++ )); do
    echo "$i " | tr -d '\n' && ./Game MrRobb_v2 Dummy Dummy Dummy --seed $i < default.cnf > default.out 2> cerr.out && grep "Dummy got top score" cerr.out;
done

echo "400 rounds";

for (( i = 0; i < 100; i++ )); do
    echo "$i " | tr -d '\n' && ./Game MrRobb_v2 Dummy Dummy Dummy --seed $i < default400.cnf > default.out 2> cerr.out && grep "Dummy got top score" cerr.out;
done

echo "1000 rounds";

for (( i = 0; i < 100; i++ )); do
    echo "$i " | tr -d '\n' && ./Game MrRobb_v2 Dummy Dummy Dummy --seed $i < default1000.cnf > default.out 2> cerr.out && grep "Dummy got top score" cerr.out;
done

rm -f cerr.out
