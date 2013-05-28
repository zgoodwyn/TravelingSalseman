#include<iostream>
#include<fstream>	
#include<string>	
#include<sstream>//wherever a stringstream pops up, it is to convert a multi-digit number to a string or vice versa
#include"math.h"
#include<stdlib.h>
#include <omp.h>


#include<algorithm>//needed for next_permutation 
#include<climits>//needed for MAX_INT   
const int MAX_PATH = 1000;//the highest cost of a path in the graph
void convertFact(unsigned long long, int*);
void orderPermutation(int*, unsigned long long, int);
unsigned long long factorial(unsigned long long);

using namespace std;
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
//converts a number into base factorial
//num: the number to convert to base factorial
//digits: a storage array to store the digits of the base factorial number
//numbers are stored in reverse order (so the 2nd digit is in position 1, the third in 2, etc.
//digits[0] will contain the length of the number in digits, since the first number of a base factorial number is always 0
void convertFact(unsigned long long num, int* digits)
{
	int numDigits = 1;//there is always a spare 0 in these numbers
	while(num > 0)
	{
		digits[numDigits] = num % (numDigits + 1);
		numDigits++;
		num /= numDigits;
	}
	digits[0] = numDigits;//because the first digit is always zero, we will store the length of the array in the 0th slot
}
//returns the array transposed to the nth permutation after the given ordering
//toOrder: the set for which you would like to define the nth permutation
//m: the numbered permutation to be stored in toOrder
//size: the size of the array toOrder
//warning: gives unpredictable behavior if n is > the number of unique permutations for toOrder
void orderPermutation(int* toOrder, unsigned long long n, int size)
{
	int swaps[100];
	convertFact(n, swaps);
	int location = size - swaps[0];//accounts for leading zeros
	int loc = swaps[0] - 1;//used to iterate through the digits of the factoradic number
	while(loc > 0)
	{
		int temp = toOrder[location + swaps[loc]];
		for(int x = location+swaps[loc]; x > location; x--)//moves all the other numbers up
		{
			toOrder[x] = toOrder[x - 1];
		}
		toOrder[location] = temp;
		location++;
		loc--;
	}
	
}

//main takes 4 arguments: the program name, # of cities, a 0 or 1 (0= read data from file, 1=use random number generator), finally the filename (if 0) or a seed (if 1)

int main(int argc, char* argv[])

{

	//cout << argc << endl

	

	if(argc != 4)//if an improper number of parameters were passed

	{
		cout <<"Error: improper number of commands" << endl;
		cout <<"arguments: #cities 0 \"filename\""<< endl;
		cout << "or: #cities 1 (seed)" << endl;
	}

	stringstream arg1(argv[1]);
	int num_Cities;
	arg1 >> num_Cities;

	//the costs of each path will be stored in an adjacency matrix

	int** adjacency_Matrix = new int*[num_Cities];

	for(int index = 0; index < num_Cities; index++)
	{
		adjacency_Matrix[index] = new int[num_Cities];
	}


	if(strcmp(argv[2],"1") == 0)//if using random number generator
	{
		stringstream arg3(argv[3]);

		unsigned int seeder;

		arg3 >> seeder;

		srand(seeder);
		
		//fills an array with randomly generated number
		for(int outer = 0; outer < num_Cities; outer++)
		{

			for(int inner = 0; inner <= outer; inner++)
			{

				adjacency_Matrix[outer][inner] = 1 + rand() % MAX_PATH;

				adjacency_Matrix[inner][outer] = adjacency_Matrix[outer][inner];
			}

		}

	}
	else//if reading from a file
	{
		//string file_Name;
		int temp_Number_Array[200];

		for(int index = 0; index < 200; index++)
		{
			temp_Number_Array[index] = 0;
		}

		char* temp_Input;
	
		//cout<<"enter the file for the adjacency matrix you would like to use"<<endl;

		//cin>>file_Name;
	
		ifstream matrix_File;
	
		//matrix_File.open(file_Name.c_str());

		matrix_File.open(argv[3]);

		if(!matrix_File)
		{
			cout<<"error"<<endl;
		}

		int counter = 0;

		//reads each number in the file and stores into a temporary array
		while(matrix_File)
		{	
			matrix_File>>temp_Number_Array[counter];	
			counter++;
		}
	
		matrix_File.close();

		double total_Num = double(counter);	
	
		int increment = 0;

		//takes the numbers read into the file in puts them into a 2d adjacency matrix
		for(int row = 0; row < num_Cities; row++)
		{	
			for(int column = 0; column < num_Cities; column++)
			{	
				adjacency_Matrix[row][column] = temp_Number_Array[increment];	
				increment++;
			}	
		}

	}
	unsigned long long total_Perms;

	total_Perms = factorial(num_Cities);

	int num_Threads, t_Num;
	#pragma omp parallel
	{
		num_Threads = omp_get_num_threads(); 
	}

	double section_Ratio = double(1)/num_Threads;

	long long* perm_Start_Indecies = new long long[num_Threads];

	for(int index = 0; index < num_Threads +1; index++)
	{
		perm_Start_Indecies[index] = total_Perms * (section_Ratio * index);
		
	}

	//system("PAUSE");

	//writes the adjacency matrix used to and output file called TSPout.txt
	//for(int outer = 0; outer < num_Cities; outer++)
	//{

	///	for(int inner = 0; inner < num_Cities; inner++)
	//	{
	//		outfile<<adjacency_Matrix[outer][inner]<<"\t";
	//	}

	//	outfile<<endl<<endl;
	//}

	//system("PAUSE");
	int* cities = new int[num_Cities];//creates an array that will contain the numeric ID's of the cities, 0 to (n-1)

	for(int index = 0; index < num_Cities; index++)
	{
		cities[index] = index;
	}

	//orderPermutation(cities,numPerm,num_Cities);

	for(int index = 0; index < num_Cities; index++)
	{
		cout << cities[index] << ",";
	}
	cout << endl;

	int leastSolution = INT_MAX;

	string shortestPath = "";
	ofstream outfile;
	outfile.open("TSPout.txt");

	
	//starts the parallel computations of the permutations
	//declares private variables to be used by individual cores
	#pragma omp parallel
	{
		int permutation = perm_Start_Indecies[omp_get_thread_num()];
		int Thread_ID;
		int totSum;
		int* cities = new int[num_Cities];
		for(int index = 0; index < num_Cities; index++)
		{
			cities[index] = index;
		}
		
		//declares where each core starts their respective permuatations
#pragma omp critical
		{
			orderPermutation(cities,perm_Start_Indecies[omp_get_thread_num()],num_Cities);
			Thread_ID = omp_get_thread_num();
		}
		//for loop that runs each core through their determined subset of permutations
		for(int index = perm_Start_Indecies[Thread_ID]; index < (perm_Start_Indecies[Thread_ID + 1]); index++){
			totSum = 0;
			stringstream possiblePath;
			//cout<<Thread_ID<<" Thread_ID "<<perm_Start_Indecies[Thread_ID]<<" perm start index"<<index <<" index"<<endl;
			//system("PAUSE");
			for(int i = 0; i < num_Cities; i++)
			{

				possiblePath << cities[i] << ",";//assempbles the "possiblePath" stringstream by concatonating the city IDs

				totSum += adjacency_Matrix[cities[i]][cities[(i+1) % num_Cities]];
				//when i=numCities-1, the next step is returning to the initial city. (i+1)%num_Cities returns 0 at this point, where cities[0] is the origin city
			}

			possiblePath << cities[0];
#pragma omp critical
			{
				
					//outfile << permutation << ": " <<  possiblePath.str() << ": " << totSum << endl;
				
			}
			//if this solution is better than the previous best, then update the best solution path and its length

			if(totSum < leastSolution)
			{
			//needs to be critical so multiple threads dont try to access it at the same time
#pragma omp critical
				{
				leastSolution = totSum;
				shortestPath = possiblePath.str();
				}
			}

			permutation++;
		
#pragma omp critical 
			{
				next_permutation(cities, cities + num_Cities);
				//orderPermutation(cities,index,num_Cities);
			}

		}//while(next_permutation(cities, cities + num_Cities));//orders the array so that it contains the next permutation, retrns false when it has hit the last permutation
	}


	outfile << "Shortest path: " << shortestPath << " with length: " << leastSolution << endl;

	outfile.close();
	

	cout << "Finished!" << endl;
	//system("PAUSE");
}

