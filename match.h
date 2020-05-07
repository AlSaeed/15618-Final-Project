#ifndef MATCH_H
#define MATCH_H

#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <cstring>
#include <utility>

#define BUFFER_SIZE 150


#ifndef OMP
#define OMP 0
#endif

#define BLACK 0
#define WHITE 1
#define VOID 2
#define INVALID 3

#define N_THREADS 8

#if OMP
#include <omp.h>
#endif

#if MPI
#include <mpi.h>
#endif

typedef struct {
    // 2D representation of the marbles in the board (not all grids valid)
    char board[11][19];
    // Which player has the next turn
    char next_player;
    // Number of marbles fallen outside the board
    char fallen_marbles[2];

    // Quantities maintained for computing Heuristic value
    char n_marbles_on_edge[2];
    // Aggregated distances of balls to the center
    char centrality[2];
    // Adjacent pair marbles
    char n_adjacent_pairs[2];
    // A measure of who occupies the region
    char occupying_center[2];

} state_t;

//The change of the board, which position, from which to which
typedef struct {
    char r;
    char c;
    char from;
    char to;
} delta_t;

//The changes on the board. Each move will change at most 6 piecies on the board
typedef struct {
    //unique move_id
    int move_id;
    delta_t delta_board[6];
    char delta_fm[2];
    char delta_nmoe[2];
    char delta_c[2];
    char delta_nap[2];
    char delta_oc[2];
} move_t;

//A move and a heuristic value for that move
typedef struct {
    move_t* move;
    double value;
} edge_t;

#endif