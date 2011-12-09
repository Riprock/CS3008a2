.PHONY: all
all: part1 part2 part3
	./part1
	@echo See test0.bin, test1.bin and tes40129.bin for evidence.
	
	./part2
	cmp testread.gif testwrite.gif
	
	./part3-producer &
	@echo Pausing for shared memory set up
	@sleep 1
	./part3-consumer

part1:
	g++ part1.c -o part1

part2:
	g++ part2.c -o part2

.PHONY: part3
part3: part3-producer part3-consumer

fcrc.o:
	g++ -c fcrc.c

part3-producer: fcrc.o
	g++ part3-producer.c -o part3-producer fcrc.o

part3-consumer: fcrc.o
	g++ part3-consumer.c -o part3-consumer fcrc.o

.PHONY: clean
clean:
	-rm -f *.o
	-rm -f part1
	-rm -f test*.bin
	-rm -f part2
	-rm -f testwrite.gif
	-rm -f part3-producer
	-rm -f part3-consumer
