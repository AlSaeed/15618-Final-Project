#ifndef PLAYER_H
#define PLAYER_

#include <mutex>  
#include "match.h"
namespace player
{
    //Value of the node and best move it has explored
    typedef struct {
        double value;
        int move_id;
    } return_t;
    class Player
    {
    //The prev_move is not used for all time, is for Monte Carlo
    //Return the move_id
    public:
        virtual std::pair<int, double> next_move(state_t* state, move_t* prev_move)=0;
    };

    //Pick the random move among all possible moves
    class RandomPlayer: public Player
    {
        private:
            std::default_random_engine gen;
        public:
            RandomPlayer();
            RandomPlayer(int seed);
            std::pair<int, double> next_move(state_t* state, move_t* prev_move);
    };

    class HeuristicMinimaxPlayer: public Player
    {
        private:
            int init_depth;
            double time_budget;
            std::chrono::time_point<std::chrono::system_clock> start_time;
            int determined_move;
            bool compute_interrupted;
            int player_col;
            int adv_col;
            double heuristic_value(state_t* state);
            double move_heuristic_value(state_t* state, move_t* move);
            return_t process(state_t* state, double alpha, double beta, int depth, int MAX_DEPTH);
            return_t process_omp(state_t* state, double alpha, double beta, int depth, int MAX_DEPTH);
            return_t process_mpi(state_t* state, double alpha, double beta, int depth, int MAX_DEPTH);

        public:
            HeuristicMinimaxPlayer();
            HeuristicMinimaxPlayer(int init_d, double t_budget);
            std::pair<int, double> next_move(state_t* state, move_t* prev_move);
    };

    typedef struct MCTS_Node Node;

    typedef struct MCTS_Edge{
        MCTS_Node* parent;
        MCTS_Node* node;
        int move_id;
        int N; //visitation count
        int N_effective;
        int W; // Total Action Value
        double q;// mean action value
        double p;// prior probability
        #if OMP 
        std::mutex mtx;
        #endif

    } MCTS_Edge;

    struct MCTS_Node{
        MCTS_Edge* parent;
        state_t state;
        bool expanded;
        int n_children;
        #if OMP 
        std::mutex mtx;
        #endif
        MCTS_Edge children[BUFFER_SIZE];
    };

    class MCTSPlayer: public Player
    {
        private:
            double time_budget;
            double start_time;
            int player_col;
            int adv_col;
            int max_rollouts;
            std::default_random_engine gen;
            std::normal_distribution<double> n_dist;
            std::uniform_real_distribution<double> u_dist;
            MCTS_Node* root;
            void descend_from_root(int move_id);
            double heuristic_value(state_t* state);
            int rollout(state_t* state, std::uniform_real_distribution<double> &t_dist, std::default_random_engine &t_gen);
            double move_heuristic_value(state_t* state, move_t* move);
        public:
            MCTSPlayer();
            MCTSPlayer(int max_rollouts, double time_budget, int seed);
            std::pair<int, double> next_move(state_t* state, move_t* prev_move);
    };

}

#if MPI
using namespace player;
/* These functions will support the required communication operations */
int assign_one_move(std::vector<edge_t> &edges, int process_id, double* result_buffer, MPI_Request* send_req, MPI_Request* recv_req);

int receive_one_move();

bool receive_one_finished_result(std::vector<edge_t> &edges, int process_id, double* result_buffer, return_t* result, MPI_Request* send_req,  MPI_Request* recv_req, int* process_assigend_task);

void send_one_finished_result(double value, bool compute_interrupted);

void update_alpha(double alpha, int process_count);

void receive_updated_alpha(double* alpha, MPI_Request* req);

void send_final_result(MPI_Request *send_reqs, int process_count, int move_id);

int receive_final_result();

#endif

#endif