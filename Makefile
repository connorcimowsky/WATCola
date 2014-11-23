OPT:=-O2 -multi

CXX = u++					# compiler
CXXFLAGS = -g -Wall -Wno-unused-label -MMD ${OPT}

OBJECTS = bank.o bottlingPlant.o config.o nameServer.o parent.o printer.o student.o truck.o vendingMachine.o watcard.o watcardoffice.o main.o
EXEC = soda

DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"

#############################################################

${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@

#############################################################

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXEC}

-include ${DEPENDS}
