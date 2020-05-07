#include "match.h"
#include "player.h"
#include "game_logic.h"
#include <chrono>
#include <vector>
#include <cstdlib>


using namespace std;
using namespace player;
using namespace chrono;


typedef struct Log
{
    vector<double> times;
    vector<double> log_values;
} Log;

Log logs[2];

inline void print_list(vector<double> &list)
{
    cout <<"[";
    for(int i=0;i<list.size()-1;i++)
        cout << list[i]<<", ";
    if(list.size()>0)
        cout << list[list.size()-1];
    cout << "]";
}

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)> PLAYER0_SPEC PLAYER1_SPEC\n"
              << "Player Specification:\n"
              << "\tH [D] [T]\tHeuristic Minimax Player with initial depth [D], and Max Runtime [T].\n"
              << "\tM [R] [T] [S]\tMCTS Player with Max rollouts [R], Max Runtime [T], and Seed [S].\n"
              << "\tR [S]\t\tRandom Player with Seed [S].\n"
              << "Options:\n"
              << "\t-h,--help\tShow this help message\n"
              << "\t-d,--display\tDisplay Game States\n"
              << "\t-s,--stats\tPrint Moves Statistics\n"
              << "\t-n,--num MOVES\tNumber of Moves to be Played\n"
              << std::endl;
}

inline bool is_double(const std::string& s)
{
    try{
        std::stod(s);}
    catch(...){
        return false;}
    return true;
}

inline bool is_int(const std::string& s)
{
    try{
        std::stoi(s);}
    catch(...){
        return false;}
    return true;
}

int main(int argc, char* argv[])
{
    bool mpi_master = true;
#if OMP
    omp_set_num_threads(N_THREADS);
#endif
#if MPI
    int process_count = 1;
    int this_process = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &this_process);
    mpi_master = this_process == 0;
#endif

    bool display = false;
    int max_total_moves = -1;
    bool display_stats = false;
    Player *players[2] = {NULL, NULL};

    if (argc < 5) {
        show_usage(argv[0]);
        return 1;
    }

    int arg_i;
    for(arg_i=1;arg_i<argc;arg_i++)
    {
        string arg = argv[arg_i];
        if(arg=="-h" || arg=="--help")
        {
            show_usage(argv[0]);
            return 0;
        }
        else if(arg == "-d" || arg == "--display")
        {
            display=true;
        }
        else if(arg == "-s" || arg == "--stats")
        {
            display_stats=true;
        }
        else if(arg == "-n" || arg == "--num")
        {
            if(arg_i+1>=argc){
                cerr << "--num option requires one argument." << endl; return 1;
            } else if(!is_int(argv[arg_i+1])){
                cerr << "--num option must be integer." << endl; return 1;
            } else {
                max_total_moves = stoi(argv[arg_i+1]);
                if(max_total_moves<0) {
                    cerr << "--num argument must be non-negative." << endl; return 1;
                } arg_i++;
            }
        }
        else if(arg == "H" || arg == "M" || arg == "R")
        {
            break;
        }
        else
        {
            cerr << arg << " Invlid Option." << endl;
            return 1;
        }
    }
    for(int p=0;arg_i<argc && p<2;p++)
    {
        string arg = argv[arg_i];
        if(arg == "R")
        {
            int seed;
            if(arg_i+1>=argc){
                cerr << "RandomPlayer requires seed value." << endl; return 1;
            }
            arg_i++;
            if(!is_int(argv[arg_i])){
                cerr << "RandomPlayer's Seed value must be integer." << endl; return 1;
            } else {
                seed = stoi(argv[arg_i]);
                if(seed<0) {
                    cerr << "RandomPlayer's Seed value must be non-negative." << endl; return 1;
                } 
            }
            arg_i++;
            players[p]=new RandomPlayer(seed);
        }
        else if (arg == "H")
        {
            int depth;
            double time_budget;
            if(arg_i+2>=argc){
                cerr << "HeuristicMinimaxPlayer requires two arguments" << endl; return 1;
            } 
            arg_i++;
            if(!is_int(argv[arg_i])){
                cerr << "HeuristicMinimaxPlayer's Depth value must be integer." << endl; return 1;
            } else {
                depth = stoi(argv[arg_i]);
                if(depth<0) {
                    cerr << "HeuristicMinimaxPlayer's Depth value must be non-negative." << endl; return 1;
                } 
            }
            arg_i++;
            if(!is_double(argv[arg_i])){
                cerr << "HeuristicMinimaxPlayer's Time Budget value must be double." << endl; return 1;
            } else {
                time_budget = stod(argv[arg_i]);
                if(time_budget<0) {
                    cerr << "HeuristicMinimaxPlayer's Time Budget value must be non-negative." << endl; return 1;
                } 
            }
            arg_i++;
            players[p]=new HeuristicMinimaxPlayer(depth, time_budget);
        }
        else if (arg == "M")
        {
            int rollouts;
            double time_budget;
            int seed;
            if(arg_i+3>=argc){
                cerr << "MCTSPlayer requires three arguments" << endl; return 1;
            } 
            arg_i++;
            if(!is_int(argv[arg_i])){
                cerr << "MCTSPlayer's Rollouts value must be integer." << endl; return 1;
            } else {
                rollouts = stoi(argv[arg_i]);
                if(rollouts<0) {
                    cerr << "MCTSPlayer's Rollouts value must be non-negative." << endl; return 1;
                } 
            }
            arg_i++;
            if(!is_double(argv[arg_i])){
                cerr << "MCTSPlayer's Time Budget value must be double." << endl; return 1;
            } else {
                time_budget = stod(argv[arg_i]);
                if(time_budget<0) {
                    cerr << "MCTSPlayer's Time Budget value must be non-negative." << endl; return 1;
                } 
            }
            arg_i++;
            if(!is_int(argv[arg_i])){
                cerr << "MCTSPlayer's Seed value must be integer." << endl; return 1;
            } else {
                seed = stoi(argv[arg_i]);
                if(seed<0) {
                    cerr << "MCTSPlayer's Seed value must be non-negative." << endl; return 1;
                } 
            }
            arg_i++;
            players[p]=new MCTSPlayer(rollouts, time_budget, seed);
        }
        else
        {
            cerr << arg << " Invlid Player Specification." << endl;
            return 1;
        }
    }
    if(players[0]==NULL || players[1]==NULL)
    {
        cerr <<  " Two players must be specified." << endl;
        return 1;
    }

    display = display & mpi_master;
    display_stats = display_stats & mpi_master;
    

    //keep two states, the state and the clone of the state
    state_t state, state_c;
    move_t move_c;
    init_state(&state);
    bool first_move = true;
    vector<move_t*> moves;

    int number_of_simulated_moves = 0;

    while(!is_terminal(&state) && (max_total_moves==-1 || number_of_simulated_moves<max_total_moves))
    {
        if(display)
        {
            cout << stringify(&state);
            cout << "-------------\n";
        }

        clone_state(&state_c, &state);
        //generate all possible moves in this state
        possible_moves(&state, moves);
        int next_move_id;
        pair<int, double> rst;
        int player_index = state.next_player;
        auto start = system_clock::now();
#if MPI
        MPI_Barrier(MPI_COMM_WORLD);
#endif
        if(first_move)
        {
            rst = players[player_index]->next_move(&state_c, NULL);
            first_move=false;
        }
        else
            rst = players[player_index]->next_move(&state_c, &move_c);
        auto end =  system_clock::now();
        next_move_id = rst.first;
        if(display_stats)
        {
            double time =duration_cast<milliseconds>(end-start).count()/1000.0;
            if(display)
                cout << "(player index, time, log value) = " <<player_index <<", "<< time << ", " << rst.second << endl;
            logs[player_index].times.push_back(time);
            logs[player_index].log_values.push_back(rst.second);
        }
        move_c.move_id=-1;
        //Go through all possible moves, make a linear search on the move_id
        for(int i=0;i<moves.size();i++)
            if(next_move_id==moves[i]->move_id)
            {
                clone_move(&move_c, moves[i]);
                break;
            }
        //If the player has chosen an invalid move, simply choose the first move in the possible move list
        //If we do that correctly, we will not use that
        if(move_c.move_id==-1)
        {
            clone_move(&move_c, moves[0]);
            if(mpi_master){
                cout << next_move_id << " is invalid move index, " << move_c.move_id << " is applied.\n";
            }
        }
        for(int i=0;i<moves.size();i++)
            free(moves[i]);
        apply_move(&state, &move_c);
        number_of_simulated_moves++;
    }
    if(display)
    {
        cout << stringify(&state);
        cout << "-------------\n";
    }
    if(is_terminal(&state))
        cout << "Winner: " << (get_winner(&state)==BLACK? "BLACK":"WHITE") << endl;
    if(display_stats)
    {
        for(int p=0;p<2;p++)
        {
            cout << "Player "<<p<<" Move Times: ";
            print_list(logs[p].times);
            cout << endl;
            cout << "Player "<<p<<" Log Values: ";
            print_list(logs[p].log_values);
            cout << endl;
        }
    }
#if MPI
    MPI_Finalize();
#endif  
}
