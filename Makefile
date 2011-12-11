.PHONY: demo
demo: demo1 demo2 demo3

.PHONY: all
all: part1 part2 part3

part1:
	g++ part1.c -o part1

.PHONY: demo1
demo1: part1
	./part1
	@echo See test0.bin, test1.bin and tes40129.bin for evidence.

part2:
	g++ part2.c -o part2

.PHONY: demo2
demo2: part2
	./part2
	cmp testread.gif testwrite.gif

.PHONY: part3
part3: part3-producer part3-consumer

.PHONY: demo3
demo3: part3
	./part3-producer &
	@echo Pausing for shared memory set up
	@sleep 1
	./part3-consumer

.PHONY: clean3
clean3:
	-killall part3-producer
	-killall part3-consumer
	-ipcrm -M 18021

.PHONY: test3
test3: clean3 demo3

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
