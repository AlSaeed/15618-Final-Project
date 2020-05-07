#include "player.h"
#include "game_logic.h"
using namespace player;

RandomPlayer::RandomPlayer()
{
    gen.seed(0);
}

RandomPlayer::RandomPlayer(int seed)
{
    gen.seed(seed);
}

std::pair<int, double> RandomPlayer::next_move(state_t* state, move_t* prev_move)
{
    // std::vector<move_t*> moves;
    // possible_moves(state, moves);
    // std::uniform_int_distribution<int> dist(0,moves.size()-1);
    // int m = moves[dist(gen)]->move_id;
    // for(int i=0;i<moves.size();i++)
    //     free(moves[i]);

    std::uniform_real_distribution<double> udist;
    move_t m = get_random_move(state, udist, gen);

    return std::pair<int, double>(m.move_id,0);
}
