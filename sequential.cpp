#include <iostream>
#include <functional>
#include <random>
#include <time.h>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <stdlib.h>

float given_function(float x, float y){

	// as an example I declare function as x + y
	return x+y;

}


float swarm_particle_optimization(std::function<float(float, float)> given_function, int number_of_particles,
	int number_of_iterations, float minimum_x, float maxmimum_x, float minimum_y, float maxmimum_y, float a, float b,
	float c){

	float position_of_particles[2][number_of_particles];
	float best_local_optimum[3][number_of_particles];
	float best_global_optimum[3][1];
	best_global_optimum[2][0] = 10000;
	best_global_optimum[0][0] = 0;
	best_global_optimum[1][0] = 0;
	float velocity[2][number_of_particles];

	srand(time(NULL));

	//INITIALIZATION:
	//distribute n particles across the search space
	std::random_device rd;
	std::default_random_engine generator(rd());
  	std::uniform_real_distribution<float> distribution_x(minimum_x, maxmimum_x);
  	std::uniform_real_distribution<float> distribution_y(minimum_y, maxmimum_y);

  	for (int i = 0; i<number_of_particles; ++i) {
    	float number_x = fmod(distribution_x(generator) - minimum_x, maxmimum_x);
    	float number_y = fmod(distribution_y(generator) - minimum_y, maxmimum_y);
    	position_of_particles[0][i] = number_x;
    	position_of_particles[1][i] = number_y;
    	std::cout<<"x: "<< position_of_particles[0][i]<<"\t\ty: "<<position_of_particles[1][i]<<std::endl;
  	}

	std::cout<<"=============================="<<std::endl;
  	
	//evaluate the objective function f for each particle position and assign the computed value as local optimum
	//and assign to the global optimum the “best” computed local optimum
	for(int i = 0; i<number_of_particles; i++){
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
	for(int i = 0; i<number_of_particles; i++){
		velocity[0][i] = rand() % 19 + (-9);
		velocity[1][i] = rand() % 19 + (-9);
	}


	//ITERATION
	for(int i = 1; i< number_of_iterations; i++){
		
		//for each particle, update position
		//Pos(t+1) = Pos(t) + V(t+1) 
		for(int j = 0; j<number_of_particles; j++){
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
		for(int j = 0; j < number_of_particles; j++){
			float helping_variable;
			helping_variable = a * velocity[0][j] + b * ((float) rand()/RAND_MAX) *  (position_of_particles[0][j] - 
				best_local_optimum[0][j]) + c * ((float) rand()/RAND_MAX) * (position_of_particles[0][j] - best_global_optimum[0][0]);

			if(helping_variable < -19){helping_variable = -19;}
			if(helping_variable > 19){helping_variable = 19;}	
			velocity[0][j] = helping_variable;

			helping_variable = a * velocity[1][j] + b * ((float) rand()/RAND_MAX) * (position_of_particles[1][j] - 
				best_local_optimum[1][j]) + c * ((float) rand()/RAND_MAX) * (position_of_particles[1][j] - best_global_optimum[1][0]);
			if(helping_variable < -19){helping_variable = -19;}
			if(helping_variable > 19){helping_variable = 19;}	
			velocity[1][j] = helping_variable;
		}

	}

	return best_global_optimum[2][0];

}



int main()
{
	int number_of_particles = 100;
	int number_of_iterations = 1000;
	int minimum_x = 1;
	int maxmimum_x = 100;
	int minimum_y = 2;
	int maxmimum_y = 50;
	int result;
	float a = 0.2;
	float b = 0.1;
	float c = 0.3;

	auto start = std::chrono::high_resolution_clock::now();

	result = swarm_particle_optimization(given_function, number_of_particles,
	 number_of_iterations, minimum_x, maxmimum_x, minimum_y, maxmimum_y, a, b, c);
	
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	std::cout<<"time: "<<usec<<" microseconds"<<std::endl;

	std::cout << "The minimum of a function is: " << result << "\n";
	
	return 0;
}