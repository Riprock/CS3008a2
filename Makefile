all: part1 part2
	./part1
	@echo See test0.bin, test1.bin and tes40129.bin for evidence.
	
	./part2
	cmp testread.gif testwrite.gif

part1:
	g++ part1.c -o part1

part2:
	g++ part2.c -o part2
