# change the target if you need
OUT_EXEC = rnnpg-generator
OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

CC = g++
CPPFLAGS = -O3 -funroll-loops -ffast-math -finline-functions -Wall -Winline -pipe -DKENLM_MAX_ORDER=6

INCLUDES = -I/home/wxuan/poem/demo/kenlm
LDFLAGS = -L/home/wxuan/poem/demo/kenlm/build/lib -lkenlm -L/home/wxuan/poem/demo/kenlm/build/lib -lkenlm_util -L/usr/local/lib -lz

all : $(OUT_EXEC)
	rm *.o

$(OUT_EXEC) : $(OBJS)
	$(CC) -o $@ $^ $(INCLUDES) $(LDFLAGS)

%.o : %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@ $(INCLUDES)

clean:
	rm -f *.o
	rm -f $(OUT_EXEC)

