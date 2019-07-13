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


float swarm_particle_optimization(int number_of_particles,int number_of_iterations,float minimum_x,
		float maxmimum_x,float minimum_y,float maxmimum_y,float a,float b,float c,
		float **position_of_particles,float **best_local_optimum,float **best_global_optimum,
		float **velocity){
	
	float given_function_with_new_particle;

	//INITIALIZATION:

	//distribute n particles across the search space
	std::random_device rd;
	std::default_random_engine generator(rd());
  	std::uniform_real_distribution<float> distribution_x(minimum_x, maxmimum_x);
  	std::uniform_real_distribution<float> distribution_y(minimum_y, maxmimum_y);

  	for (int i = 0; i<number_of_particles; ++i) {
    	float number_x = distribution_x(generator);
    	float number_y = distribution_y(generator);
    	position_of_particles[0][i] = number_x;
    	position_of_particles[1][i] = number_y;
  	}

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
	//I choose numbers to be only in range(-5,5)
	for(int i = 0; i<number_of_particles; i++){
		velocity[0][i] = rand() % 5 + (-5);
		velocity[1][i] = rand() % 5 + (-5);
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
			given_function_with_new_particle = given_function(position_of_particles[0][j],position_of_particles[1][j]);
			if(best_local_optimum[2][j] > given_function_with_new_particle){
				best_local_optimum[0][j] = position_of_particles[0][j];
				best_local_optimum[1][j] = position_of_particles[1][j];
				best_local_optimum[2][j] = given_function_with_new_particle;
				if(best_global_optimum[2][0] > best_local_optimum[2][j]){
					
					best_global_optimum[0][0] = position_of_particles[0][j];
					best_global_optimum[1][0] = position_of_particles[1][j];
					best_global_optimum[2][0] = best_local_optimum[2][j];
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

			if(helping_variable < -5){helping_variable = -5;}
			if(helping_variable > 5){helping_variable = 5;}	
			velocity[0][j] = helping_variable;

			helping_variable = a * velocity[1][j] + b * ((float) rand()/RAND_MAX) * (position_of_particles[1][j] - 
				best_local_optimum[1][j]) + c * ((float) rand()/RAND_MAX) * (position_of_particles[1][j] - best_global_optimum[1][0]);
			if(helping_variable < -5){helping_variable = -5;}
			if(helping_variable > 5){helping_variable = 5;}	
			velocity[1][j] = helping_variable;
		}
	}

	return best_global_optimum[2][0];
}



int main(int argc, char * argv[])
{
	//parameters with default numbers
    int number_of_particles = 1000;
    int number_of_iterations = 100;
	float minimum_x = 1;
	float maxmimum_x = 100;
	float minimum_y = 2;
	float maxmimum_y = 100;
	float result;
	float a = 0.2;
	float b = 0.1;
	float c = 0.3;
	
	if (argc == 1){}

	//changing the number of parameters
	else if(argc == 7){
		number_of_particles = atoi(argv[1]);
		number_of_iterations = atoi(argv[2]);
		minimum_x = atoi(argv[3]);
		maxmimum_x = atoi(argv[4]);
		minimum_y = atoi(argv[5]);
		maxmimum_y = atoi(argv[6]);
	}
	else if(argc == 10){
		number_of_particles = atoi(argv[1]);
		number_of_iterations = atoi(argv[2]);
		minimum_x = atoi(argv[3]);
		maxmimum_x = atoi(argv[4]);
		minimum_y = atoi(argv[5]);
		maxmimum_y = atoi(argv[6]);
		a = atoi(argv[7]);
		b = atoi(argv[8]);
		c = atoi(argv[9]);

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
	
	float **best_global_optimum = new float*[3];
	for (int i = 0; i < 3; ++i)
	{
		best_global_optimum[i] = new float[1];
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

	auto start = std::chrono::high_resolution_clock::now();

	result = swarm_particle_optimization(number_of_particles,number_of_iterations,minimum_x,
		maxmimum_x,minimum_y,maxmimum_y,a,b,c,position_of_particles,best_local_optimum,best_global_optimum,
		velocity);
	
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	std::cout<<"time: "<<usec<<" microseconds"<<std::endl;

	std::cout << "The minimum of a function is: " << result << "\n";
	
	return 0;
}