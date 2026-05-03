#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Point {
    double x;
    double y;
};

// we euclidean distance between points
double dist(struct Point a, struct Point b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

double energy(struct Point route[], int n, struct Point start_point) {
    double total_energy = dist(start_point, route[0]); 

    for (int i = 0; i < n - 1; i++) {
        total_energy += dist(route[i], route[i + 1]);
    }

    // distance back to the start point from the end of the route
    total_energy += dist(route[n-1], start_point);
    return total_energy;
}

// loopback logic for getting the next point in the route
struct Point get_p(int idx, struct Point route[], int n, struct Point start) {
    if (idx < 0 || idx >= n) return start;
    return route[idx];
}

double calculate_local_energy(struct Point route[], int n, struct Point start, int i, int j) {
    double d = 0;

    d += dist(get_p(i - 1, route, n, start), route[i]);
    d += dist(route[i], get_p(i + 1, route, n, start));
    
    // check if the points aren't neighbours
    if (j - 1 != i && j + 1 != i) {
        d += dist(get_p(j - 1, route, n, start), route[j]);
        d += dist(route[j], get_p(j + 1, route, n, start));
    } else {
        if (j > i) {
            d += dist(route[j], get_p(j + 1, route, n, start));
        } else {
            d += dist(get_p(j - 1, route, n, start), route[j]);
        }
    }

    return d;
}

// randomly shuffle the original points
void shuffle_points(struct Point points[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int j = i + rand() % (n - i);

        struct Point temp = points[i];
        points[i] = points[j];
        points[j] = temp;
    }
}

void simulated_annealing(struct Point current_route[], int n, struct Point start_point, int iters, double temp, double temp_lower, int swap, int cool_type, char* out_dir) {
    double current_distance = energy(current_route, n, start_point);

    char energy_path[512], temperature_path[512];

    sprintf(energy_path, "%s/energy_log.csv", out_dir);
    FILE* f_energy = fopen(energy_path, "w");
    fprintf(f_energy, "iteration,distance\n");

    sprintf(temperature_path, "%s/temp_log.csv", out_dir);
    FILE* f_temperature = fopen(temperature_path, "w");
    fprintf(f_temperature, "iteration,temp\n");

    char frames_path[512];
    sprintf(frames_path, "%s/route_frames.csv", out_dir);
    FILE* f_frames = fopen(frames_path, "w");
    fprintf(f_frames, "frame,distance,x,y\n");
    
    // how often to save information for later plotting
    int total_frames = 200; 
    int frame_interval = iters / total_frames;
    int energy_interval = iters / 10000;

    double delta_t = (temp - 1e-10) / iters;
    double temp_init = temp;
    for(int i = 0; i < iters; i++) {
	// save current energy and temperature
    	if (i % energy_interval == 0) {
            fprintf(f_energy, "%d,%.15f\n", i, current_distance);
            fprintf(f_temperature, "%d,%.15f\n", i, temp);
        }
	
	// save current path
	if (i % frame_interval == 0) {
	    int f_id = i / frame_interval;
	    fprintf(f_frames, "%d,%.15f,%.15f,%.15f\n", f_id, current_distance, start_point.x, start_point.y);

	    for(int p = 0; p < n; p++) 
		fprintf(f_frames, "%d,%.15f,%.15f,%.15f\n", f_id, current_distance, current_route[p].x, current_route[p].y);

	    fprintf(f_frames, "%d,%.15f,%.15f,%.15f\n", f_id, current_distance, start_point.x, start_point.y);
        }

	int id1,id2;
	if(swap == 0){
	// arbitrary_swap
		id1 = rand() % n; id2 = rand() % n;
	} else if(swap == 1) {
	// consecutive_swap
		id1 = rand() % (n - 1); id2 = id1 + 1;
	}
	
	// we don't need to calculate the energy of the whole path each time as it is very expensive
	// we can calculate only the change in energy that swapping the points causes
	// so we calculate the energy on edges of the chosen points
	// then swap them and calculate the energy on edges again
	// by subtracting them we get the delta which shows us if the swap lowers the total distance
	double old_edges = calculate_local_energy(current_route, n, start_point, id1, id2);

	struct Point temp_p = current_route[id1];
        current_route[id1] = current_route[id2];
        current_route[id2] = temp_p;

	double new_edges = calculate_local_energy(current_route, n, start_point, id1, id2);

	double delta = new_edges - old_edges;

        if(delta < 0 || ((double) rand() / RAND_MAX) < exp(-delta / temp)) {
		current_distance += delta;
        } else {
		// instead of keeping a copy of the old route we just swap the points back
		struct Point back = current_route[id1];
		current_route[id1] = current_route[id2];
		current_route[id2] = back;
	}

	if(cool_type == 0){
		// exponential
		temp *= temp_lower;
	} else if (cool_type == 1) {
		// linear
		temp -= delta_t;
	} else if (cool_type == 2) {
		// logarithmic
		// with euler number offset
		temp = temp_init / log(i + 2.718281828);
	}
    }
    fclose(f_frames);
    fclose(f_temperature);
    fclose(f_energy);
}

void TSP(struct Point points[], int n, int iters, double temp, double temp_lower, int swap, int cool_type, char* out_dir) {
    // pick random start point
    int i = rand() % n;	
    struct Point start_point = points[i];
    
    // we'll keep the start point as the last point
    struct Point temp_p = points[n-1];
    points[n-1] = points[i];
    points[i] = temp_p;

    int n_route = n - 1;

    // shuffle points to get initial path
    shuffle_points(points, n_route);

    printf("PROCESSING: %d\nBefore: %f\n",n,energy(points, n, start_point));

    clock_t start_t = clock();

    simulated_annealing(points, n, start_point, iters, temp, temp_lower, swap, cool_type, out_dir);

    clock_t end_t = clock();
    double time_taken = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    
    // saving the final route and stats to files
    char final_path[512], stats_path[512];
    sprintf(final_path, "%s/final_route.csv", out_dir);
    sprintf(stats_path, "%s/stats", out_dir);

    FILE* f_route = fopen(final_path, "w");
    fprintf(f_route, "x,y\n%.15f,%.15f\n", start_point.x, start_point.y);
    for(int i = 0; i < n_route; i++) fprintf(f_route, "%.15f,%.15f\n", points[i].x, points[i].y);
    fprintf(f_route, "%.15f,%.15f\n", start_point.x, start_point.y);
    fclose(f_route);

    FILE* f_stats = fopen(stats_path, "w");
    fprintf(f_stats, "%f\n%f\n", time_taken, energy(points, n_route, start_point));
    fclose(f_stats);

    printf("Done. Time: %fs | Final Distance: %f\n", time_taken, energy(points, n_route, start_point));
}

int main(int argc, char* argv[]) {
    char* input_file = argv[1];
    char* output_dir = argv[2];
    int n = atoi(argv[3]);
    int iters = atoi(argv[4]);
    int swap = atoi(argv[5]);
    double temp = atof(argv[6]);
    double temp_lower = atof(argv[7]);
    int cool_type = atoi(argv[8]);
    int seed = atoi(argv[9]);

    srand(seed);
    
    FILE* input = fopen(input_file, "r");

    struct Point points[n];
	
    // reading points from a file
    for(int i = 0; i < n; i++) {
	fscanf(input, "%lf %lf", &points[i].x, &points[i].y);
    }

    TSP(points, n, iters, temp, temp_lower, swap, cool_type, output_dir);

    return 1;
} 
