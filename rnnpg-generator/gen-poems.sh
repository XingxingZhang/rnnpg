#!/bin/sh

keywordsF=../MISC/rand.keywords.30
outputF=$keywordsF.poems.out
outstdF=$keywordsF.poems.std

./rnnpg-generator poem_generator.conf $keywordsF.firstSent.txt $outputF

# convert the verbose format to human readable format
java -jar rnn_poem_post_precessor.jar $outputF $outstdF


