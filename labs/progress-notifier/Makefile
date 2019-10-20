OBJFILES = logger.o base64.o
TARGET   = base64

all:
	gcc -c logger.c -o logger.o
	gcc -c base64.c -o base64.o
	gcc base64.o logger.o -o base64

clean:
	rm -f $(OBJFILES) $(TARGET) *~

include ../common.mk
