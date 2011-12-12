.PHONY: demo
demo: demo1 demo2 demo3 demo4 demo5

.PHONY: all
all: part1 part2 part3 part4 part5

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
	-ipcrm -M 18021 -S 18022

.PHONY: test3
test3: clean3 demo3

fcrc.o:
	g++ -c fcrc.c

part3-producer: fcrc.o
	g++ part3-producer.c -o part3-producer fcrc.o

part3-consumer: fcrc.o
	g++ part3-consumer.c -o part3-consumer fcrc.o

.PHONY: part4
part4: part4-producer part4-consumer

.PHONY: demo4
demo4: part4
	./part4-producer &
	@echo Pausing for shared memory set up
	@sleep 1
	./part4-consumer
	@echo See part4-evidence.txt for evidence.

.PHONY: clean4
clean4:
	-killall part4-producer
	-killall part4-consumer
	-ipcrm -M 18023 -S 18024
	-rm -f part4-evidence.txt

.PHONY: test4
test4: clean4 demo4

part4-producer: fcrc.o
	g++ part4-producer.c -o part4-producer fcrc.o

part4-consumer: fcrc.o
	g++ part4-consumer.c -o part4-consumer fcrc.o

part5:
	g++ part5.c -o part5

.PHONY: demo5
demo5: part5
	@echo Writing to mapped file "part5-mapped.bin".
	./part5
	@echo Walking list and print strings.
	./part5 -r

.PHONY: clean
clean:
	-rm -f *.o
	-rm -f part1
	-rm -f test*.bin
	-rm -f part2
	-rm -f testwrite.gif
	-rm -f part3-producer
	-rm -f part3-consumer
	-rm -f part4-producer
	-rm -f part4-consumer
	-rm -f part4-evidence.txt
	-rm -f part5
	-rm -f part5-mapped.bin
