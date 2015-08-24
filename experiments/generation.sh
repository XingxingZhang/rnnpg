#!/bin/sh

cd ..
cd rand-keywords
./run.sh
cd ..

cd first-sentence-generator/
./gen-first.sh
cd ..

cd rnnpg-generator
./gen-poems.sh
cd ..

echo "checkout the generated poems at ../MISC/rand.keywords.30.poems.std"
