#!/bin/sh

keywordsF=../MISC/rand.keywords.30
./tostdkeywords.py $keywordsF $keywordsF.tmp

./first-sent $keywordsF.tmp 300 300 $keywordsF.firstSent.txt ../MISC/poem.all.model.hidden200.class100.txt ../MISC/kenlm.poem.20140306.bin ../MISC/Shixuehanying.txt.cpp ../MISC/psy_table_cpp

