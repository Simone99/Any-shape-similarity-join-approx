#
# Makefile
#

target :
	g++ -g -Wall -o main main.cpp Database.cpp Grid.cpp

clean :
	rm -f file3_text.txt
	rm -rf main

distclean : clean
	rm -rf core
	rm -rf *~