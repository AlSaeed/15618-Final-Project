#include "player.h"

#if MPI
/* Called by master thread to assign a move to a thread*/
int assign_one_move(std::vector<edge_t> &edges, int process_id, double* result_buffer, MPI_Request* send_req, MPI_Request* recv_req){
    int move_id;
    //if no pending moves, send -1
    if(edges.size() == 0){
        move_id = -1;
        MPI_Isend(&move_id, 1, MPI_INT, process_id, 0, MPI_COMM_WORLD, send_req);
        return -1;
    }else{
        move_id = edges[0].move->move_id;
        edges.erase(edges.begin());
        MPI_Isend(&move_id, 1, MPI_INT, process_id, 0, MPI_COMM_WORLD, send_req);
        MPI_Irecv(result_buffer, 2, MPI_DOUBLE, process_id, 0, MPI_COMM_WORLD, recv_req);
        return move_id;
    }
}
/*Called by other threads to receive one move from the master thread*/
int receive_one_move(){
    int move_id;
    MPI_Recv(&move_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return move_id;
}

/* Called by the master thread. Check if has received the result from one particular process, if so, send it another move to finish*/
bool receive_one_finished_result(std::vector<edge_t> &edges, int process_id, 
double* result_buffer, return_t* result, MPI_Request* send_req,  MPI_Request* recv_req, int* process_finished){
    int finished;
    MPI_Status status;
    MPI_Test(recv_req, &finished, &status);
    if(finished){
        double val = *result_buffer;
        int compute_interrupted = *(result_buffer+1) ? 1:0;
        result->value = val;
        //borrow move_id to store the compute_interrupted
        result->move_id = compute_interrupted;
        *process_finished = assign_one_move(edges, process_id, result_buffer, send_req, recv_req);
        return true;
    }else{
        return false;
    }
}

/* Called by other threads. Send back result to master thread*/
void send_one_finished_result(double value, bool compute_interrupted){
    MPI_Request send_req;
    double result[2]; 
    result[0] = value;
    result[1] = compute_interrupted ? 1:0;
    MPI_Isend(result, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &send_req);
}
/* Called by the master thread. Send updated alpha to all other threads*/
void update_alpha(double alpha, int process_count){
    for(int i=1; i<process_count; i++){
        MPI_Request request;
        MPI_Isend(&alpha, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &request);
    }
}

/* Called by other threads. Receive updated alpha from master thread*/
void receive_updated_alpha(double* alpha, MPI_Request* req){
    int finished;
    MPI_Status status;
    MPI_Test(req, &finished, &status);
    if(finished){
        MPI_Irecv(alpha, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, req);
    }
}

/* Called by master thread. Master thread send final move_id to all other threads*/
void send_final_result(MPI_Request *send_reqs, int process_count, int move_id){
    for(int i=1; i<process_count; i++){
        MPI_Isend(&move_id, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &send_reqs[i]);
    }
    for(int i=1; i<process_count; i++){
        MPI_Wait(&send_reqs[i], MPI_STATUS_IGNORE);
    }
}

/* Called by other threads. Recevie final result of next_move from master thread*/
int receive_final_result(){
    int move_id;
    int process_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Recv(&move_id, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // printf("[Process %d] Receive final move_id %d\n", process_id, move_id);
    return move_id;
}
#endif