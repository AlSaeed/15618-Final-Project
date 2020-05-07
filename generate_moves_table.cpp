/*
 * This file is run independently to generate the all_moves.h file,
 * which lists all possible moves in both thier condition and effect.
 * This is used in MCTSPlayer for fast random move sampling.
 */
#define BLACK 0
#define WHITE 1
#define VOID 2
#define INVALID -1
#include <iostream>
#include <cstring>

using namespace std;

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

typedef struct {
    int dr;
    int dc;
} Dir;
const Dir all_dirs[6]={{0,2},{0,-2},{1,1},{-1,-1},{-1,1},{1,-1}};

typedef struct {
    int r,c, color;
} condition;

typedef struct {
    int r,c, from, to;
} delta;

typedef struct {
    int move_id;
    int c_len;
    int e_len;
    condition conditions[6];
    delta effects[6];
} move;

inline void push_condition(move& m, int r, int c, int color)
{
    m.conditions[m.c_len].r=r;
    m.conditions[m.c_len].c=c;
    m.conditions[m.c_len].color=color;
    m.c_len++;
}

inline void pop_condition(move& m)
{
    m.c_len--;
}

inline void push_effect(move& m, int r, int c, int from, int to)
{
    m.effects[m.e_len].r=r;
    m.effects[m.e_len].c=c;
    m.effects[m.e_len].from=from;
    m.effects[m.e_len].to=to;
    m.e_len++;
}

inline void pop_effect(move& m)
{
    m.e_len--;
}

inline void clone_move(move* to, move* from)
{
    memcpy(to, from, sizeof(move));
}

move moves[10000];
int move_index=0;

inline void add_move(move &m)
{
    clone_move(&moves[move_index++], &m);
}

int curr_player = BLACK;
int adv_player = WHITE;


inline void one_marble_moves(move &m, int r, int c, int base_move_id)
{
    for(int i=0;i<6;i++)
    {
        Dir d = all_dirs[i];
        if(centrality_matrix[r+d.dr][c+d.dc]==INVALID)
            continue;
        push_condition(m, r+d.dr, c+d.dc, VOID);
        m.move_id=i+base_move_id;

        push_effect(m, r, c, curr_player, VOID);
        push_effect(m, r+d.dr, c+d.dc, VOID, curr_player);

        add_move(m);

        pop_effect(m);
        pop_effect(m);
        pop_condition(m);
    }
}

inline void two_marbles_moves(move &m, int r, int c, int d, int base_move_id)
{
    Dir D = all_dirs[2*d];
    //1
    if(centrality_matrix[r+2*D.dr][c+2*D.dc]==INVALID);
    else
    {
        push_condition(m, r+2*D.dr, c+2*D.dc, VOID);
        m.move_id=base_move_id;

        push_effect(m, r, c, curr_player, VOID);
        push_effect(m, r+2*D.dr, c+2*D.dc, VOID, curr_player);

        add_move(m);

        pop_effect(m);
        pop_effect(m);

        pop_condition(m);

    }
    if(centrality_matrix[r+2*D.dr][c+2*D.dc]==INVALID);
    else
    {
        push_condition(m, r+2*D.dr, c+2*D.dc, adv_player);
        int p = centrality_matrix[r+3*D.dr][c+3*D.dc];

        
        push_effect(m, r, c, curr_player, VOID);
        push_effect(m, r+2*D.dr, c+2*D.dc, adv_player, curr_player);
        if(centrality_matrix[r+3*D.dr][c+3*D.dc] != INVALID)
        {
            push_condition(m, r+3*D.dr, c+3*D.dc, VOID);
            push_effect(m, r+3*D.dr, c+3*D.dc, VOID, adv_player);
            add_move(m);
            pop_condition(m);
            pop_effect(m);
        }
        else
            add_move(m);

        pop_effect(m);
        pop_effect(m);
        pop_condition(m); 
    }

    //2
    if(centrality_matrix[r-D.dr][c-D.dc]==INVALID);
    else
    {
        push_condition(m, r-D.dr, c-D.dc, VOID);
        m.move_id=base_move_id+1;

        push_effect(m, r+D.dr, c+D.dc, curr_player, VOID);
        push_effect(m, r-D.dr, c-D.dc, VOID, curr_player);

        add_move(m);

        pop_effect(m);
        pop_effect(m);
        pop_condition(m);

    }
    if(centrality_matrix[r-D.dr][c-D.dc]==INVALID);
    else
    {
        push_condition(m, r-D.dr, c-D.dc, adv_player);
        m.move_id=base_move_id+1;

        push_effect(m, r+D.dr, c+D.dc, curr_player, VOID);
        push_effect(m, r-D.dr, c-D.dc, adv_player, curr_player);
        if(centrality_matrix[r-2*D.dr][c-2*D.dc] == INVALID)
        {
            add_move(m);
        }
        else
        {
            push_condition(m, r-2*D.dr, c-2*D.dc, VOID);
            push_effect(m, r-2*D.dr, c-2*D.dc, VOID, adv_player);
            add_move(m);
            pop_condition(m);
            pop_effect(m);
        }
        pop_condition(m);
        pop_effect(m);
        pop_effect(m);
    }
    int d1 = (2*d+2)%6;
    //3-6
    for(int i=2;i<6;i++)
    {
        Dir D1 = all_dirs[d1];
        bool ok = true;
        for(int j=0;j<2;j++)
            if(centrality_matrix[r+j*D.dr+D1.dr][c+j*D.dc+D1.dc]==INVALID)
                ok = false;
        if(ok)
        {
            m.move_id=base_move_id+i;                

            for(int j=0;j<2;j++)
            {
                push_condition(m, r+j*D.dr+D1.dr, c+j*D.dc+D1.dc, VOID);
                push_effect(m, r+j*D.dr, c+j*D.dc, curr_player, VOID);
                push_effect(m, r+j*D.dr+D1.dr, c+j*D.dc+D1.dc, VOID, curr_player);
            }
            add_move(m);
            for(int j=0;j<2;j++)
            {
                pop_condition(m);
                pop_effect(m);
                pop_effect(m);
            }


        }

        d1=(d1+1)%6;
    }
}

inline void three_marbles_moves(move &m, int r, int c, int d, int base_move_id)
{
    Dir D = all_dirs[2*d];
    //1
    if(centrality_matrix[r+3*D.dr][c+3*D.dc]==INVALID);
    else
    {
        push_condition(m, r+3*D.dr, c+3*D.dc, VOID);
        m.move_id=base_move_id;
        
        push_effect(m, r, c, curr_player, VOID);
        push_effect(m, r+3*D.dr, c+3*D.dc, VOID, curr_player);

        add_move(m);
        pop_effect(m);
        pop_effect(m);
        pop_condition(m);
    }
    if(centrality_matrix[r+3*D.dr][c+3*D.dc]==INVALID);
    else
    {
        push_condition(m, r+3*D.dr, c+3*D.dc, adv_player);
        if(centrality_matrix[r+4*D.dr][c+4*D.dc]==INVALID)
        {
            m.move_id=base_move_id;

            push_effect(m, r, c, curr_player, VOID);
            push_effect(m, r+3*D.dr, c+3*D.dc, adv_player, curr_player);
            add_move(m);
            pop_effect(m);
            pop_effect(m);
        }
        else
        {
            
            push_condition(m, r+4*D.dr, c+4*D.dc, VOID);
            m.move_id=base_move_id;

            push_effect(m, r, c, curr_player, VOID);
            push_effect(m, r+3*D.dr, c+3*D.dc, adv_player, curr_player);
            push_effect(m, r+4*D.dr, c+4*D.dc, VOID, adv_player);

            add_move(m);
            pop_effect(m);pop_effect(m);pop_effect(m);
            pop_condition(m);

            
            push_condition(m, r+4*D.dr, c+4*D.dc, adv_player);
            if(centrality_matrix[r+5*D.dr][c+5*D.dc]==INVALID)
            {
                m.move_id=base_move_id;

                push_effect(m, r, c, curr_player, VOID);
                push_effect(m, r+3*D.dr, c+3*D.dc, adv_player, curr_player);

                add_move(m);

                pop_effect(m);
                pop_effect(m);

            }
            else
            {
                push_condition(m, r+5*D.dr, c+5*D.dc, VOID);
                m.move_id=base_move_id;
                push_effect(m, r, c, curr_player, VOID);
                push_effect(m, r+3*D.dr, c+3*D.dc, adv_player, curr_player);
                push_effect(m, r+5*D.dr, c+5*D.dc, VOID, adv_player);

                add_move(m);
                pop_effect(m);pop_effect(m);pop_effect(m);
                pop_condition(m);

            }
            pop_condition(m);
        }
        pop_condition(m);
    }

    //2
    if(centrality_matrix[r-D.dr][c-D.dc]==INVALID);
    else
    {
        push_condition(m, r-D.dr, c-D.dc, VOID);
        m.move_id=base_move_id+1;
        push_effect(m, r+2*D.dr, c+2*D.dc, curr_player, VOID);
        push_effect(m, r-D.dr, c-D.dc, VOID, curr_player);
        add_move(m);
        pop_effect(m);pop_effect(m);
        pop_condition(m);
    }
    if(centrality_matrix[r-D.dr][c-D.dc]==INVALID);
    {
        push_condition(m, r-D.dr, c-D.dc, adv_player);
        if(centrality_matrix[r-2*D.dr][c-2*D.dc]==INVALID)
        {
            m.move_id=base_move_id+1;

            push_effect(m, r+2*D.dr, c+2*D.dc, curr_player, VOID);
            push_effect(m, r-D.dr, c-D.dc, adv_player, curr_player);
            add_move(m);
            pop_effect(m);pop_effect(m);

        }
        else
        {
            push_condition(m, r-2*D.dr, c-2*D.dc, VOID);
            m.move_id=base_move_id+1;

            push_effect(m, r+2*D.dr, c+2*D.dc, curr_player, VOID);
            push_effect(m, r-D.dr, c-D.dc, adv_player, curr_player);
            push_effect(m, r-2*D.dr, c-2*D.dc, VOID, adv_player);
            
            add_move(m);
            pop_effect(m);pop_effect(m);pop_effect(m);
            pop_condition(m);

            
            push_condition(m, r-2*D.dr, c-2*D.dc, adv_player);
            if(centrality_matrix[r-3*D.dr][c-3*D.dc]==INVALID)
            {
                m.move_id=base_move_id+1;

                push_effect(m,r+2*D.dr, c+2*D.dc, curr_player, VOID);
                push_effect(m,r-D.dr, c-D.dc, adv_player, curr_player);
                add_move(m);
                pop_effect(m);pop_effect(m);
            }
            else
            {
                push_condition(m, r-3*D.dr, c-3*D.dc, VOID);
                m.move_id=base_move_id+1;

                push_effect(m, r+2*D.dr, c+2*D.dc, curr_player, VOID);
                push_effect(m, r-D.dr, c-D.dc, adv_player, curr_player);
                push_effect(m, r-3*D.dr, c-3*D.dc, VOID, adv_player);
                
                add_move(m);
                pop_effect(m);pop_effect(m);pop_effect(m);
                pop_condition(m);

            }
                
            pop_condition(m);
        }
        pop_condition(m);
    }

    int d1 = (2*d+2)%6;
    //3-6
    for(int i=2;i<6;i++)
    {
        Dir D1 = all_dirs[d1];
        bool ok = true;
        for(int j=0;j<3;j++)
            if(centrality_matrix[r+j*D.dr+D1.dr][c+j*D.dc+D1.dc]==INVALID)
                ok = false;
        if(ok)
        {
            m.move_id=base_move_id+i;

            for(int j=0;j<3;j++)
            {
                push_condition(m, r+j*D.dr+D1.dr, c+j*D.dc+D1.dc, VOID);
                push_effect(m, r+j*D.dr, c+j*D.dc, curr_player, VOID);
                push_effect(m, r+j*D.dr+D1.dr, c+j*D.dc+D1.dc, VOID, curr_player);
            }
            add_move(m);
            for(int j=0;j<3;j++)
            {
                pop_condition(m);
                pop_effect(m);
                pop_effect(m);
            }

        }

        d1=(d1+1)%6;
    }
}

void possible_moves()
{
    move m;
    m.c_len=m.e_len=0;
    for(int r=1;r<=10;r++)
    for(int c=1;c<=17;c++)
    {
        if(centrality_matrix[r][c]== INVALID)
            continue;

        push_condition(m, r, c, curr_player);
        
        int base_move_id=((r-1)*17+c-1)*42;
        
        one_marble_moves(m, r, c, base_move_id);
        base_move_id+=6;
        for(int i=0;i<3;i++)
        {
            Dir d = all_dirs[2*i];
            if(centrality_matrix[r+d.dr][c+d.dc]== INVALID)
            {
                base_move_id+=12;
            }
            else
            {
                push_condition(m, r+d.dr, c+d.dc, curr_player);
                two_marbles_moves(m, r, c, i, base_move_id);
                base_move_id+=6;
                if(centrality_matrix[r+2*d.dr][c+2*d.dc]==INVALID)
                {
                    base_move_id+=6;
                }
                else
                {
                    push_condition(m, r+2*d.dr, c+2*d.dc, curr_player);
                    three_marbles_moves(m, r, c, i, base_move_id);
                    base_move_id+=6;
                    pop_condition(m);
                }
                pop_condition(m);
            }
        }
        pop_condition(m);
    }
    
}

inline void print_list()
{
    cout << "{";
    for(int m=0;m<move_index;m++)
    {
        cout << endl << "{";
        cout << moves[m].move_id<<", "<< moves[m].c_len<<", "<< moves[m].e_len<<", {";
        for(int i=0;i<moves[m].c_len;i++)
        {
            cout <<"{" << moves[m].conditions[i].r<<", "<< moves[m].conditions[i].c<<", "<< moves[m].conditions[i].color<<"}, ";
        }
        for(int i=moves[m].c_len;i<6;i++)
        {
            cout <<"{-1, -1, -1}, ";
        }
        cout <<"}, {";
        for(int i=0;i<moves[m].e_len;i++)
        {
            cout <<"{" << moves[m].effects[i].r<<", "<< moves[m].effects[i].c<<", "<< moves[m].effects[i].from<<", "<< moves[m].effects[i].to<<"}, ";
        }
        for(int i=moves[m].e_len;i<6;i++)
        {
            cout <<"{-1, -1, -1, -1}, ";
        }
        cout <<"}},";

    }
    cout << "\n},";
}


int main()
{
    possible_moves();

    cout << "const mov_t ALL_MOVES[2]["<<move_index<<"]={\n";
    print_list();
    for(int m=0;m<move_index;m++)
    {
        for(int i=0;i<6;i++)
        {
            if(moves[m].conditions[i].color==WHITE)
                moves[m].conditions[i].color=BLACK;
            else if(moves[m].conditions[i].color==BLACK)
                moves[m].conditions[i].color=WHITE;

            if(moves[m].effects[i].from==WHITE)
                moves[m].effects[i].from=BLACK;
            else if(moves[m].effects[i].from==BLACK)
                moves[m].effects[i].from=WHITE;

            if(moves[m].effects[i].to==WHITE)
                moves[m].effects[i].to=BLACK;
            else if(moves[m].effects[i].to==BLACK)
                moves[m].effects[i].to=WHITE;
        }
    }
    cout << "\n";
    print_list();
    cout << "\n};";
    return 0;
}