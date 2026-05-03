#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define N 9

struct Coords{
	int x;
	int y;
};

unsigned char* read_sudoku(char* filename) {
    FILE* input = fopen(filename, "r");

    unsigned char* data = (unsigned char *) malloc(N * N);
    int count = 0;

    while (count < N * N) {
        int c = fgetc(input);
        if (c == EOF) break;

        if (c == 'x' || (c >= '0' && c <= '9')) {
            data[count++] = (unsigned char)c;
        }
    }

    fclose(input);
    return data;
}

double get_local_energy(unsigned char* board, int x, int y) {
	char current = board[N * y + x];
	double energy = 0;

	if(current == 'x') {
		return energy;
	}

	for(int i = 0; i < N; i++) {
		if(x != i && board[N * y + i] == current) {
			energy += 1;
		}

		if (y != i && board[N * i + x] == current) {
			energy += 1;
		}
	}

	int start_x = (x / 3) * 3;
	int start_y = (y / 3) * 3;

	for(int i = start_y; i < start_y + 3; i++) {
		for(int j = start_x; j < start_x + 3; j++) {
			if(i != y && j != x && board[N * i + j] == current) {
				energy += 1;
			}
		}
	}

	return energy;
}

double get_full_energy(unsigned char* board) {
	double energy = 0;

	for(int y = 0; y < N; y++) {
		for(int x = 0; x < N; x++) {
			energy += get_local_energy(board, x, y);
		}
	}

	return energy;
}

int count_empty_spaces(unsigned char* board) {
	int count = 0;

	for(int y = 0; y < N; y++) {
		for(int x = 0; x < N; x++) {
			if(board[N * y + x] == 'x') {
				count += 1;
			}
		}
	}

	return count;
}

struct Coords* get_empty_spaces(unsigned char* board, int empty_count) {
	struct Coords* data = malloc(empty_count * sizeof(struct Coords));

	int id = 0;
	for(int y = 0; y < N; y++) {
		for(int x = 0; x < N; x++) {
			if(board[N * y + x] == 'x') {
				struct Coords point = {x,y};
				data[id] = point;
				id++;
			}
		}
	}

	return data;
}

void prefill_empty(unsigned char* board, struct Coords* empty_spaces, int empty_count) {
	for(int i = 0; i < empty_count; i++) {
		int x = empty_spaces[i].x;
		int y = empty_spaces[i].y;

		board[N * y + x] = (rand() % 9) + 1 + '0';
	}
}

void print_board(unsigned char* board) {
	for(int y = 0; y < N; y++) {
		for(int x = 0; x < N; x++) {
			printf("%c",board[N * y + x]);
		}
		printf("\n");
	}
}

void simulated_annealing(unsigned char* board, double temp, double step_lower, char* out_dir) {
	int empty_count = count_empty_spaces(board);
	printf("%d\n",empty_count);
	struct Coords* empty_spaces = get_empty_spaces(board, empty_count);

	prefill_empty(board, empty_spaces, empty_count);

	double current_energy = get_full_energy(board);
	int count_iter = 0;

	char energy_path[512];

	sprintf(energy_path, "%s/energy_log.csv", out_dir);
	FILE* f_energy = fopen(energy_path, "w");
	fprintf(f_energy, "iteration,energy\n");

	while(temp > 1e-2 && current_energy != 0 && count_iter < 6000000) {
		if(count_iter % 1000 == 0) {
			fprintf(f_energy, "%d,%f\n", count_iter, current_energy);
		}

		int e_id = rand() % empty_count;
		int n_x = empty_spaces[e_id].x;
		int n_y = empty_spaces[e_id].y;

		char new_val;
		do {
			new_val = (rand() % 9) + 1 + '0';
		}while (new_val == board[N * n_y + n_x]);

		int n_id = N * n_y + n_x;

		double old_energy = get_local_energy(board, n_x, n_y);
		
		char old_val = board[n_id];
		board[n_id] = new_val;

		double new_energy = get_local_energy(board, n_x, n_y);

		double delta = new_energy - old_energy;

		if(delta < 0 || ((double) rand() / RAND_MAX) < exp((-delta * 2) / temp)) {
			current_energy += (delta * 2);
		} else {
			board[n_id] = old_val;
		}

		temp *= step_lower;
		count_iter++;
	}
	fprintf(f_energy, "%d,%f\n", count_iter, current_energy);
	fclose(f_energy);
	free(empty_spaces);
}

int main(int argc, char* argv[]) {
    	char* input_file = argv[1];
	char* out_dir = argv[2];
	double temp = atof(argv[3]);
    	double temp_lower = atof(argv[4]);

	unsigned char* board = read_sudoku(input_file);
	int initial_empty = count_empty_spaces(board);

	simulated_annealing(board, temp, temp_lower, out_dir);

	char board_path[256];
    	sprintf(board_path, "%s/final_board.txt", out_dir);
    	FILE* f_board = fopen(board_path, "w");
    	for(int y = 0; y < N; y++) {
		for(int x = 0; x < N; x++) {
	    		fprintf(f_board, "%c", board[N * y + x]);
		}
		fprintf(f_board, "\n");
    	}
    	fclose(f_board);

	char stats_path[256];
	sprintf(stats_path, "%s/stats.txt", out_dir);
	FILE* f_stats = fopen(stats_path, "w");
	fprintf(f_stats, "empty_spaces: %d\n", initial_empty);
	fprintf(f_stats, "final_energy: %f\n", get_full_energy(board));
	fclose(f_stats);

	printf("%f\n\n",get_full_energy(board));
	free(board);
}
