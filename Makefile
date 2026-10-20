PROGRAM = decrypt-jakx-elf
SOURCES = decrypt-jakx-elf.cpp minilzo.c 

default:
	@echo ""
	@echo "Please choose one of the following 'make' targets:"
	@echo ""
	@echo "    g++:   g++"
	@echo "    clang++:  clang++"
	@echo ""


# Make sure that minilzo.h, lzoconf.h and lzodefs.h are in the
# current dircectory. Otherwise you may want to adjust CPPFLAGS.
CPPFLAGS = -I. -I./include/

GCC_CFLAGS = -s -Wall -O2 -fomit-frame-pointer

g++:
	g++ $(CPPFLAGS) $(GCC_CFLAGS) -o $(PROGRAM) $(SOURCES)

clang++:
	clang++ $(CPPFLAGS) $(GCC_CFLAGS) -o $(PROGRAM) $(SOURCES)

clean:
	rm -f $(PROGRAM) $(PROGRAM).exe $(PROGRAM).map $(PROGRAM).tds
	rm -f *.err *.o *.obj

.PHONY: default clean
