OPT:=-O2 -multi

CXX = u++					# compiler
CXXFLAGS = -g -Wall -Wno-unused-label -MMD ${OPT}

OBJECTS = config.o
EXEC = soda

DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"

#############################################################

${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} -o ${EXEC}

#############################################################

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXEC}

-include ${DEPENDS}
