CFLAGS  = -std=c99 -g -Wno-unused-result -O3 

#------------------------------------------------------------------------------
.PHONY : all clean

#------------------------------------------------------------------------------
all : floodit_solver

floodit_solver : floodit_solver.c mapa.h mapa.c
	$(CC) $(CFLAGS) \
	      -Wno-cast-qual \
              -Wno-padded \
              -Wno-undef \
              -o $@ \
              $^ \
	      -l cgraph

#------------------------------------------------------------------------------
clean :
	$(RM) floodit_solver *.o

