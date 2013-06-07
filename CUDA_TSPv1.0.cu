//By: Goodwyn, Zach & Mueller, Jerome
//This program uses a CUDA-enabled GPU to brute-force solve the travelling salesman problem

#include<stdio.h>
#include"math.h"
#include<stdlib.h>
#include<time.h>
#include<sys/time.h> // Used for timing this.
#include<unistd.h> // Used for timing this.
#include<algorithm>//needed for next_permutation
#include<climits>//needed for MAX_INT
#include<assert.h>
#include<time.h>
#include<sys/time.h> // Used for timing this.
#include<unistd.h> // Used for timing this.

#define MAX_PATH 1000
//#define NUM_CORES 96

__host__ __device__ void convertFact(unsigned long long, int*);
__host__ __device__ void orderPermutation(int*, unsigned long long, int);
unsigned long long factorial(unsigned long long);
int* generateArray(int num_Cities);
int charToInt(char* temp_Input);
void checkCuda(cudaError_t problem, int id);
__global__ void permute(int* device_Matrix, short* device_Cities, long long* device_Perm_Start_Indecies, int* device_Least_Cost_Array,long long* device_Least_Path_Array, int num_Cities, int threads_Per_Block);

//returns the factorial for the given number
//x: the index for which factorial number you want.
//returns: x!
unsigned long long factorial(unsigned long long x)
{
	if(x == 1)
		return 1;
	else
		return x * factorial(x-1);
}

//converts a number into base factoriaint** adjacency_Matrix = new int*[num_Cities];
//num: the number to convert to base factorial
//digits: a storage array to store the digits of the base factorial number
//numbers are stored in reverse order (so the 2nd digit is in position 1, the third in 2, etc.
//digits[0] will contain the length of the number in digits, since the first number of a base factorial number is always 0
__host__ __device__ void convertFact(unsigned long long num, int* digits)
{
	int numDigits = 1;//there is always a spare 0 in these numbers
	while(num > 0)
	{
		digits[numDigits] = num % (numDigits + 1);
		numDigits++;
		num /= numDigits;
	}
	digits[0] = numDigits;//because the first digit is always zero, we will 	store the length of the array in the 0th slot
}

//returns the array transposed to the nth permutation after the given ordering
//toOrder: the set for which you would like to define the nth permutation
//m: the numbered permutation to be stored in toOrder
//size: the size of the array toOrder
//warning: gives unpredictable behavior if n is > the number of unique permutations for toOrder
__host__ __device__ void orderPermutation(short* toOrder, unsigned long long n, int size)
{
	int swaps[100];
	convertFact(n, swaps);
	int location = size - swaps[0];//accounts for leading zeros
	int loc = swaps[0] - 1;//used to iterate through the digits of the 					factoradic number
	while(loc > 0)
	{
		int temp = toOrder[location + swaps[loc]];
		for(int x = location+swaps[loc]; x > location; x--)		
		{
			toOrder[x] = toOrder[x - 1];
		}
		toOrder[location] = temp;
		location++;
		loc--;
	}

}

//returns the array transposed to the nth permutation after the given ordering
//toOrder: the set for which you would like to define the nth permutation
//m: the numbered permutation to be stored in toOrder
//size: the size of the array toOrder
//warning: gives unpredictable behavior if n is > the number of unique permutations for toOrder
__host__ __device__ void orderPermutation(int* toOrder, unsigned long long n, int size)
{
	int swaps[100];
	convertFact(n, swaps);
	int location = size - swaps[0];//accounts for leading zeros
	int loc = swaps[0] - 1;//used to iterate through the digits of the 					factoradic number
	while(loc > 0)
	{
		int temp = toOrder[location + swaps[loc]];
		for(int x = location+swaps[loc]; x > location; x--)		
		{
			toOrder[x] = toOrder[x - 1];
		}
		toOrder[location] = temp;
		location++;
		loc--;
	}

}

//returns a pointer to a "two" dimensional randomly generated symmetrical adjacency matrix
//num_Cities: used to decide how much memory should be allocated
int* generateArray(int num_Cities)
{
	int* temp_Matrix = (int*)malloc(num_Cities*num_Cities*sizeof(int*));

	/*for(int index = 0; index < num_Cities; index++)
	{
		temp_Matrix[index] = (int*)malloc(num_Cities*sizeof(int));
	}*/
	for(int outer = 0; outer < num_Cities; outer++)
	{

		for(int inner = 0; inner <= outer; inner++)
		{

			temp_Matrix[outer * num_Cities + inner] = 1 + rand() % MAX_PATH;

			temp_Matrix[inner * num_Cities + outer] = temp_Matrix[outer * num_Cities + inner];
		}

	}

	return temp_Matrix;
	

}

//Converts a character string to an integer
//temp_Input: A cstring (char*) containing a number to translate
//Gives unpredictable, but not undefined behavior if temp_Input contains non-numeric characters,
//or if temp_Input is too large to be held in an int. Does not support negatives or decimals
int charToInt(char* temp_Input)
{
	int num_Digit = 0;
	int number = 0;

	while(temp_Input[num_Digit] != '\0')//loops until the end of the string
	{
		number = number * 10;//accounts for the place-value of the digits
		if(temp_Input[num_Digit] != '0')
		{
			//in character sets, the character one minus '1' yields zero, '2' minus '1' yields 1, etc.
			number += temp_Input[num_Digit] + 1 - '1';
		}
		num_Digit++;
	}
	return number;
		
}

//shorthand to check if a cuda error occured
//problem: cudaError_t storing the result of a cudaMalloc
//id: an integer id that gets print out with the message, default = -1
void checkCuda(cudaError_t problem, int id = -1)
{
	if(problem != cudaSuccess)
	{
		printf("%s\n", cudaGetErrorString(problem));
		printf("Terminating process with id = %d\n", id);
		abort();
	}
}

//Kernel Function!
//Each thread calculates its starting and ending index, then calculates the total costs of the paths assigned to it, then stores its best result in the device_Least_Cost_Array, and the index of that path in the device_Least_Path_Array
//ALL ARGUMENTS POINT TO THE GPU'S COPY OF THE GIVEN DATA STRUCTURE
//device_Matrix: The adjacency matrix representing the different costs of getting from one city to another
//device_Cities: The array containing EVERY thread's representation of the cities (i.e. each thread gets an array of num_Cities elements containing numbers 1 - [num_Cities-1]) See comments in method body for further comments
//device_Perm_Start_Indecies: The starting locations for a given Block of threads. It is up to the individual thread to calculate which paths to run based on this array and its thread ID
//device_Least_Cost_Array: When a thread has finished its permutations, it writes the cost of the shortest path it found into a unique slot in this array
//device_Least_Path_Array: The corresponding permutation number for the shortest calculated path. Used to retrieve the city-ordering for the best solution
//num_Cities: The number of cities in this instance of the travelling salesman problem
//threads_Per_Block: The number of threads in a given block. 
__global__ void permute(int* device_Matrix, short* device_Cities, long long* device_Perm_Start_Indecies, int* device_Least_Cost_Array,long long* device_Least_Path_Array, int num_Cities, int threads_Per_Block)
{
	int id = blockIdx.x * threads_Per_Block + threadIdx.x;//this id is unique for each thread
	//each thread works with a subset of the device_Cities array, this next statement procures the starting address of this thread's subset
	short* cities = device_Cities + (id * num_Cities);
	
	long long index;//which path permutation the thread is calculating
	int tot_Sum;//the running cost of the permutation this thread is calculating
	long long least_Path = 0;//the permutation id# of the shortest path this thread has found
	int least_Cost = 999999;//the least cost solution this thread has so far found
	float sectionRatio = (float)1.0/threads_Per_Block;//calculates what portion of the thread's block's workload needs to be completed by this thread
	long long block_Perms = device_Perm_Start_Indecies[blockIdx.x + 1] - device_Perm_Start_Indecies[blockIdx.x];//the total number permutations in this thread's block's workload
	long long start_Perm = (sectionRatio * threadIdx.x) * block_Perms;//An offset denoting which path permutation number this thread should start to calculate
	long long end_Perm = (sectionRatio * (threadIdx.x + 1)) * block_Perms;//An offset denoting the one permutation beyond what this thread should calculate

	orderPermutation(cities, device_Perm_Start_Indecies[blockIdx.x] + start_Perm, num_Cities);//initializes this thread's cities array to the appropriate ordering	

	//loops through all the permutations assigned to this thread
	for(index = device_Perm_Start_Indecies[blockIdx.x] + start_Perm ; index < device_Perm_Start_Indecies[blockIdx.x] + end_Perm ; index++)
	{
		tot_Sum = 0;
		int inner;
		for(inner = 0; inner < num_Cities; inner++)//for each city, looks up the distance to the next city and adds it to a running sum
		{
			tot_Sum += device_Matrix[cities[inner] * num_Cities + cities[(inner + 1) % num_Cities]];	
		}

		if(tot_Sum < least_Cost)//updates if the soplution is the best so far
		{
			least_Cost = tot_Sum;
			least_Path = index;
		}
		for(inner = 0; inner < num_Cities; inner++)//resets the cities array for use in orderPermutation
		{
			cities[inner] = inner;
		}
		orderPermutation(cities, index + 1, num_Cities);//sets the cities array to the next permutation
	}
	//writes this thread's best solutions to the two arrays for transfer back to the host
	device_Least_Cost_Array[id] = least_Cost;
	device_Least_Path_Array[id] = least_Path;	

	
}



int main(int argc, char* argv[])
{
	//initialize timer
	struct timeval startTime, endTime;
	gettimeofday(&startTime, NULL);

	//variables corresponding to the arguments
	unsigned int seeder;
	int num_Threads;//(threads per block)
	int num_Blocks;
	int num_Cities;
	int total_Threads;


	if(argc != 5)//if an improper number of parameters were passed
	{
		printf("Error: improper number of commands");
		printf("arguments: #cities (seed) (requestedCores)");
		fflush(stdout);
	}

	num_Cities = charToInt(argv[1]);

	seeder = charToInt(argv[2]);
	srand(seeder);

	num_Blocks = charToInt(argv[3]);
	
	num_Threads = charToInt(argv[4]);

	total_Threads = num_Blocks * num_Threads;
	
	//calculates the starting index for each block
	double section_Ratio = double(1)/num_Blocks;

	long long total_Perms = factorial(num_Cities);

	long long* perm_Start_Indecies = (long long*)malloc((num_Blocks + 1) * sizeof(long long));

	int index;
	for(index = 0; index < num_Blocks + 1; index++)
	{	
		perm_Start_Indecies[index] = total_Perms * (section_Ratio * index);
	//	printf("%d index %lld\n", index, perm_Start_Indecies[index]);
	//	fflush(stdout);
	}

	//Following section allocates memory on the host and on the device, and transfers the adjacency matrix the cities array, and the starting index array to the device
	cudaError_t problemo;

	long long* device_Perm_Start_Indecies;
	problemo = cudaMalloc((void**)&device_Perm_Start_Indecies, sizeof(long long) * (num_Blocks + 1));
	checkCuda(problemo,1);
	problemo = cudaMemcpy(device_Perm_Start_Indecies, perm_Start_Indecies, (sizeof(long long) * (num_Blocks + 1)), cudaMemcpyHostToDevice);
	checkCuda(problemo,2);

	int* adjacency_Matrix = generateArray(num_Cities);
	
/*	int foo = 0;
	for(foo; foo < (num_Cities * num_Cities); foo++)
	{
		printf("%d\t",adjacency_Matrix[foo]);
		if((foo + 1) % num_Cities == 0)
		{
			printf("\n");
		}
	}
*/	
	int* device_Matrix;
	problemo = cudaMalloc((void**)&device_Matrix, num_Cities*num_Cities*sizeof(int));
	checkCuda(problemo,3);
	problemo = cudaMemcpy(device_Matrix, adjacency_Matrix,num_Cities*num_Cities*sizeof(int),cudaMemcpyHostToDevice);
	checkCuda(problemo,4);

	int* device_Least_Cost_Array;
	problemo = cudaMalloc((void**)&device_Least_Cost_Array, total_Threads * sizeof(int));
	checkCuda(problemo,5);

	long long* device_Least_Path_Array;
	problemo = cudaMalloc((void**)&device_Least_Path_Array, total_Threads * sizeof(long long));
	checkCuda(problemo,6);

	short* cities = (short*)malloc(num_Cities * total_Threads * sizeof(short));

	for(index = 0; index < total_Threads; index++)//initializes the cities array with the appropriate values
	{
		int inner = 0;
		for(inner = 0; inner < num_Cities; inner++)
		{	
			cities[index * num_Cities + inner] = inner;
		}
	}

	short* device_Cities;
	problemo = cudaMalloc((void**)&device_Cities, num_Cities * total_Threads * sizeof(short));
	checkCuda(problemo,7);

	problemo = cudaMemcpy(device_Cities, cities, num_Cities * total_Threads * sizeof(short), cudaMemcpyHostToDevice);
	checkCuda(problemo,8);

	int* least_Cost_Array = (int*)malloc(total_Threads * sizeof(int));
	
	long long* least_Path_Array = (long long*)malloc(total_Threads * sizeof(long long));

	for(index = 0; index < total_Threads; index++)
	{
		least_Cost_Array[index] = 2;
		least_Path_Array[index] = 2;
	}

	//printf("fertig!1\n");
	//fflush(stdout);

	
	//kernel call//////////////////////////////////////////////////////////////////////////////////

	permute<<<num_Blocks,num_Threads>>>(device_Matrix, device_Cities, device_Perm_Start_Indecies, device_Least_Cost_Array, device_Least_Path_Array, num_Cities, num_Threads);
	checkCuda(cudaGetLastError(),13);


	//printf("fertig!2\n");
	//fflush(stdout);
	
	//retrieves the arrays storing the best results from each threas
	problemo = cudaMemcpy(least_Cost_Array, device_Least_Cost_Array,total_Threads * sizeof(int), cudaMemcpyDeviceToHost);
	checkCuda(problemo,9);
	problemo = cudaMemcpy(least_Path_Array, device_Least_Path_Array,total_Threads * sizeof(long long), cudaMemcpyDeviceToHost);
	checkCuda(problemo,10);	

	//initializes an int[] to store the cities of the best path
	int* true_Cities = (int*)malloc(num_Cities*sizeof(int));
	index = 0;
	for(index = 0; index < num_Cities; index++)
	{
		true_Cities[index] = index;
	}	

	int block_Index;
	int temp_Best = 99999999;
	int best_Index = 0;
	
	//calculates the best path of those returned by the GPU
	for(block_Index = 0; block_Index < total_Threads; block_Index++)
	{
		//printf("%d << leastCost element %d\n",least_Cost_Array[block_Index], block_Index);
		if(least_Cost_Array[block_Index] < temp_Best)
		{
			best_Index = block_Index;
			temp_Best = least_Cost_Array[block_Index];
		}
	}

	//displays the results
	printf("%d  << best! from thread %d\n",temp_Best, best_Index);
	orderPermutation(true_Cities, least_Path_Array[best_Index], num_Cities);
	index = 0;
	for(index = 0; index < num_Cities; index++)
	{
		printf("%d\t", true_Cities[index]);
	}

	printf("\nFinished!\n");
	//system("PAUSE");
	// Timing code adapted from: http://stackoverflow.com/questions/588307/c-obtaining-milliseconds-time-on-linux-clock-doesnt-seem-to-work-properl
	gettimeofday(&endTime, NULL);
	long timeDelta, startSeconds, startUSeconds, stopSeconds, stopUSeconds, startTotal, stopTotal;
	startSeconds = startTime.tv_sec;
	stopSeconds = endTime.tv_sec;
	startUSeconds = startTime.tv_usec;
	stopUSeconds = endTime.tv_usec;
	startTotal = (startSeconds * 1000) + (startUSeconds / 1000);
	stopTotal = (stopSeconds * 1000) + (stopUSeconds / 1000);


	timeDelta = stopTotal - startTotal;
	printf("Time: %d milliseconds\n",timeDelta);

}















