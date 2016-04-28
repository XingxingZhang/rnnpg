# Chinese Poetry Generation with Recurrent Neural Networks
This project includes the code/model for the paper 

[Chinese Poetry Generation with Recurrent Neural Networks](http://aclweb.org/anthology/D/D14/D14-1074.pdf)


```
@InProceedings{zhang-lapata:2014:EMNLP2014,
  author    = {Zhang, Xingxing  and  Lapata, Mirella},
  title     = {Chinese Poetry Generation with Recurrent Neural Networks},
  booktitle = {Proceedings of the 2014 Conference on Empirical Methods in Natural Language Processing (EMNLP)},
  month     = {October},
  year      = {2014},
  address   = {Doha, Qatar},
  publisher = {Association for Computational Linguistics},
  pages     = {670--680},
  url       = {http://www.aclweb.org/anthology/D14-1074}
}
```

# Acknowledgement
Our implementation is greatly inspired by Tomas Mikolov's [rnnlm toolkit](http://rnnlm.org/).
We would like to thank Tomas Mikolov for making his code public available.

# Dataset
Download the complete dataset from [here](http://homepages.inf.ed.ac.uk/mlap/Data/EMNLP14/)

# Dependencies
* [KenLM](https://kheafield.com/code/kenlm/)
* G++ (4.4.7)
* Java (1.8.0_51, 1.6 or 1.7 should also be fine)
* Python (2.7)

# Installation
1) Install [KenLM](https://kheafield.com/code/kenlm/). Also remember to add kenlm to your LD_LIBRARY_PATH
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/afs/inf.ed.ac.uk/user/s12/s1270921/usr/kenlm/lib
```
2) Go to *rnnpg* folder and modify the Makefile (see below). Direct `INCLUDES` and `LDFLAGS` to your KenLM library. Also modify the Makefiles in *rnnpg-decoder* and *rnnpg-generator*.

3) Make everything by ./INSTALL.sh
```
OUT_EXEC = rnnpg 
OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

CC = g++
CPPFLAGS = -O3 -funroll-loops -ffast-math -finline-functions -Wall -Winline -pipe -DKENLM_MAX_ORDER=6

INCLUDES = -I/afs/inf.ed.ac.uk/user/s12/s1270921/usr/kenlm
LDFLAGS = -L/afs/inf.ed.ac.uk/user/s12/s1270921/usr/kenlm/lib -lkenlm

all : $(OUT_EXEC)
	rm *.o

$(OUT_EXEC) : $(OBJS)
	$(CC) -o $@ $^ $(INCLUDES) $(LDFLAGS)

%.o : %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@ $(INCLUDES)

clean:
	rm -f *.o
	rm -f $(OUT_EXEC)

```

# Run Experiments
Download data/model from [here](https://drive.google.com/file/d/0B6-YKFW-MnbOYnJDeWVXRnlObzA/view?usp=sharing)
```
# move MISC.tar.bz2 to the root folder of this project, then
tar jxvf MISC.tar.bz2
```
**1. Perplexity**
```
cd experiments
./ppl.sh
```

**2. Generation**
```
cd experiments
./generation.sh
```
Enjoy the generated poems!

**3. BLEU**

Download from [here](https://drive.google.com/file/d/0B6-YKFW-MnbORk16WmNXbDhsVk0/view?usp=sharing)
```
tar jxvf BLEU2-final.tar.bz2 
cd BLEU2-final
cd MERT_channel-1_RNN-CB-POS-LM-Eval-BLEU2
python showBLEU.py .
```
