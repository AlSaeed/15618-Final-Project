#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "string.h"
#include "match.h"

// Fills state with data of initial state.
void init_state(state_t* state);

// Returns true if and only if state corresponds to a terminal state.
bool is_terminal(state_t* state);

// If state is terminal state, returns the index of the winning player.
int get_winner(state_t* state);

// Clears and fills the vector moves will all possible moves from state.
void possible_moves(state_t* state, std::vector<move_t*> &moves);

// Writes all possible moves from state to the array moves.
int possible_moves_omp(state_t* state, move_t* moves);

// Samples a random move from state using the uniform distribution udist,
// and the random engine gen.
move_t get_random_move(state_t* state, std::uniform_real_distribution<double> &udist,
                       std::default_random_engine &gen);

// Results in dest being an identical state to source.
void clone_state(state_t* dest, state_t* source);

// Results in dest being an identical move to source.
void clone_move(move_t* dest, move_t* source);

// Alters state in-place to represent the effect of making move.
void apply_move(state_t* state, move_t* move);

// Reverses the effect of move on state in-place.
void reverse_move(state_t* state, move_t* move);

// Returns a string that represents state.
std::string stringify(state_t* state);

#endif