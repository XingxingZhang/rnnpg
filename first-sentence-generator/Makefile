# change the target if you need
OUT_EXEC = first-sent
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

