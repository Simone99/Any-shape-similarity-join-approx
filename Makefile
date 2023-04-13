#
# Makefile
#

target :
	g++ -g -std=c++20 -Wall -o main main.cpp Database.cpp Grid.cpp Graph.cpp

clean :
	rm -f file3_text.txt
	rm -rf main

distclean : clean
	rm -rf core
	rm -rf *~