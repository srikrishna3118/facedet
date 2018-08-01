CC = g++
CFLAGS = -fpermissive -g
CVFLAGS = `pkg-config --cflags opencv`
LDFLAGS = `pkg-config --libs opencv`




.PHONY: all clean

facedet: main.o pic_data.o facedet.o 
	$(CC) $(CFLAGS) $(CVFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) $(CVFLAGS) $(LDFLAGS)  -c $^ -o $@

facedet.o: facedet.c
	$(CC) $(CFLAGS) -c  $^ -o $@

pic_data.o: pic_data.c
	$(CC) -c $(CFLAGS) $^ -o $@

all: facedet

clean:
	rm -f *.o facedet *.d
