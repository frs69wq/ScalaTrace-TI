SCALATRACE_HOME=../..

MPI_CC:=mpicxx
CFLAGS:=-Wall -g -O0

LDFLAGS:=-L${SCALATRACE_HOME}/record/lib -L${SCALATRACE_HOME}/stackwalk/ver0
LIBS:=-lglob -lstackwalk -lpapi

TARGET=stencil
SOURCE=$(addsuffix .c, $(basename $(TARGET)))

all: ${TARGET}

${TARGET} : ${SOURCE}
	$(MPI_CC) $(CFLAGS) $(LDFLAGS) ${SOURCE} -o ${TARGET} $(LIBS)

clean: 
	rm -rf *.o *~ ${TARGET} 

veryclean: clean
	rm -rf trace_*

