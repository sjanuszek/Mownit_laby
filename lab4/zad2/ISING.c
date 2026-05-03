#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef double (*CoolingFunc)(double,double,double,int);
typedef double (*EnergyFunc)(unsigned char*,int,int,int,int,int);

// neighbor is determined by the offset from the original point 
typedef struct {
    int dx, dy;
} Neighbor;

// points neighborhood is defined as an array of neighbors thats is an array of offsets
typedef struct {
    Neighbor* offsets;
    int count;
} Neighborhood;

// read input pgm file
unsigned char* read_pgm(char* filename, int* n, int* m) {
	FILE* input = fopen(filename,"rb");

	char magic[3];
	int max_val;

	fscanf(input, "%s\n%d %d\n%d\n", magic, n, m, &max_val);

	int size = (*n) * (*m);
	unsigned char* data = (unsigned char*) malloc(size);

	fread(data, 1, size, input);

	fclose(input);
	return data;
}

// write bitmap to a pgm file
void write_pgm(const char* filename, unsigned char* data, int width, int height) {
    FILE* output = fopen(filename, "wb");

    fprintf(output, "P5\n%d %d\n255\n", width, height);

    size_t written = fwrite(data, sizeof(unsigned char), width * height, output);

    fclose(output);
}
// this section contains all the different neighborhoods
// all the fixed number neighborhoods could be defined as static consts but here they're dynamically created
// as those neighborhoods are created only once this basically has no effect on performance, but is more convenient to write

// LEGEND:
// O - center point
// X - neighbor

/* SHAPE:
 *
 *  X
 * XOX
 *  X
 *
 */
Neighborhood get_4_neighbor_plus() {
	Neighborhood n;
	n.count = 4;

	n.offsets = malloc(4 * sizeof(Neighbor));

	int dx[] = {-1,0,1,0};
	int dy[] = {0,1,0,-1};

	for(int i = 0; i < 4; i++) {
		n.offsets[i] = (Neighbor){dx[i],dy[i]};
	}

	return n;
}

/* SHAPE:
 *
 * X X
 *  O
 * X X
 *
 */
Neighborhood get_4_neighbor_x() {
	Neighborhood n;
	n.count = 4;
	n.offsets = malloc(4 * sizeof(Neighbor));

	int dx[] = {-1,1,1,-1};
	int dy[] = {-1,-1,1,1};

	for(int i = 0; i < 4; i++) {
		n.offsets[i] = (Neighbor){dx[i],dy[i]};
	}

	return n;
}

/* SHAPE:
 *
 * XXX
 * XOX
 * XXX
 *
 */
Neighborhood get_8_neighbor_square() {
	Neighborhood n;
	n.count = 8;
	n.offsets = malloc(8 * sizeof(Neighbor));

	int dx[] = {-1,1,1,-1,-1,0,1,0};
	int dy[] = {-1,-1,1,1,0,1,0,-1};

	for(int i = 0; i < 8; i++) {
		n.offsets[i] = (Neighbor){dx[i],dy[i]};
	}

	return n;
}

/* SHAPE:
 *
 *   X
 *   X
 * XXOXX
 *   X
 *   X
 *
 */
Neighborhood get_8_neighbor_plus() {
	Neighborhood n;
	n.count = 8;
	n.offsets = malloc(8 * sizeof(Neighbor));

	int dx[] = {-1,0,1,0,-2,0,2,0};
	int dy[] = {0,1,0,-1,0,2,0,-2};

	for(int i = 0; i < 8; i++) {
		n.offsets[i] = (Neighbor){dx[i],dy[i]};
	}

	return n;
}

/* SHAPE:
 * 
 * X   X
 *  X X
 *   O
 *  X X
 * X   X
 *
 */
Neighborhood get_8_neighbor_x() {
	Neighborhood n;
	n.count = 8;
	n.offsets = malloc(8 * sizeof(Neighbor));

	int dx[] = {-1,1,1,-1,-2,2,2,-2};
	int dy[] = {-1,-1,1,1,-2,-2,2,2};

	for(int i = 0; i < 8; i++) {
		n.offsets[i] = (Neighbor){dx[i],dy[i]};
	}

	return n;
}

/* SHAPE:
 *
 * XXXXX
 * XXXXX
 * XXOXX
 * XXXXX
 * XXXXX
 *
 */
Neighborhood get_8_16_neighbor_square() {
	Neighborhood n;
	n.count = 24;
	n.offsets = malloc(24 * sizeof(Neighbor));

	int dx[] = {-1,1,1,-1,-1,0,1,0,-2,-1,0,1,2,-2,-1,0,1,2,-2,-2,-2,2,2,2};
	int dy[] = {-1,-1,1,1,0,1,0,-1,-2,-2,-2,-2,-2,2,2,2,2,2,-1,0,1,-1,0,1};

	for(int i = 0; i < 24; i++) {
		n.offsets[i] = (Neighbor){dx[i],dy[i]};
	}

	return n;
}

/* SHAPE:	
 *
 *	.
 *	.
 *	.
 *      X
 *      X
 * ...XXOXX...
 *      X
 *      X
 *      .
 *      .
 *      .
 *
 */
Neighborhood get_row_col_neighborhood(int width, int height) {
    Neighborhood n;
    n.count = (width - 1) + (height - 1);
    n.offsets = malloc(sizeof(Neighbor) * n.count);

    int id = 0;

    for(int i = 1; i < width; i++) {
        n.offsets[id++] = (Neighbor){i, 0};
    }

    for(int j = 1; j < height; j++) {
        n.offsets[id++] = (Neighbor){0, j};
    }

    return n;
}

/* SHAPE:
 *
 * .         .
 *  .       .
 *   .     .
 *    X   X
 *     X X
 *      O
 *     X X
 *    X   X
 *   .	   .
 *  .       .
 * .         .
 *
 */
Neighborhood get_full_x_neighborhood(int width, int height) {
    Neighborhood n;
    n.count = (width - 1) + (height - 1);
    n.offsets = malloc(sizeof(Neighbor) * n.count);

    int id = 0;

    for(int i = 1; i < width; i++) {
        n.offsets[id++] = (Neighbor){i, i};
    }

    for(int j = 1; j < height; j++) {
        n.offsets[id++] = (Neighbor){j, -j};
    }

    return n;
}

double exponential_cooling(double T, double T0, double temp_lower, int i) {
	return T * temp_lower;
}

double linear_cooling(double T, double T0, double delta, int i) {
	return T - delta;
}

double log_cooling(double T, double T0, double temp_lower, int i) {
	return T0 / log(i + 2.718281828);
}

// we calculate euclidean distance between points
double dist(int x, int y, int n_x, int n_y) {
    double dx = x - n_x;
    double dy = y - n_y;
    return sqrt(dx * dx + dy * dy);
}

// points of the different color repel each other
double energy_basic(unsigned char* bitmap, int width, int x, int y, int n_x, int n_y) {
	if (bitmap[width * y + x] != bitmap[width * n_y + n_x]) return 1; 
	
	return 0;
}

// points of the different color repel each other based on the distance between points
double energy_dist(unsigned char* bitmap, int width, int x, int y, int n_x, int n_y) {
	if (bitmap[width * y + x] != bitmap[width * n_y + n_x]) return 1 / dist(x, y, n_x, n_y); 

	return 0;
}

// we calculate the energy of a point by summing the energy of all its neighbors based on the get_energy function that we're using
double get_point_energy(unsigned char* bitmap, int width, int height, int x, int y, Neighborhood neighbors, EnergyFunc get_energy) {
	double energy = 0;

	for(int i = 0; i < neighbors.count; i++) {
		int n_x = (x + neighbors.offsets[i].dx + width) % width;
		int n_y = (y + neighbors.offsets[i].dy + height) % height;

		energy += get_energy(bitmap, width, x, y, n_x, n_y);
	}

	return energy;
}

// to calculate the energy of the whole bitmap we calculate the local energy of every point and sum them
double get_full_energy(unsigned char* bitmap, int width, int height, Neighborhood neighbors, EnergyFunc get_energy) {
	double energy = 0;

	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			energy += get_point_energy(bitmap, width, height, x, y, neighbors, get_energy);
		}
	}

	return energy;
}

void simulated_annealing(unsigned char* bitmap, int width, int height, int iters, double temp, double step_lower, Neighborhood neighbors, EnergyFunc get_energy, CoolingFunc get_temp, char* out_dir) {
	double current_energy = get_full_energy(bitmap, width, height, neighbors, get_energy);
	double temp_init = temp;

	char energy_path[512], temperature_path[512];

	sprintf(energy_path, "%s/energy_log.csv", out_dir);
	FILE* f_energy = fopen(energy_path, "w");
	fprintf(f_energy, "iteration,distance\n");

	sprintf(temperature_path, "%s/temp_log.csv", out_dir);
	FILE* f_temperature = fopen(temperature_path, "w");
	fprintf(f_temperature, "iteration,temp\n");
	
        // how often to save information for later plotting
	int total_frames = 100; 
    	int frame_interval = iters / total_frames;
    	int energy_interval = iters / 10000;

	for(int i = 0; i < iters; i++) {
		// save current energy and temperature
		if (i % energy_interval == 0) {
		    fprintf(f_energy, "%d,%.15f\n", i, current_energy);
		    fprintf(f_temperature, "%d,%.15f\n", i, temp);
		}

		// save current bitmap as frame for later animating into a gif
		if(i % frame_interval == 0) {
		    int frame_id = i / frame_interval;

		    char filename[512];
		    sprintf(filename, "%s/frames/frame_%04d.pgm",out_dir, frame_id);
		    
		    write_pgm(filename, bitmap, width, height);
		}


		// we keep picking random points until they are different colors
		// this could be done better by keeping lists of all black and white points
		// but it's good enough for our needs
		int x1, y1, x2, y2;
		int id1, id2;

		do {
		x1 = rand() % width;
		y1 = rand() % height;
		x2 = rand() % width;
		y2 = rand() % height;

		id1 = y1 * width + x1;
		id2 = y2 * width + x2;
		}while (bitmap[id1] == bitmap[id2]);

		// we don't need to calculate the energy of the whole bitmap each time as it is very expensive
		// we can calculate only the change in energy that swapping the points causes
		// so we calculate the energy of the chosen points
		// then swap them and calculate the energy again
		// by subtracting them we get the delta which shows us if the swap lowers the totale energy 
		double old_energy = get_point_energy(bitmap, width, height, x1, y1, neighbors, get_energy) + get_point_energy(bitmap, width, height, x2, y2, neighbors, get_energy);

		unsigned char to_switch = bitmap[id1];
		bitmap[id1] = bitmap[id2];
		bitmap[id2] = to_switch;

		double new_energy = get_point_energy(bitmap, width, height, x1, y1, neighbors, get_energy) + get_point_energy(bitmap, width, height, x2, y2, neighbors, get_energy);

		double delta = new_energy - old_energy;

		if(delta < 0 || ((double) rand() / RAND_MAX) < exp(-delta / temp)) {
			current_energy += delta;
		} else {
			unsigned char back = bitmap[id1];
			bitmap[id1] = bitmap[id2];
			bitmap[id2] = back;
		}
		
		// lower temperature using the chosen cooling method
		temp = get_temp(temp, temp_init, step_lower, i);
	}
	fclose(f_energy);
	fclose(f_temperature);
}

int main(int argc, char* argv[]) {
    	char* input_file = argv[1];
	char* out_dir = argv[2];
    	int iters = atoi(argv[3]);
    	double temp = atof(argv[4]);
    	double temp_lower = atof(argv[5]);
	int cool_type = atoi(argv[6]);
	int energy_type = atoi(argv[7]);
	int neighbor_type = atoi(argv[8]);

	int width, height;

	unsigned char* bitmap = read_pgm(input_file, &width, &height);

	CoolingFunc get_temp;
	EnergyFunc get_energy;
	Neighborhood neighbors;

	switch (cool_type) {
		case 0:
			get_temp= exponential_cooling;
			break;
		case 1:
			get_temp= linear_cooling;
			temp_lower = (temp - 1e-10) / iters;
			break;
		case 2:
			get_temp= log_cooling;
			break;
		default:
			get_temp = exponential_cooling;
			break;
	}

	switch (energy_type) {
		case 0:
			get_energy = energy_basic;
			break;
		case 1:
			get_energy = energy_dist;
			break;
		default:
			get_energy = energy_basic;
			break;
	}

	switch (neighbor_type) {
		case 0:
			neighbors = get_4_neighbor_plus();
			break;
		case 1:
			neighbors = get_4_neighbor_x();
			break;
		case 2:
			neighbors = get_8_neighbor_plus();
			break;
		case 3:
			neighbors = get_8_neighbor_x();
			break;
		case 4:
			neighbors = get_8_neighbor_square();
			break;
		case 5:
			neighbors = get_8_16_neighbor_square();
			break;
		case 6:
			neighbors = get_row_col_neighborhood(width, height);
			break;
		case 7:
			neighbors = get_full_x_neighborhood(width, height);
			break;
		default:
			neighbors = get_4_neighbor_plus();
			break;
	}

	printf("PROCESSING %s\n",out_dir);
    	clock_t start_t = clock();

	simulated_annealing(bitmap, width, height, iters, temp, temp_lower, neighbors, get_energy, get_temp, out_dir);

	clock_t end_t = clock();
    	double time_taken = (double)(end_t - start_t) / CLOCKS_PER_SEC;

	char filename[512];
	sprintf(filename, "%s/done.pgm",out_dir);

	char stats_path[512];
	sprintf(stats_path, "%s/stats", out_dir);

	FILE* f_stats = fopen(stats_path, "w");
	fprintf(f_stats, "%f\n%f\n", time_taken, get_full_energy(bitmap, width, height, neighbors, get_energy));
	fclose(f_stats);

	write_pgm(filename, bitmap, width, height);
	printf("DONE  TIME: %f\n",time_taken);
}
