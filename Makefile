CC=g++ -Wall -Wextra -Dkevin -DJZQ -O2 -Wno-unused-result -static -std=c++14
CLEAR=cat /dev/null > 

.PHONY: build-duipai build clean zip

build-duipai: a b rand main

build: main

main: main.cpp
	$(CC) -o main main.cpp -lpthread

%: %.cpp
	$(CC) -o $@ $<

init:
	rm a b rand a.txt b.txt in.txt diff.txt mduipai.tar.gz *-*.txt

# 请不要随意使用make clean，否则你的代码将会丢失
clean:
	$(CLEAR) a.cpp
	$(CLEAR) b.cpp
	$(CLEAR) rand.cpp
	rm a b rand a.txt b.txt in.txt diff.txt mduipai.tar.gz *-*.txt
	
tar: mduipai.tar.gz

mduipai.tar.gz:
	tar zcvf mduipai.tar.gz *