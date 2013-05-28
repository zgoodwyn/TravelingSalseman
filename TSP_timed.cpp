#include<iostream>

#include<fstream>

#include<string>

#include<sstream>//wherever a stringstream pops up, it is to convert a multi-digit number to a string or vice versa

#include"math.h"

#include<stdlib.h>

#include<algorithm>//needed for next_permutation

#include<climits>//needed for MAX_INT



const int MAX_PATH = 10;//the highest cost of a path in the graph



using namespace std;



//main takes 4 arguments: the program name, # of cities, a 0 or 1 (0= read data from file, 1=use random number generator), finally the filename (if 0) or a seed (if 1)

int main(int argc, char* argv[])

{

	//cout << argc << endl;

	if(argc != 4)

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

		while(matrix_File)

		{

	

			matrix_File>>temp_Number_Array[counter];

	

			counter++;

		}

	

		matrix_File.close();

		double total_Num = double(counter);

	

	

		int increment = 0;

		for(int row = 0; row < num_Cities; row++)

		{

	

			for(int column = 0; column < num_Cities; column++)

			{

	

				adjacency_Matrix[row][column] = temp_Number_Array[increment];

	

				increment++;

			}

	

		}

	}



	ofstream outfile;



	outfile.open("TSPout.txt");



	for(int outer = 0; outer < num_Cities; outer++)

	{

		for(int inner = 0; inner < num_Cities; inner++)

		{

			outfile<<adjacency_Matrix[outer][inner]<<"\t";



		}

		outfile<<endl<<endl;



	}



	//system("PAUSE");



	int* cities = new int[num_Cities];//creates an array that will contain the numeric ID's of the cities, 0 to (n-1)

	for(int index = 0; index < num_Cities; index++)

	{

		cities[index] = index;



	}



	int leastSolution = INT_MAX;

	string shortestPath = "";

	int permutation = 0;



	do{

		int totSum = 0;

		stringstream possiblePath;

		for(int i = 0; i < num_Cities; i++)

		{

			possiblePath << cities[i] << ",";//assempbles the "possiblePath" stringstream by concatonating the city IDs

			totSum += adjacency_Matrix[cities[i]][cities[(i+1) % num_Cities]];

			//when i=numCities-1, the next step is returning to the initial city. (i+1)%num_Cities returns 0 at this point, where cities[0] is the origin city

		}

		possiblePath << cities[0];

		outfile << permutation << ": " <<  possiblePath.str() << ": " << totSum << endl;



		//if this solution is better than the previous best, then update the best solution path and its length

		if(totSum < leastSolution)

		{

			leastSolution = totSum;

			shortestPath = possiblePath.str();

		}

		permutation++;

	}while(next_permutation(cities, cities + num_Cities));//orders the array so that it contains the next permutation, retrns false when it has hit the last permutation



	outfile << "Shortest path: " << shortestPath << " with length: " << leastSolution << endl;

	outfile.close();



	cout << "Finished!" << endl;

	//system("PAUSE");



}

