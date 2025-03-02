CC = clang
CFLAGS = -g -Wall -fPIC -DDEBUG
LDFLAGS = -ldl -lbsd 
FORMAT_FLAGS = -style=file  # Uses .clang-format if available

all: rod_cut_lru rod_cut_rr

OBJS = main.o cut_list.o piece_values.o vec.o cache.o

liblru.so: least_recently_used.o
	$(CC) -shared -o $@ $^ $(CFLAGS)

librr.so: random_replacement.o
	$(CC) -shared -o $@ $^ $(CFLAGS)  

rod_cut_lru: $(OBJS) liblru.so
	$(CC) -o $@ $(OBJS) -L. -llru $(CFLAGS) $(LDFLAGS)  # Links to liblru.so

rod_cut_rr: $(OBJS) librr.so
	$(CC) -o $@ $(OBJS) -L. -lrr $(CFLAGS) $(LDFLAGS)  # Links to librr.so

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f rod_cut_lru rod_cut_rr *.o *.so

# Clang-Format rule to format all C and header files
format:
	clang-format -i $(wildcard *.c *.h) $(FORMAT_FLAGS)
