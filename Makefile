.PHONY: clean mrproper

CC=gcc
CFLAGS=-g -Wall
LDFLAGS=
LDLIBS=

TARGET=tsh

OUTDIR=out

SRC=$(wildcard src/*.c)
OBJ=$(SRC:src/%.c=$(OUTDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) -o $(TARGET) $(OBJ)

$(OUTDIR)/%.o: src/%.c
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTDIR)

mrproper:
	rm -rf $(TARGET)

docker: all
	docker build -t projet .
	docker run -it projet
