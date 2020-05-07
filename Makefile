CC=g++

OMP=-fopenmp -DOMP
MPI=-DMPI
MPICC = mpiCC

DEBUG=0
INSTRUMENT=1
FLAGS= -DDEBUG=$(DEBUG) -DTRACK=$(INSTRUMENT) -std=c++11

CPPFILES = match.cpp game_logic.cpp random_player.cpp heuristic_minimax_player.cpp heuristic_minimax_mpi_util.cpp mcts_player.cpp
HFILES = match.h game_logic.h all_moves.h player.h


all: program-seq program-omp program-mpi
program-seq: $(CPPFILES) $(HFILES)
	$(CC) $(FLAGS) -o program-seq $(CPPFILES) 
program-omp: $(CPPFILES) $(HFILES)
	$(CC) $(FLAGS) $(OMP) -o program-omp $(CPPFILES)  
program-mpi: $(CPPFILES) $(HFILES)
	$(MPICC) $(FLAGS) $(MPI) -o program-mpi $(CPPFILES) 

clean:
	rm -f program-omp
	rm -f program-seq
	rm -f program-mpi
