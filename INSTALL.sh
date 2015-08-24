
for d in rnnpg rnnpg-decoder first-sentence-generator rnnpg-generator
do
	cd $d
	make
	cd ..
done

