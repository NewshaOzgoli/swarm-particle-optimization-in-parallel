#include <iostream>
#include <functional>
#include <random>
#include <time.h>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <stdlib.h>
#include <thread>
#include <vector>
#include <atomic>
#include <pthread.h> 

using namespace std;


//global variables
const int number_of_particles = 10000;
int number_of_iterations = 10000;
int minimum_x = 1;
int maxmimum_x = 100;
int minimum_y = 2;
int maxmimum_y = 100;
int result;
float a = 0.2;
float b = 0.1;
float c = 0.3;
float position_of_particles[2][number_of_particles];
float best_local_optimum[3][number_of_particles];
atomic<float> best_global_optimum[3][1];
float velocity[2][number_of_particles];


//use for divide particles between threads
typedef struct{
	int start;
	int end;
} RANGE;

float given_function(float x, float y){

	// as an example I declare function as x + y
	return x+y;

}

float rand_function(const float & min, const float & max) {
    static thread_local mt19937* generator = nullptr;
    if (!generator) generator = new mt19937(clock() + std::hash<std::thread::id>()(std::this_thread::get_id()));
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(*generator);
}

void swarm_particle_optimization_initialization(RANGE range){

	//INITIALIZATION:

	//distribute n particles across the search space
  	for (int i = range.start; i<range.end; ++i) {
  		position_of_particles[0][i] = rand_function(minimum_x,maxmimum_x);
  		position_of_particles[1][i] = rand_function(minimum_y,maxmimum_y);

  	}
  	
	//evaluate the objective function f for each particle position and assign the computed value as local optimum
	//and assign to the global optimum the “best” computed local optimum
	for(int i = range.start; i<range.end; ++i){
		best_local_optimum[0][i] = position_of_particles[0][i];
		best_local_optimum[1][i] = position_of_particles[1][i];
		best_local_optimum[2][i] = given_function(best_local_optimum[0][i], best_local_optimum[1][i]);
		if(best_local_optimum[2][i] < best_global_optimum[2][0]){
			best_global_optimum[0][0] = best_local_optimum[0][i];
			best_global_optimum[1][0] = best_local_optimum[1][i];
			best_global_optimum[2][0] = best_local_optimum[2][i];
		}
	}
	
	//randomly initialize particle velocities
	for(int i = range.start; i<range.end; ++i){
		velocity[0][i] = rand_function(-18,18);
		velocity[1][i] = rand_function(-18,18);
	}
	

}

void swarm_particle_optimization_iteration(RANGE range){
	//ITERATION
		
	//for each particle, update position
	//Pos(t+1) = Pos(t) + V(t+1) 
	for(int j = range.start; j<range.end; ++j){
		position_of_particles[0][j] = position_of_particles[0][j] + velocity[0][j];
		position_of_particles[1][j] = position_of_particles[1][j] + velocity[1][j];
		//what happend if particle is out of range
		if(position_of_particles[0][j] < minimum_x){
			position_of_particles[0][j] = minimum_x;
		}
		if(position_of_particles[0][j] > maxmimum_x){
			position_of_particles[0][j] = maxmimum_x;
		}
		if(position_of_particles[1][j] < minimum_y){
			position_of_particles[1][j] = minimum_y;			}
		if(position_of_particles[1][j] > maxmimum_y){
			position_of_particles[1][j] = maxmimum_y;
		}
		//re-evaluate local and global optimal
		float given_function_with_new_particle = given_function(position_of_particles[0][j],position_of_particles[1][j]);
		if(best_local_optimum[2][j] > given_function_with_new_particle){
			best_local_optimum[0][j] = position_of_particles[0][j];
			best_local_optimum[1][j] = position_of_particles[1][j];
			best_local_optimum[2][j] = given_function_with_new_particle;
			if(best_global_optimum[2][j] > given_function_with_new_particle){
				best_global_optimum[0][j] = position_of_particles[0][j];
				best_global_optimum[1][j] = position_of_particles[1][j];
				best_global_optimum[2][j] = given_function_with_new_particle;
			}
		}
	}

	//for each particle, update velocity.
	//V(t+1) = a V(t) + b R 1 (Pos(t) - Pos(localOpt)) + c R 2 (Pos(t) – Pos(globalOpt))
	//srand( (unsigned)time( NULL ) );
	for(int j = range.start; j<range.end; ++j){
		float helping_variable;
		helping_variable = a * velocity[0][j] + b * rand_function(0,1) *  (position_of_particles[0][j] - 
			best_local_optimum[0][j]) + c * rand_function(0,1) * (position_of_particles[0][j] - best_global_optimum[0][0]);

		if(helping_variable < -19){helping_variable = -19;}
		if(helping_variable > 19){helping_variable = 19;}	
		velocity[0][j] = helping_variable;

		helping_variable = a * velocity[1][j] + b * rand_function(0,1) * (position_of_particles[1][j] - 
			best_local_optimum[1][j]) + c * rand_function(0,1) * (position_of_particles[1][j] - best_global_optimum[1][0]);
		if(helping_variable < -19){helping_variable = -19;}
		if(helping_variable > 19){helping_variable = 19;}	
		velocity[1][j] = helping_variable;
	}
}

int main(int argc, char * argv[])
{
	int number_of_threads = atoi(argv[1]);
	best_global_optimum[2][0] = 10000;
	best_global_optimum[0][0] = 0;
	best_global_optimum[1][0] = 0;


	vector<RANGE> ranges(number_of_threads);
	int delta{number_of_particles/number_of_threads};
	vector<thread> tids;
	vector<thread> tid;

	
	//assign particles to each thread
	for (int i = 0; i < number_of_threads; ++i){
		ranges[i].start = i * delta;
		ranges[i].end = (i != (number_of_threads-1) ? (i+1)*delta : number_of_particles);
	}

	auto start = std::chrono::high_resolution_clock::now();
	
	//Do initialization in parallel
	for (int i = 0; i < number_of_threads; ++i){
		tids.push_back(thread(swarm_particle_optimization_initialization,ranges[i]));
	}
	for (thread& t: tids){
		t.join();
	}

	//Do iterations in parallel
	for(int j = 1; j< number_of_iterations; j++){
		for (int i = 0; i < number_of_threads; ++i){
				tid.push_back(thread(swarm_particle_optimization_iteration,ranges[i]));
			}
		for (thread& t: tid){
			t.join();
		}
		tid.clear();
	}
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	std::cout<<"time: "<<usec<<" microseconds"<<std::endl;

	std::cout << "The minimum of a function is: " << best_global_optimum[2][0] << "\n";
	
	
	return 0; 
}