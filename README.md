# Swarm Particle Optimization in Parallel
## What is Swarm Particle Optimization
The target is to develop an application finding the minimum of a function in a given interval using swarm
particle optimization. The Swarm Particle Optimization (SPO) uses particles to explore the state
of solutions. It is an iterative process where a set of particles, initially spread randomly across the solution
space, autonomously travel the solution space driven by local and global knowledge relative to the
solution explored so far. 
Swarm particle optimization has two phases. First phase is initialization which we initialize and distribute n particles across the search space. After that we evaluate the objective function ‘f’ for each particle position and assign the computed value as local optimum and assign to the global optimum the minimum computed local optimum and randomly initialize particle velocities. Next is iteration phase which for each iteration and for each particle, we update position as a function of current velocity, distance from local optimum and distance from global optimum.  After that, we re-evaluate local and global optima.
## How I Implement It in Parallel
For doing things in parallel for the initialization phase, I split the particles between threads so each thread first compute the initial position and second each thread computes the velocity of particles that belongs to it. Third, evaluate the objective function ‘f’ for each particle position and assign the computed value as local optimum and also checks if this particle is better than the global optimum and change it. But for changing the global optimum we need to use lock because threads can change the global optimum simultaneously.
Iteration phase is the same as the initialization phase in which I split particles between threads. First, each thread computes the new position of the particles, second checks if it should change the local optimum and global optimum, third computes velocity.  ALso again we need the lock for changing global optimum. 
We do the iteration phase as much as the user wants but the point here is that after each iteration, before starting a new iteration we need a barrier which waits for all the threads to finish that specific iteration then we can start a new iteration. Another detail worth mentioning is that before starting the iteration phase we have to make sure that the initialization phase has been finished.
 
## Run the Code
### sequential version
For running the program we have three options:
1. Give no parameters to the program. In this situation the program will run with default parameters which are: number of particles = 10000, number of iterations = 1000, minimum for dimension x = 1, maximum for dimension x = 100, minimum for dimension y = 2, maximum for dimension y = 100, ‘a’ in the formula for changing the velocity = 0.2, ‘b’ in the formula for changing the velocity = 0.1, ‘c’ in the formula for changing the velocity = 0.3
2. Give 6 parameters to the program respectively indicating: number of particles, number of iterations, minimum for dimension, maximum for dimension x, minimum for dimension, maximum for dimension y. And for a,b and c the program will use the default numbers mentioned before.
3. Give 9 parameters to program Which in order they are: number of particles, number of iterations, minimum for dimension, maximum for dimension x, minimum for dimension y, maximum for dimension y, a, b and c.	
### pure c++ version
When we want to compile the program we need to use -pthread.
For running the program we have three options.
1. Give one parameter to the program which is the number of threads. So when we want to run the program for example with 2 threads we have to write something like this: ./name.out  2. In this situation the program will run with 2 threads and with default parameters which are the same as above.
2. Give 7 parameters to the program. Which in order they are: number of threads, number of particles, number of iterations, minimum for dimension, maximum for dimension x, minimum for dimension, maximum for dimension y. And for a,b and c the program will use the default numbers mentioned above.  
3. Give 10 parameters to the program: number of threads, number of particles, number of iterations, minimum for dimension, maximum for dimension x, minimum for dimension y, maximum for dimension y, a, b and c.
### fastflow version
For compiling the program we need to use -pthread and -I /the location of fast flow cloned from GitHub and we need to use -std=c++17. For running the program, it is the same as above. 
