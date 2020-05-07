#include "match.h"
#include "game_logic.h"
#include "all_moves.h"
#define BLACK_SYMBOL "@"
#define WHITE_SYMBOL "O"
#define VOID_SYMBOL "."
#define INVALID_SYMBOL " "

using namespace std;

typedef struct {
    int dr;
    int dc;
} Dir;
const Dir all_dirs[6]={{0,2},{0,-2},{1,1},{-1,-1},{-1,1},{1,-1}};

const char centrality_matrix[11][19] = {
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1, 4,-1, 4,-1, 4,-1, 4,-1, 4,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1, 4,-1, 3,-1, 3,-1, 3,-1, 3,-1, 4,-1,-1,-1,-1},
    {-1,-1,-1, 4,-1, 3,-1, 2,-1, 2,-1, 2,-1, 3,-1, 4,-1,-1,-1},
    {-1,-1, 4,-1, 3,-1, 2,-1, 1,-1, 1,-1, 2,-1, 3,-1, 4,-1,-1},
    {-1, 4,-1, 3,-1, 2,-1, 1,-1, 0,-1, 1,-1, 2,-1, 3,-1, 4,-1},
    {-1,-1, 4,-1, 3,-1, 2,-1, 1,-1, 1,-1, 2,-1, 3,-1, 4,-1,-1},
    {-1,-1,-1, 4,-1, 3,-1, 2,-1, 2,-1, 2,-1, 3,-1, 4,-1,-1,-1},
    {-1,-1,-1,-1, 4,-1, 3,-1, 3,-1, 3,-1, 3,-1, 4,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1, 4,-1, 4,-1, 4,-1, 4,-1, 4,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};

inline void add_delta_board(move_t* move, int i, int r, int c, int from, int to)
{
    move->delta_board[i].r = r;
    move->delta_board[i].c = c;
    move->delta_board[i].from = from;
    move->delta_board[i].to = to;
}

inline void clean_move(move_t* move)
{
    move->move_id=0;
    for(int i=0;i<6;i++)
        move->delta_board[i].r=move->delta_board[i].c=-1;
    for(int i=0;i<2;i++)
        move->delta_fm[i]=move->delta_nmoe[i]=move->delta_c[i]=move->delta_nap[i]=move->delta_oc[i]=0;
}

move_t* new_move()
{
    move_t *move = (move_t*)malloc(sizeof(move_t));
    clean_move(move);
    return move;
}

void apply_move(state_t* state, move_t* move)
{
    for(int i=0;i<6;i++)
        if(move->delta_board[i].r!=-1)
        {
            int r = move->delta_board[i].r;
            int c = move->delta_board[i].c;
            int v = move->delta_board[i].to;
            state->board[r][c]=v;
        }
    state->next_player = BLACK + WHITE - state->next_player;

    for(int i=0;i<2;i++)
    {
        state->fallen_marbles[i]+=move->delta_fm[i];
        state->n_marbles_on_edge[i]+=move->delta_nmoe[i];
        state->centrality[i]+=move->delta_c[i];
        state->n_adjacent_pairs[i]+=move->delta_nap[i];
        state->occupying_center[i]+=move->delta_oc[i];
    }
}

void reverse_move(state_t* state, move_t* move)
{
    for(int i=0;i<6;i++)
        if(move->delta_board[i].r!=-1)
        {
            int r = move->delta_board[i].r;
            int c = move->delta_board[i].c;
            int v = move->delta_board[i].from;
            state->board[r][c]=v;
        }

    state->next_player = BLACK + WHITE - state->next_player;
    for(int i=0;i<2;i++)
    {
        state->fallen_marbles[i]-=move->delta_fm[i];
        state->n_marbles_on_edge[i]-=move->delta_nmoe[i];
        state->centrality[i]-=move->delta_c[i];
        state->n_adjacent_pairs[i]-=move->delta_nap[i];
        state->occupying_center[i]-=move->delta_oc[i];
    }
}

void fill_delta_stats(state_t* state, move_t* move)
{
    for(int i=0;i<6;i++)
    {
        if(move->delta_board[i].r==-1)
            continue;
        int r = move->delta_board[i].r,
            c = move->delta_board[i].c,
            from = move->delta_board[i].from,
            to = move->delta_board[i].to;
        int centrality = centrality_matrix[r][c];

        if(from==WHITE || from==BLACK)
        {
            move->delta_fm[from]++;
            if(centrality==4)
                move->delta_nmoe[from]--;
            move->delta_c[from]-=centrality;
            if(centrality==0)
                move->delta_oc[from]-=3;
            if(centrality==1)
                move->delta_oc[from]--;
        }

        if(to==WHITE || to==BLACK)
        {
            move->delta_fm[to]--;
            if(centrality==4)
                move->delta_nmoe[to]++;
            move->delta_c[to]+=centrality;
            if(centrality==0)
                move->delta_oc[to]+=3;
            if(centrality==1)
                move->delta_oc[to]++;
        }

        for(int d=0;d<6;d++)
        {
            Dir D = all_dirs[d];
            int r1 = r + D.dr,
                c1 = c + D.dc;
            bool from_ok = (from==WHITE || from==BLACK) && (state->board[r1][c1]==from);
            bool to_ok = (to==WHITE || to==BLACK) && (state->board[r1][c1]==to);
            for(int j=0;j<6;j++)
            {
                if(move->delta_board[j].r==-1)
                    continue;
                if(move->delta_board[j].r==r1 &&
                   move->delta_board[j].c==c1)
                   from_ok=to_ok=false;
            }
            if(from_ok)
                move->delta_nap[from]--;
            if(to_ok)
                move->delta_nap[to]++;
        }
    }

     
}

void init_state(state_t* state)
{

    state->board[7][7]=state->board[7][9]=state->board[7][11]=state->board[8][4]=state->board[8][6]=state->board[8][8]=state->board[8][10]=state->board[8][12]=state->board[8][14]=state->board[9][5]=state->board[9][7]=state->board[9][9]=state->board[9][11]=state->board[9][13]=BLACK;
    state->board[1][5]=state->board[1][7]=state->board[1][9]=state->board[1][11]=state->board[1][13]=state->board[2][4]=state->board[2][6]=state->board[2][8]=state->board[2][10]=state->board[2][12]=state->board[2][14]=state->board[3][7]=state->board[3][9]=state->board[3][11]=WHITE;
    state->board[3][3]=state->board[3][5]=state->board[3][13]=state->board[3][15]=state->board[4][2]=state->board[4][4]=state->board[4][6]=state->board[4][8]=state->board[4][10]=state->board[4][12]=state->board[4][14]=state->board[4][16]=state->board[5][1]=state->board[5][3]=state->board[5][5]=state->board[5][7]=state->board[5][9]=state->board[5][11]=state->board[5][13]=state->board[5][15]=state->board[5][17]=state->board[6][2]=state->board[6][4]=state->board[6][6]=state->board[6][8]=state->board[6][10]=state->board[6][12]=state->board[6][14]=state->board[6][16]=state->board[7][3]=state->board[7][5]=state->board[7][13]=state->board[7][15]=VOID;
    state->board[0][0]=state->board[0][1]=state->board[0][2]=state->board[0][3]=state->board[0][4]=state->board[0][5]=state->board[0][6]=state->board[0][7]=state->board[0][8]=state->board[0][9]=state->board[0][10]=state->board[0][11]=state->board[0][12]=state->board[0][13]=state->board[0][14]=state->board[0][15]=state->board[0][16]=state->board[0][17]=state->board[0][18]=state->board[1][0]=state->board[1][1]=state->board[1][2]=state->board[1][3]=state->board[1][4]=state->board[1][6]=state->board[1][8]=state->board[1][10]=state->board[1][12]=state->board[1][14]=state->board[1][15]=state->board[1][16]=state->board[1][17]=state->board[1][18]=state->board[2][0]=state->board[2][1]=state->board[2][2]=state->board[2][3]=state->board[2][5]=state->board[2][7]=state->board[2][9]=state->board[2][11]=state->board[2][13]=state->board[2][15]=state->board[2][16]=state->board[2][17]=state->board[2][18]=state->board[3][0]=state->board[3][1]=state->board[3][2]=state->board[3][4]=state->board[3][6]=state->board[3][8]=state->board[3][10]=state->board[3][12]=state->board[3][14]=state->board[3][16]=state->board[3][17]=state->board[3][18]=state->board[4][0]=state->board[4][1]=state->board[4][3]=state->board[4][5]=state->board[4][7]=state->board[4][9]=state->board[4][11]=state->board[4][13]=state->board[4][15]=state->board[4][17]=state->board[4][18]=state->board[5][0]=state->board[5][2]=state->board[5][4]=state->board[5][6]=state->board[5][8]=state->board[5][10]=state->board[5][12]=state->board[5][14]=state->board[5][16]=state->board[5][18]=state->board[6][0]=state->board[6][1]=state->board[6][3]=state->board[6][5]=state->board[6][7]=state->board[6][9]=state->board[6][11]=state->board[6][13]=state->board[6][15]=state->board[6][17]=state->board[6][18]=state->board[7][0]=state->board[7][1]=state->board[7][2]=state->board[7][4]=state->board[7][6]=state->board[7][8]=state->board[7][10]=state->board[7][12]=state->board[7][14]=state->board[7][16]=state->board[7][17]=state->board[7][18]=state->board[8][0]=state->board[8][1]=state->board[8][2]=state->board[8][3]=state->board[8][5]=state->board[8][7]=state->board[8][9]=state->board[8][11]=state->board[8][13]=state->board[8][15]=state->board[8][16]=state->board[8][17]=state->board[8][18]=state->board[9][0]=state->board[9][1]=state->board[9][2]=state->board[9][3]=state->board[9][4]=state->board[9][6]=state->board[9][8]=state->board[9][10]=state->board[9][12]=state->board[9][14]=state->board[9][15]=state->board[9][16]=state->board[9][17]=state->board[9][18]=state->board[10][0]=state->board[10][1]=state->board[10][2]=state->board[10][3]=state->board[10][4]=state->board[10][5]=state->board[10][6]=state->board[10][7]=state->board[10][8]=state->board[10][9]=state->board[10][10]=state->board[10][11]=state->board[10][12]=state->board[10][13]=state->board[10][14]=state->board[10][15]=state->board[10][16]=state->board[10][17]=state->board[10][18]=INVALID;    
    
    state->next_player = BLACK;
    state->fallen_marbles[BLACK]=state->fallen_marbles[WHITE]=0;
    state->n_marbles_on_edge[BLACK]=state->n_marbles_on_edge[WHITE]=7;
    state->centrality[BLACK]=state->centrality[WHITE]=46;
    state->n_adjacent_pairs[BLACK]=state->n_adjacent_pairs[WHITE]=27;
    state->occupying_center[BLACK]=state->occupying_center[WHITE]=0;

}

bool is_terminal(state_t* state)
{
    return state->fallen_marbles[BLACK]==6 || state->fallen_marbles[WHITE]==6 ;
}

int get_winner(state_t* state)
{
    if(state->fallen_marbles[BLACK]==6)
        return WHITE;
    if(state->fallen_marbles[WHITE]==6)
        return BLACK;
    return INVALID;
}

inline void one_marble_moves(state_t* state, std::vector<move_t*> &moves, 
    int curr_player, int r, int c, int base_move_id)
{
    for(int i=0;i<6;i++)
    {
        Dir d = all_dirs[i];
        if(state->board[r+d.dr][c+d.dc]!=VOID)
            continue;
        move_t* move = new_move();
        move->move_id=i+base_move_id;

        add_delta_board(move, 0, r, c, curr_player, VOID);
        add_delta_board(move, 1, r+d.dr, c+d.dc, VOID, curr_player);

        fill_delta_stats(state, move);
        moves.push_back(move);
    }
}

inline void two_marbles_moves(state_t* state, std::vector<move_t*> &moves, 
    int curr_player, int adv_player, int r, int c, int d, int base_move_id)
{
    Dir D = all_dirs[2*d];
    //1
    if(state->board[r+2*D.dr][c+2*D.dc]==VOID)
    {
        move_t* move = new_move();
        move->move_id=base_move_id;

        add_delta_board(move, 0, r, c, curr_player, VOID);
        add_delta_board(move, 1, r+2*D.dr, c+2*D.dc, VOID, curr_player);

        fill_delta_stats(state, move);
        moves.push_back(move);
    }
    if(state->board[r+2*D.dr][c+2*D.dc]==adv_player)
    {
        int p = state->board[r+3*D.dr][c+3*D.dc];
        if(p==VOID || p==INVALID)
        {
            move_t* move = new_move();
            move->move_id=base_move_id;

            add_delta_board(move, 0, r, c, curr_player, VOID);
            add_delta_board(move, 1, r+2*D.dr, c+2*D.dc, adv_player, curr_player);
            if(p == VOID)
                add_delta_board(move, 2, r+3*D.dr, c+3*D.dc, VOID, adv_player);
            

            fill_delta_stats(state, move);
            moves.push_back(move);
        }
    }

    //2
    if(state->board[r-D.dr][c-D.dc]==VOID)
    {
        move_t* move = new_move();
        move->move_id=base_move_id+1;

        add_delta_board(move, 0, r+D.dr, c+D.dc, curr_player, VOID);
        add_delta_board(move, 1, r-D.dr, c-D.dc, VOID, curr_player);

        fill_delta_stats(state, move);
        moves.push_back(move);
    }
    if(state->board[r-D.dr][c-D.dc]==adv_player)
    {
        int p = state->board[r-2*D.dr][c-2*D.dc];
        if(p==VOID || p==INVALID)
        {
            move_t* move = new_move();
            move->move_id=base_move_id+1;

            add_delta_board(move, 0, r+D.dr, c+D.dc, curr_player, VOID);
            add_delta_board(move, 1, r-D.dr, c-D.dc, adv_player, curr_player);
            if(p == VOID)
                add_delta_board(move, 2, r-2*D.dr, c-2*D.dc, VOID, adv_player);

            fill_delta_stats(state, move);
            moves.push_back(move);
        }
    }
    int d1 = (2*d+2)%6;
    //3-6
    for(int i=2;i<6;i++)
    {
        Dir D1 = all_dirs[d1];
        bool ok = true;
        for(int j=0;j<2;j++)
            if(state->board[r+j*D.dr+D1.dr][c+j*D.dc+D1.dc]!=VOID)
                ok = false;
        if(ok)
        {
            move_t* move = new_move();
            move->move_id=base_move_id+i;

            for(int j=0;j<2;j++)
            {
                add_delta_board(move, 2*j, r+j*D.dr, c+j*D.dc, curr_player, VOID);
                add_delta_board(move, 2*j+1, r+j*D.dr+D1.dr, c+j*D.dc+D1.dc, VOID, curr_player);
            }

            fill_delta_stats(state, move);
            moves.push_back(move);
        }

        d1=(d1+1)%6;
    }
    // printf("After two marbles the size is %d\n", moves.size());
}

inline void three_marbles_moves(state_t* state, std::vector<move_t*> &moves, 
    int curr_player, int adv_player, int r, int c, int d, int base_move_id)
{
    Dir D = all_dirs[2*d];
    //1
    if(state->board[r+3*D.dr][c+3*D.dc]==VOID)
    {
        move_t* move = new_move();
        move->move_id=base_move_id;
        
        add_delta_board(move, 0, r, c, curr_player, VOID);
        add_delta_board(move, 1, r+3*D.dr, c+3*D.dc, VOID, curr_player);

        fill_delta_stats(state, move);
        moves.push_back(move);
    }
    if(state->board[r+3*D.dr][c+3*D.dc]==adv_player)
    {
        int p = state->board[r+4*D.dr][c+4*D.dc];
        if(p==VOID || p==INVALID)
        {
            move_t* move = new_move();
            move->move_id=base_move_id;

            add_delta_board(move, 0, r, c, curr_player, VOID);
            add_delta_board(move, 1, r+3*D.dr, c+3*D.dc, adv_player, curr_player);
            if(p == VOID)
                add_delta_board(move, 2, r+4*D.dr, c+4*D.dc, VOID, adv_player);

            fill_delta_stats(state, move);
            moves.push_back(move);
        }
        if( p == adv_player)
        {
            int q = state->board[r+5*D.dr][c+5*D.dc];
            {
                if(q==VOID || q==INVALID)
                {
                    move_t* move = new_move();
                    move->move_id=base_move_id;

                    add_delta_board(move, 0, r, c, curr_player, VOID);
                    add_delta_board(move, 1, r+3*D.dr, c+3*D.dc, adv_player, curr_player);
                    if(q == VOID)
                        add_delta_board(move, 2, r+5*D.dr, c+5*D.dc, VOID, adv_player);
                    
                    fill_delta_stats(state, move);
                    moves.push_back(move);
                }
            }
        }
    }

    //2
    if(state->board[r-D.dr][c-D.dc]==VOID)
    {
        move_t* move = new_move();
        move->move_id=base_move_id+1;
        
        add_delta_board(move, 0, r+2*D.dr, c+2*D.dc, curr_player, VOID);
        add_delta_board(move, 1, r-D.dr, c-D.dc, VOID, curr_player);

        fill_delta_stats(state, move);
        moves.push_back(move);
    }
    if(state->board[r-D.dr][c-D.dc]==adv_player)
    {
        int p = state->board[r-2*D.dr][c-2*D.dc];
        if(p==VOID || p==INVALID)
        {
            move_t* move = new_move();
            move->move_id=base_move_id+1;

            add_delta_board(move, 0, r+2*D.dr, c+2*D.dc, curr_player, VOID);
            add_delta_board(move, 1, r-D.dr, c-D.dc, adv_player, curr_player);
            if(p == VOID)
                add_delta_board(move, 2, r-2*D.dr, c-2*D.dc, VOID, adv_player);

            fill_delta_stats(state, move);
            moves.push_back(move);
        }
        if( p == adv_player)
        {
            int q = state->board[r-3*D.dr][c-3*D.dc]; 
            if(q==VOID || q==INVALID)
            {
                move_t* move = new_move();
                move->move_id=base_move_id+1;

                add_delta_board(move, 0, r+2*D.dr, c+2*D.dc, curr_player, VOID);
                add_delta_board(move, 1, r-D.dr, c-D.dc, adv_player, curr_player);
                if(q == VOID)
                    add_delta_board(move, 2, r-3*D.dr, c-3*D.dc, VOID, adv_player);
                    
                fill_delta_stats(state, move);
                moves.push_back(move);
            }
            
        }
    }

    int d1 = (2*d+2)%6;
    //3-6
    for(int i=2;i<6;i++)
    {
        Dir D1 = all_dirs[d1];
        bool ok = true;
        for(int j=0;j<3;j++)
            if(state->board[r+j*D.dr+D1.dr][c+j*D.dc+D1.dc]!=VOID)
                ok = false;
        if(ok)
        {
            move_t* move = new_move();
            move->move_id=base_move_id+i;

            for(int j=0;j<3;j++)
            {
                add_delta_board(move, 2*j, r+j*D.dr, c+j*D.dc, curr_player, VOID);
                add_delta_board(move, 2*j+1, r+j*D.dr+D1.dr, c+j*D.dc+D1.dc, VOID, curr_player);
            }

            fill_delta_stats(state, move);
            moves.push_back(move);
        }

        d1=(d1+1)%6;
    }
}

void possible_moves(state_t* state, std::vector<move_t*> &moves)
{
    moves.clear();
    int curr_player = state->next_player;
    int adv_player = curr_player==BLACK?WHITE:BLACK;
    for(int r=1;r<=10;r++)
    for(int c=1;c<=17;c++)
    {
        if(state->board[r][c]!= curr_player)
            continue;
        int base_move_id=((r-1)*17+c-1)*42;
        
        one_marble_moves(state, moves, curr_player, r, c, base_move_id);
        base_move_id+=6;
        for(int i=0;i<3;i++)
        {
            Dir d = all_dirs[2*i];
            if(state->board[r+d.dr][c+d.dc]!= curr_player)
            {
                base_move_id+=12;
                continue;
            }
            two_marbles_moves(state, moves, curr_player, adv_player, 
            r, c, i, base_move_id);
            base_move_id+=6;
            if(state->board[r+2*d.dr][c+2*d.dc]!= curr_player)
            {
                base_move_id+=6;
                continue;
            }
            three_marbles_moves(state, moves, curr_player, adv_player, 
            r, c, i, base_move_id);
            base_move_id+=6;
        }
    }
    
}


move_t get_random_move(state_t* state, std::uniform_real_distribution<double> &u_dist, std::default_random_engine &gen)
{
    int p = state->next_player;
    int m = -1;
    bool found = false;
    while(!found)
    {
        found = true;
        m = (int)(NUMBER_OF_POSSIBLE_MOVES*u_dist(gen));
        int C = ALL_MOVES[p][m].c_len;
        for(int cc=0;found && cc<C;cc++)
        {
            int r = ALL_MOVES[p][m].conditions[cc].r;
            int c = ALL_MOVES[p][m].conditions[cc].c;
            int color = ALL_MOVES[p][m].conditions[cc].color;
            if(state->board[r][c]!=color)
                found=false;
        }
    }
    move_t move;
    clean_move(&move);
    move.move_id = ALL_MOVES[p][m].move_id;

    int E = ALL_MOVES[p][m].e_len;
    for(int e=0;e<E;e++)
        add_delta_board(&move, e, ALL_MOVES[p][m].effects[e].r, ALL_MOVES[p][m].effects[e].c, ALL_MOVES[p][m].effects[e].from, ALL_MOVES[p][m].effects[e].to);
    //fill_delta_stats(state, &move);

    for(int i=0;i<E;i++)
    {
        int from = move.delta_board[i].from,
            to = move.delta_board[i].to;

        if(from==WHITE || from==BLACK)
            move.delta_fm[from]++;
        if(to==WHITE || to==BLACK)
            move.delta_fm[to]--;
    }
    return move;
}


inline int one_marble_moves_omp(state_t* state, move_t* moves, 
    int curr_player, int r, int c, int base_move_id, int index)
{
    for(int i=0;i<6;i++)
    {
        Dir d = all_dirs[i];
        if(state->board[r+d.dr][c+d.dc]!=VOID)
            continue;
        move_t* move = moves+index++;
        clean_move(move);
        move->move_id=i+base_move_id;

        add_delta_board(move, 0, r, c, curr_player, VOID);
        add_delta_board(move, 1, r+d.dr, c+d.dc, VOID, curr_player);

        fill_delta_stats(state, move);
    }
    return index;
}


inline int two_marbles_moves_omp(state_t* state, move_t* moves,  
    int curr_player, int adv_player, int r, int c, int d, int base_move_id, int index)
{
    Dir D = all_dirs[2*d];
    //1
    if(state->board[r+2*D.dr][c+2*D.dc]==VOID)
    {
        move_t* move = moves+index++;
        clean_move(move);
        move->move_id=base_move_id;

        add_delta_board(move, 0, r, c, curr_player, VOID);
        add_delta_board(move, 1, r+2*D.dr, c+2*D.dc, VOID, curr_player);

        fill_delta_stats(state, move);
    }
    if(state->board[r+2*D.dr][c+2*D.dc]==adv_player)
    {
        int p = state->board[r+3*D.dr][c+3*D.dc];
        if(p==VOID || p==INVALID)
        {
            move_t* move = moves+index++;
            clean_move(move);
            move->move_id=base_move_id;

            add_delta_board(move, 0, r, c, curr_player, VOID);
            add_delta_board(move, 1, r+2*D.dr, c+2*D.dc, adv_player, curr_player);
            if(p == VOID)
                add_delta_board(move, 2, r+3*D.dr, c+3*D.dc, VOID, adv_player);
            

            fill_delta_stats(state, move);
        }
    }

    //2
    if(state->board[r-D.dr][c-D.dc]==VOID)
    {
        move_t* move = moves+index++;
        clean_move(move);
        move->move_id=base_move_id+1;

        add_delta_board(move, 0, r+D.dr, c+D.dc, curr_player, VOID);
        add_delta_board(move, 1, r-D.dr, c-D.dc, VOID, curr_player);

        fill_delta_stats(state, move);
    }
    if(state->board[r-D.dr][c-D.dc]==adv_player)
    {
        int p = state->board[r-2*D.dr][c-2*D.dc];
        if(p==VOID || p==INVALID)
        {
            move_t* move = moves+index++;
            clean_move(move);
            move->move_id=base_move_id+1;

            add_delta_board(move, 0, r+D.dr, c+D.dc, curr_player, VOID);
            add_delta_board(move, 1, r-D.dr, c-D.dc, adv_player, curr_player);
            if(p == VOID)
                add_delta_board(move, 2, r-2*D.dr, c-2*D.dc, VOID, adv_player);

            fill_delta_stats(state, move);
        }
    }
    int d1 = (2*d+2)%6;
    //3-6
    for(int i=2;i<6;i++)
    {
        Dir D1 = all_dirs[d1];
        bool ok = true;
        for(int j=0;j<2;j++)
            if(state->board[r+j*D.dr+D1.dr][c+j*D.dc+D1.dc]!=VOID)
                ok = false;
        if(ok)
        {
            move_t* move = moves+index++;
            clean_move(move);
            move->move_id=base_move_id+i;

            for(int j=0;j<2;j++)
            {
                add_delta_board(move, 2*j, r+j*D.dr, c+j*D.dc, curr_player, VOID);
                add_delta_board(move, 2*j+1, r+j*D.dr+D1.dr, c+j*D.dc+D1.dc, VOID, curr_player);
            }

            fill_delta_stats(state, move);
        }

        d1=(d1+1)%6;
    }
    return index;
}

inline int three_marbles_moves_omp(state_t* state, move_t* moves,  
    int curr_player, int adv_player, int r, int c, int d, int base_move_id, int index)
{
    Dir D = all_dirs[2*d];
    //1
    if(state->board[r+3*D.dr][c+3*D.dc]==VOID)
    {
        move_t* move = moves+index++;
        clean_move(move);
        move->move_id=base_move_id;
        
        add_delta_board(move, 0, r, c, curr_player, VOID);
        add_delta_board(move, 1, r+3*D.dr, c+3*D.dc, VOID, curr_player);

        fill_delta_stats(state, move);
    }
    if(state->board[r+3*D.dr][c+3*D.dc]==adv_player)
    {
        int p = state->board[r+4*D.dr][c+4*D.dc];
        if(p==VOID || p==INVALID)
        {
            move_t* move = moves+index++;
            clean_move(move);
            move->move_id=base_move_id;

            add_delta_board(move, 0, r, c, curr_player, VOID);
            add_delta_board(move, 1, r+3*D.dr, c+3*D.dc, adv_player, curr_player);
            if(p == VOID)
                add_delta_board(move, 2, r+4*D.dr, c+4*D.dc, VOID, adv_player);

            fill_delta_stats(state, move);
        }
        if( p == adv_player)
        {
            int q = state->board[r+5*D.dr][c+5*D.dc];
            {
                if(q==VOID || q==INVALID)
                {
                    move_t* move = moves+index++;
                    clean_move(move);
                    move->move_id=base_move_id;

                    add_delta_board(move, 0, r, c, curr_player, VOID);
                    add_delta_board(move, 1, r+3*D.dr, c+3*D.dc, adv_player, curr_player);
                    if(q == VOID)
                        add_delta_board(move, 2, r+5*D.dr, c+5*D.dc, VOID, adv_player);
                    
                    fill_delta_stats(state, move);
                }
            }
        }
    }

    //2
    if(state->board[r-D.dr][c-D.dc]==VOID)
    {
        move_t* move = moves+index++;
        clean_move(move);
        move->move_id=base_move_id+1;
        
        add_delta_board(move, 0, r+2*D.dr, c+2*D.dc, curr_player, VOID);
        add_delta_board(move, 1, r-D.dr, c-D.dc, VOID, curr_player);

        fill_delta_stats(state, move);
    }
    if(state->board[r-D.dr][c-D.dc]==adv_player)
    {
        int p = state->board[r-2*D.dr][c-2*D.dc];
        if(p==VOID || p==INVALID)
        {
            move_t* move = moves+index++;
            clean_move(move);
            move->move_id=base_move_id+1;

            add_delta_board(move, 0, r+2*D.dr, c+2*D.dc, curr_player, VOID);
            add_delta_board(move, 1, r-D.dr, c-D.dc, adv_player, curr_player);
            if(p == VOID)
                add_delta_board(move, 2, r-2*D.dr, c-2*D.dc, VOID, adv_player);

            fill_delta_stats(state, move);
        }
        if( p == adv_player)
        {
            int q = state->board[r-3*D.dr][c-3*D.dc]; 
            if(q==VOID || q==INVALID)
            {
                move_t* move = moves+index++;
                clean_move(move);
                move->move_id=base_move_id+1;

                add_delta_board(move, 0, r+2*D.dr, c+2*D.dc, curr_player, VOID);
                add_delta_board(move, 1, r-D.dr, c-D.dc, adv_player, curr_player);
                if(q == VOID)
                    add_delta_board(move, 2, r-3*D.dr, c-3*D.dc, VOID, adv_player);
                    
                fill_delta_stats(state, move);
            }
            
        }
    }

    int d1 = (2*d+2)%6;
    //3-6
    for(int i=2;i<6;i++)
    {
        Dir D1 = all_dirs[d1];
        bool ok = true;
        for(int j=0;j<3;j++)
            if(state->board[r+j*D.dr+D1.dr][c+j*D.dc+D1.dc]!=VOID)
                ok = false;
        if(ok)
        {
            move_t* move = moves+index++;
            clean_move(move);
            move->move_id=base_move_id+i;

            for(int j=0;j<3;j++)
            {
                add_delta_board(move, 2*j, r+j*D.dr, c+j*D.dc, curr_player, VOID);
                add_delta_board(move, 2*j+1, r+j*D.dr+D1.dr, c+j*D.dc+D1.dc, VOID, curr_player);
            }

            fill_delta_stats(state, move);
        }

        d1=(d1+1)%6;
    }
    return index;
}

int possible_moves_omp(state_t* state, move_t* moves)
{
    int curr_player = state->next_player;
    int adv_player = curr_player==BLACK?WHITE:BLACK;
    int index = 0;
    for(int r=1;r<=10;r++)
    for(int c=1;c<=17;c++)
    {
        if(state->board[r][c]!= curr_player)
            continue;
        int base_move_id=((r-1)*17+c-1)*42;
        
        index = one_marble_moves_omp(state, moves, curr_player, r, c, base_move_id, index);
        base_move_id+=6;
        for(int i=0;i<3;i++)
        {
            Dir d = all_dirs[2*i];
            if(state->board[r+d.dr][c+d.dc]!= curr_player)
            {
                base_move_id+=12;
                continue;
            }
            index = two_marbles_moves_omp(state, moves, curr_player, adv_player, 
            r, c, i, base_move_id, index);
            base_move_id+=6;
            if(state->board[r+2*d.dr][c+2*d.dc]!= curr_player)
            {
                base_move_id+=6;
                continue;
            }
            index = three_marbles_moves_omp(state, moves, curr_player, adv_player, 
            r, c, i, base_move_id, index);
            base_move_id+=6;
        }
    }
    return index;
}


std::string stringify(state_t* state)
{
    std::string rst;
    if(state->next_player==BLACK)
        rst="BLACK\n";
    else
        rst="WHITE\n";
    rst+=std::to_string(state->fallen_marbles[BLACK])+" "+std::to_string(state->fallen_marbles[WHITE])+"\n";
    for(int i=1;i<10;i++)
    {
        for(int j=1;j<18;j++)
        {
            std::string sym;
            switch(state->board[i][j])
            {
                case BLACK:
                    sym=BLACK_SYMBOL;
                break;
                case WHITE:
                    sym=WHITE_SYMBOL;
                break;
                case VOID:
                    sym=VOID_SYMBOL;
                break;
                case INVALID:
                    sym=INVALID_SYMBOL;
                break;
            }
            rst+=sym;
        }
        rst+="\n";
    }
    return rst;
}

void clone_state(state_t* dest, state_t* source)
{
    memcpy(dest, source, sizeof(state_t));
}

void clone_move(move_t* dest, move_t* source)
{
    memcpy(dest, source, sizeof(move_t));
}


