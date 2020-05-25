# 15-618 Final Project: Parallelizing Game Playing Agents for Abalone

This was the final project for *15-618 Parallel Computer Architecture and Programming* [Spring 2020](http://www.cs.cmu.edu/afs/cs.cmu.edu/academic/class/15418-s20/www/) offering. The project explored the potential for parallelism in implementing game playing agents for the game of Abalone. Two methodologies of designing playing agents were explored, namely Heuristic Minimax Search and Monte Carlo Tree Search. Both methods were implemented in [OpenMP](https://www.openmp.org/) as a shared memory parallel program, achieving speedups of 6.17 and 7.43 respectively on the 8-cores GHC machines. Additionally, the Heuristic Minimax Search agent was implemented in [MPI](https://www.mpi-forum.org/) as a message-passing parallel program, achieving a speedup of 6.90 in the same setting. 

## Abalone

![The Initial Position in a Game of Abalone](https://www.nerdly.co.uk/wp-content/uploads/2018/03/abalone-1.jpg)

[Abalone](https://en.wikipedia.org/wiki/Abalone_(board_game)) is a two-players strategy board-game on a hexagonal grid. Each player starts with 14 marbles, and take turns in making moves consisting of a line of 1,2, or 3 marbles in any of the 6 directions. A player can push opponent's marbles only if the move was in-line (same direction as the line of marbles), the number of player's marbles is greater than the opponent's marbles, and the opponent's marbles are pushed into an empty location or outside the grid. The first player to push 6 of their opponent's marbles outside the grid is considered the winner.

<!--## Heuristic Minimax Search

## Monte Carlo Tree Search-->

## Usage 
### Prerequisites
- [MPI](https://www.mpi-forum.org/) 
- [OpenMP](https://www.openmp.org/)
### Building
    make all
will build *program-seq* the sequential implementation, *program-omp* the parallel OpenMP-based implementation, and *program-mpi* the parallel MPI-based implementation.
Additionally, for building a specific implementation you can simply run:

    make program-X
### Command Line Interface
The program can be run using the following template:

    program-X <option(s)> PLAYER0_SPEC PLAYER1_SPEC

where `program-X` stands for one of the three valid implementations.
#### Options
- `-h,--help`: To show help message.
- `-d,--display`: To print game states during the game-play.
- `-s,--stats`: To print the moves' logged statistics according to the player type.
- `-n,--num MOVES`: To play the game to a maximum of `MOVES` number of moves.

#### Player Specification
The player can be specified to be one of the three following types of players:
* **Random Player**: `R [S]` where `[S]` is the seed of the Random Player.
* **Heuristic Minimax Player**:  `H [D] [T]` where `[D]` is the initial exploration depth, and `[T]` is the maximum time per move, which is respected unless the the initial exploration depth hasn't been fully explored.
* **MCTS Player**: `M [R] [T] [S]` where `[R]` is the maximum number of rollouts to be simulated per move, `[T]` is the maximum time per move, and `[S]` is the seed to the MCTS Player.
#### Examples

    program-seq -d R 0 H 5 5
This will execute the sequential implementation of the agents, printing the game state at each move, the first player is a Random player with seed 0, and the second player is Heuristic Minimax Player with initial exploration depth of 5 and maximum time of 5 seconds per move.

    program-omp -s -n 50 H 5 5 M 10000 5 0
This will execute the OpenMP implementation of agents, printing the agents' moves' statistics at the end of the program, playing for a maximum of 50 moves total. The first player is Heuristic Minimax Player with initial exploration depth of 5 and maximum time of 5 seconds per move. The second player is MCTS player with maximum of 10000 rollouts per move, a maximum of 5 seconds per move, and a seed of 0. 

## Files

* **game_logic.h**: a header file for the game logic functions, e.g. initializing a game state, getting list of legal moves from a game state, etc.
* **game_logic.cpp**: an implementation of the functions declared in **game_logic.h**.
* **player.h**: a header file for *Player* abstract class and its subclasses *RandomPlayer*, *MCTSPlayer*, and *HeuristicMinimaxPlayer*. Additionally it declares data structures needed for these classes.
* **random_player.cpp**: an implementation of  *RandomPlayer* class, which responds with a uniformly sampled legal move at each position.
* **mcts_player.cpp**: an implementation of *MCTSPlayer* class, which selects the next move at each game state based on Monte Carlo Tree Search algorithm.
* **heuristic_minimax_player.cpp**: an implementation of *HeuristicMinimaxPlayer* class, which selects the next move at each game state based on the Heuristic Minimax Search Algorithm with iterative deepening.
* **heuristic_minimax_mpi_util.cpp**: utility functions for the MPI implementation of *HeuristicMinimaxPlayer*.
* **all_moves.h**: a header file that contains an exhaustive list of all possible moves with the condition and effect of each move.
* **generate_moves_table.cpp**: a code of a program that is independently run to generate the **all_moves.h** file.
* **match.h**: a header file that declares the fundamental data-structures, e.g. game state, move, etc. It also declares constants utilized across multiple files.
* **match.cpp**: contains the *main* function which parses the command line argument and executes the game loop.

## Contributors
* [**Wael  H. Al Saeed**](https://github.com/AlSaeed) - <wsaeed@cs.cmu.edu>.
* [**Yumeng Wang**](https://github.com/yumengwDoor) - <yumengw@andrew.cmu.edu>.
