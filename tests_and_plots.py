import numpy as np
import matplotlib.pyplot as plt

# Problem-Constrained Context

# Heuristic Minimax Agent - Open MP
D0 = [
    [5.247, 7.438, 10.721, 13.433, 15.054, 17.491, 16.579, 17.377, 34.681, 55.483],
    [2.377, 2.683, 4.045, 5.905, 6.091, 7.27, 6.206, 7.269, 13.763, 22.298],
    [2.128, 2.444, 3.41, 4.994, 5.521, 6.433, 5.645, 6.379, 12.05, 19.281],
    [1.643, 1.777, 2.4, 3.329, 4.034, 4.412, 3.832, 4.459, 7.884, 12.557],
    [1.274, 1.712, 2.284, 3.019, 3.674, 3.948, 3.399, 3.902, 7.056, 10.72],
    [1.158, 1.573, 2.074, 2.778, 3.368, 2.952, 3.067, 3.496, 6.063, 10.09],
    [1.089, 1.472, 1.937, 2.625, 2.639, 2.494, 2.838, 3.803, 9.636, 7.053],
    [1.026, 1.388, 1.815, 2.106, 2.282, 2.554, 3.531, 3.773, 5.001, 7.869]
]

D1 = [
    [5.247, 7.438, 10.721, 13.433, 15.054, 17.491, 16.579, 17.377, 34.681, 55.483],
    [3.837, 5.291, 7.083, 10.051, 10.008, 10.4, 13.696, 12.485, 13.733, 23.38],
    [2.316, 2.691, 4.148, 6.488, 5.721, 6.133, 8.209, 7.448, 8.125, 13.695],
    [1.807, 2.118, 2.95, 4.615, 4.623, 4.776, 5.886, 6.842, 5.861, 6.321],
    [1.575, 1.902, 2.727, 4.381, 4.178, 4.235, 5.262, 6.097, 5.223, 5.422],
    [1.26, 1.68, 2.249, 3.984, 3.181, 3.252, 3.986, 4.08, 6.556, 8.855],
    [1.135, 1.505, 2.047, 2.973, 2.821, 2.784, 3.483, 4.148, 3.997, 4.125],
    [1.086, 1.422, 1.924, 2.792, 2.607, 2.571, 3.353, 4.399, 3.858, 4.029]
]

D2 = [
    [8.34, 8.311, 8.269, 8.336, 8.35, 8.291, 8.254, 8.286, 8.245, 8.171],
    [4.057, 4.132, 4.098, 4.132, 4.069, 4.131, 4.113, 4.104, 4.047, 4.11],
    [2.74, 2.899, 2.82, 2.875, 2.798, 2.78, 2.818, 2.766, 2.768, 2.766],
    [2.074, 2.136, 2.16, 2.147, 2.143, 2.131, 2.182, 2.149, 2.123, 2.115],
    [1.702, 1.764, 1.724, 1.743, 1.748, 1.701, 1.751, 1.724, 1.755, 1.729],
    [1.429, 1.443, 1.462, 1.43, 1.46, 1.471, 1.445, 1.461, 1.467, 1.457],
    [1.234, 1.236, 1.266, 1.277, 1.274, 1.259, 1.251, 1.263, 1.258, 1.276],
    [1.076, 1.1, 1.112, 1.109, 1.1, 1.138, 1.114, 1.136, 1.125, 1.137]

]

for D, title, filename in [(D0, 'Heuristic Minimax Agent - Open MP Implementation', 'Problem_Constrained_HMM_OMP'),
                           (D1, 'Heuristic Minimax Agent - MPI Implementation', 'Problem_Constrained_HMM_MPI'),
                           (D2, 'Monte Carlo Tree Search Agent - Open MP Implementation', 'Problem_Constrained_MCTS')]:
    fig, axs = plt.subplots(1, 2)
    fig.tight_layout(rect=[0, 0.03, 1, 0.95])
    fig.set_size_inches(10, 6)

    M = np.array(D).mean(axis=1)
    fig.suptitle(title, fontsize=16)

    ax = axs[0]
    ax.set_title('Average Speedup v # Threads')
    ax.set_xlabel('# Threads')
    ax.set_ylabel('Speedup')
    ax.plot(list(range(1, 9)), M[0]*(1/M))

    ax = axs[1]
    ax.set_title('Average Execution Time v # Threads')
    ax.set_xlabel('# Threads')
    ax.set_ylabel('Speedup')
    ax.plot(list(range(1, 9)), M)

    fig.savefig(filename+".png", dpi=300)
    plt.close(fig)

    print(filename)
    print('\tSpeedup:', list(M[0]*(1/M)))
    print('\tRun Times:', list(M))

D0 = [
    [6, 6, 6, 6, 5, 5, 5, 5, 5, 5],
    [7, 7, 6, 6, 6, 6, 6, 6, 6, 6],
    [7, 7, 7, 6, 6, 6, 6, 6, 6, 6],
    [7, 7, 7, 7, 6, 7, 6, 6, 6, 6],
    [7, 7, 7, 7, 7, 6, 7, 6, 6, 6],
    [8, 7, 7, 7, 7, 7, 6, 6, 6, 6],
    [8, 7, 7, 7, 7, 7, 7, 6, 6, 6],
    [8, 7, 7, 7, 7, 7, 7, 7, 6, 6]
]

D1 = [
    [6, 6, 6, 6, 5, 5, 5, 5, 5, 5],
    [6, 6, 6, 6, 6, 6, 6, 6, 6, 6],
    [7, 7, 7, 7, 6, 6, 6, 6, 6, 6],
    [7, 7, 7, 7, 7, 7, 6, 6, 6, 6],
    [8, 8, 7, 7, 7, 7, 7, 7, 6, 7],
    [8, 8, 7, 7, 7, 7, 7, 7, 6, 7],
    [8, 8, 7, 7, 7, 7, 7, 7, 7, 6],
    [8, 8, 7, 7, 7, 7, 7, 7, 7, 7]
]

D2 = [
    [602, 610, 619, 607, 602, 604, 632, 624, 614, 632],#1
    [1213, 1181, 1220, 1214, 1203, 1228, 1232, 1214, 1202, 1214],#2
    [1787, 1789, 1755, 1764, 1782, 1769, 1763, 1802, 1799, 1788],#3
    [2404, 2344, 2432, 2368, 2361, 2389, 2388, 2373, 2361, 2337],#4
    [2910, 2874, 2874, 2834, 2888, 2886, 2780, 2796, 2835, 2818],#5
    [3561, 3505, 3506, 3290, 3218, 3238, 3262, 3268, 3290, 3233],#6
    [4112, 4060, 4061, 4043, 3765, 3652, 3691, 3670, 3660, 3693],#7
    [4677, 4457, 3987, 3992, 4000, 4041, 4010, 4307, 4657, 4780]#8
]

for D, title, filename, term_1, term_2 in [
 (D0, 'Heuristic Minimax Agent - Open MP Implementation', 'Time_Constrained_HMM_OMP', 'Depth Increase', 'Depth Explored'),
 (D1, 'Heuristic Minimax Agent - MPI Implementation', 'Time_Constrained_HMM_MPI', 'Depth Increase', 'Depth Explored'),
 (D2, 'MCTS Agent - Open MP Implementation', 'Time_Constrained_MCTS', 'Rollouts Increase', 'Rollouts Simulated')]:
    fig, axs = plt.subplots(1, 2)
    fig.tight_layout(rect=[0, 0.03, 1, 0.95])
    fig.set_size_inches(10, 6)

    M = np.array(D).mean(axis=1)
    fig.suptitle(title, fontsize=16)

    ax = axs[0]
    ax.set_title('Average '+term_1+' v # Threads')
    ax.set_xlabel('# Threads')
    ax.set_ylabel('Average '+term_1)
    ax.plot(list(range(1, 9)), M-M[0])

    ax = axs[1]
    ax.set_title('Average '+term_2+' v # Threads')
    ax.set_xlabel('# Threads')
    ax.set_ylabel('Average '+term_2)
    ax.plot(list(range(1, 9)), M)
    ax.set_ylim(0, ax.get_ylim()[1]+0.5)

    fig.savefig(filename+".png", dpi=300)
    plt.close(fig)

    print(filename)
    print('\t'+term_1+':', list(M-M[0]))
    print('\t'+term_2+':', list(M))
