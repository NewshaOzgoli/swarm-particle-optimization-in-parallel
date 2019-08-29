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
#include <mutex>

using namespace std;


//parameters with default numbers
	int number_of_threads;
    int number_of_particles = 10000;
    int number_of_iterations = 1000;
	float minimum_x = 1;
	float maxmimum_x = 100;
	float minimum_y = 2;
	float maxmimum_y = 100;
	float result;
	float a = 0.2;
	float b = 0.1;
	float c = 0.3;
	float best_global_optimum[3][1];

	std::mutex mtx;
	pthread_barrier_t mybarrier;

//for dividing particles between threads
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

void swarm_particle_optimization(RANGE range,float **position_of_particles,
	float **best_local_optimum,float **velocity){
	
	//INITIALIZATION:

	//distribute n particles across the search space
  	for (int i = range.start; i<range.end; ++i) {
  		position_of_particles[0][i] = rand_function(minimum_x,maxmimum_x);
  		position_of_particles[1][i] = rand_function(minimum_y,maxmimum_y);

  	}
  	
  	//randomly initialize particle velocities
	for(int i = range.start; i<range.end; ++i){
		velocity[0][i] = rand_function(-5,5);
		velocity[1][i] = rand_function(-5,5);
	}

	//evaluate the objective function f for each particle position and assign the computed value as local optimum
	//and assign to the global optimum the “best” computed local optimum
	for(int i = range.start; i<range.end; ++i){
		best_local_optimum[0][i] = position_of_particles[0][i];
		best_local_optimum[1][i] = position_of_particles[1][i];
		best_local_optimum[2][i] = given_function(best_local_optimum[0][i], best_local_optimum[1][i]);
		if(best_local_optimum[2][i] < best_global_optimum[2][0]){
			std::lock_guard<std::mutex> lock(mtx);
			best_global_optimum[0][0] = best_local_optimum[0][i];
			best_global_optimum[1][0] = best_local_optimum[1][i];
			best_global_optimum[2][0] = best_local_optimum[2][i];
		}
	}
	pthread_barrier_wait(&mybarrier);

	//ITERATION
	for(int z = 1; z< number_of_iterations; z++){
		//for avoiding the iteration using the wrong global oprimum
		float old_global_optimum = best_global_optimum[2][0];
			
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
				position_of_particles[1][j] = minimum_y;			
			}
			if(position_of_particles[1][j] > maxmimum_y){
				position_of_particles[1][j] = maxmimum_y;
			}
			//re-evaluate local and global optimal
			float given_function_with_new_particle = given_function(position_of_particles[0][j],position_of_particles[1][j]);

			if(best_local_optimum[2][j] > given_function_with_new_particle){
				best_local_optimum[0][j] = position_of_particles[0][j];
				best_local_optimum[1][j] = position_of_particles[1][j];
				best_local_optimum[2][j] = given_function_with_new_particle;
				if(old_global_optimum > given_function_with_new_particle){
					std::lock_guard<std::mutex> lock(mtx);
					best_global_optimum[0][0] = position_of_particles[0][j];
					best_global_optimum[1][0] = position_of_particles[1][j];
					best_global_optimum[2][0] = given_function_with_new_particle;
				}
			}
		}

		//for each particle, update velocity.
		//V(t+1) = a V(t) + b R 1 (Pos(t) - Pos(localOpt)) + c R 2 (Pos(t) – Pos(globalOpt))
	
		for(int j = range.start; j<range.end; ++j){
			float helping_variable;
			helping_variable = a * velocity[0][j] + b * rand_function(0,1) *  (position_of_particles[0][j] - 
				best_local_optimum[0][j]) + c * rand_function(0,1) * (position_of_particles[0][j] - old_global_optimum);

			if(helping_variable < -5){helping_variable = -5;}
			if(helping_variable > 5){helping_variable = 5;}	
			velocity[0][j] = helping_variable;

			helping_variable = a * velocity[1][j] + b * rand_function(0,1) * (position_of_particles[1][j] - 
				best_local_optimum[1][j]) + c * rand_function(0,1) * (position_of_particles[1][j] - old_global_optimum);
			if(helping_variable < -5){helping_variable = -5;}
			if(helping_variable > 5){helping_variable = 5;}	
			velocity[1][j] = helping_variable;
		}
		pthread_barrier_wait(&mybarrier);
	}
}


int main(int argc, char * argv[])
{	
	
	if (argc == 2){
		number_of_threads = atoi(argv[1]);
	}

	//changing the number of parameters
	else if(argc == 8){
		number_of_threads = atoi(argv[1]);
		number_of_particles = atoi(argv[2]);
		number_of_iterations = atoi(argv[3]);
		minimum_x = atoi(argv[4]);
		maxmimum_x = atoi(argv[5]);
		minimum_y = atoi(argv[6]);
		maxmimum_y = atoi(argv[7]);
	}
	else if(argc == 11){
		number_of_threads = atoi(argv[1]);
		number_of_particles = atoi(argv[2]);
		number_of_iterations = atoi(argv[3]);
		minimum_x = atoi(argv[4]);
		maxmimum_x = atoi(argv[5]);
		minimum_y = atoi(argv[6]);
		maxmimum_y = atoi(argv[7]);
		a = atoi(argv[8]);
		b = atoi(argv[9]);
		c = atoi(argv[10]);

	}
	else{
		std::cout<<"The numbers of parameters aren't correct"<<std::endl;
		return 0;
	}

	float **position_of_particles = new float*[2];
	for (int i = 0; i < 2; ++i){
		position_of_particles[i] = new float[number_of_particles];
	}
	float **best_local_optimum = new float*[3];
	for (int i = 0; i < 3; ++i){
		best_local_optimum[i] = new float[number_of_particles];
	}
	
	float **velocity = new float*[2];
	for (int i = 0; i < 2; ++i)
	{
		velocity[i] = new float[number_of_particles];
	}

	//declare best global optimum as a high number
   	best_global_optimum[2][0] = 10000;
	best_global_optimum[0][0] = 0;
	best_global_optimum[1][0] = 0;

	pthread_barrier_init(&mybarrier, NULL, number_of_threads);

	vector<RANGE> ranges(number_of_threads);
	int delta{number_of_particles/number_of_threads};
	vector<thread> tid;

	//assign particles to each thread
	for (int i = 0; i < number_of_threads; ++i){
		ranges[i].start = i * delta;
		ranges[i].end = (i != (number_of_threads-1) ? (i+1)*delta : number_of_particles);
	}

	auto start = std::chrono::high_resolution_clock::now();
	
	
	for (int i = 0; i < number_of_threads; ++i){
			tid.push_back(thread(swarm_particle_optimization,ranges[i],
				position_of_particles,best_local_optimum,velocity));
		}
	for (thread& t: tid){
		t.join();
	}
	
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	std::cout<<"time: "<<usec<<" microseconds"<<std::endl;

	std::cout << "The minimum of a function is: " << best_global_optimum[2][0] << "\n";
	
	
	return 0;
}
