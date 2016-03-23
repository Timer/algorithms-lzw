COMPILER=g++ -std=c++11
EXECUTABLES=lzw435 lzw435M

all:
	make $(EXECUTABLES)

lzw435: lzw435.cpp
	$(COMPILER) -o $@ $^

lzw435M: lzw435M.cpp
	$(COMPILER) -o $@ $^

clean:
	rm -f $(EXECUTABLES)
