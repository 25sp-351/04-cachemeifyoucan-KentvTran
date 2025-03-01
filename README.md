##Assignment 04

to run 
- make
- LD_LIBRARY_PATH=. ./rod_cut_rr input.txt or LD_LIBRARY_PATH=. ./rod_cut_lru input.txt
- then enter rod_length as an int

in main.c you have to change cache policy in the code on line 37 
librr.so for random replacement
liblru.so for least recently used

input.txt contains (length, value) pairs edit as you please accordingly

reused vec.c/.h, piece_values.c/.h cut_list.c/.h and cache.c/.h as provided by Prof. Bruce Mckenzie as I did rod_cutting using dynamic programming