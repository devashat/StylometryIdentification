CC = clang
CFLAGS = -g -Wall -Werror -Wextra -Wpedantic

all: identify

identify: identify.o bv.o bf.o ht.o node.o parser.o pq.o speck.o text.o
	$(CC) -o identify identify.o bv.o bf.o ht.o node.o parser.o pq.o speck.o text.o -lm

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.o
	rm identify

format:
	clang-format -i -style=file *.c *.h

tex:
	pdflatex *.tex
