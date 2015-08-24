#!/bin/sh

codedir=PoemGeneration_via_SMT_utils
javac -sourcepath $codedir/src -d $codedir/bin $codedir/src/dio/*.java
javac -sourcepath $codedir/src -d $codedir/bin $codedir/src/smt_poem_generation/*.java

sxhyF=../MISC/Shixuehanying.txt.jian
outF=rand.keywords.300
outRndF=../MISC/rand.keywords.30
java -cp $codedir/bin smt_poem_generation.KeywordsGenerator $sxhyF 300 3 $outF true
cat $outF | head -30 | awk '{printf("%s 5 7\n", $0)}' > $outRndF
