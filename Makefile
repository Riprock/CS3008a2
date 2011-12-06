all: part1
	./part1
	@echo See test0.bin, test1.bin and tes40129.bin for evidence.

part1:
	g++ part1.c -o part1
