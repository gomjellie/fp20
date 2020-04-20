
all:
	gcc fdevicedriver.c ftlmgr.c -o run

test_create:
	./run c flashfile 100

test_write:
	./run w flashfile 16 "abcde12345#@444" "5"

test_read:
	./run r flashfile 16

test_erase:
	./run e flashfile 4

clean:
	rm -f flashfile run
