
#include<iostream>
#include<fstream>
#include<string.h>
#include<sstream>//wherever a stringstream pops up, it is to convert a multi-digit number to a string or vice versa
#include"math.h"
#include<stdlib.h>
#include <omp.h>
#include<time.h>

//#include<sys/time.h> // Used for timing this.
//#include<unistd.h> // Used for timing this.

#include<algorithm>//needed for next_permutation
#include<climits>//needed for MAX_INT
const int MAX_PATH = 1000;//the highest cost of a path in the graph
void convertFact(unsigned long long, int*);
void orderPermutation(int*, unsigned long long, int);
unsigned long long factorial(unsigned long long);

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template<class _Ty1, class _Ty2> inline
	bool prayer(const _Ty1& _Left, const _Ty2& _Right)
{	// test if _Left < _Right and operator< is strict weak ordering
	//	cout <<"!!" << endl;
	if (!(_Left < _Right))
		return (false);
	else if (_Right < _Left)
		cout << "help us" << endl;
	//_DEBUG_ERROR2("invalid operator<", _File, _Line);
	return (true);
}

inline void swap(int* a, int* b)
{
	//cout <<"swap!!" << endl;
	int temp = *a;
	*a = *b;
	*b = temp;
}

inline void reverse(int* a, int* b)
{
	//cout <<"reverse!!" << endl;
	b--;
	while(a < b)
	{
		swap(a,b);
		a++;
		b--;
		//cout << "swapping: " << *a << " with " << *b << endl;
	}
}

inline bool nextPerm(int* _First, int* _Last)
{	// permute and test for pure ascending, using operator<
	int* _Next = _Last;
	if (_First == _Last || _First == --_Next)
		return (false);

	for (; ; )
	{	// find rightmost element smaller than successor
		//	cout <<"!!" << endl;
		int* _Next1 = _Next;
		if (prayer(*--_Next, *_Next1))
		{	// swap with rightmost element that's smaller, flip suffix
			int* _Mid = _Last;
			for (; !prayer(*_Next, *--_Mid); )
				;
			swap(_Next, _Mid);
			reverse(_Next1, _Last);
			return (true);
		}

		if (_Next == _First)
		{	// pure descending, flip all
			reverse(_First, _Last);
			return (false);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




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


int main(int argc, char* argv[])
{

	//struct timeval startTime, endTime;
	//gettimeofday(&startTime, NULL);

	if(argc != 3)
	{
		cout<<"You have enetered and invalid argument list"<<endl;
		cout<<"Required, (file for graph) (number of threads)"<<endl;
	}

	string filename = argv[1];

	stringstream arg1(argv[2]);
	int numThreads;
	arg1 >> numThreads;

	int maxWeight, numItems, usedWeight;  
	// initialize arrays to numItems

	ifstream file;
	file.open(filename);

	int numTrials;
	file >> numTrials;

	file >> numItems;
	file >> maxWeight;

	    // take in this round's values and weights
	int* values = new int[numItems];
	int* weights = new int[numItems];

	for(int index = 0; index < numItems; index++)
	{
		file >> values[index];
	}
	for(int index = 0; index < numItems; index++)
	{
		file >> weights[index];
	}



	
	long long* permStartIndecies = new long long[numThreads + 1];

	long long numPerms = factorial(numItems);

	double sectionRatio = double(1)/numThreads;

	for(int index = 0; index < numThreads + 1; index++)
	{
		permStartIndecies[index] = numPerms * (sectionRatio * index);
	}

	long long* bestPermArray = new long long[numThreads];
	int* bestValueArray = new int[numThreads];


	////entering the parallel section
	#pragma omp parallel num_threads(numThreads)
	{
		int bestValue = 0;
		long long bestPerm = LLONG_MAX;

		int Thread_ID = omp_get_thread_num();
		int totSum;
		int totWeight;

		int* itemPermArray = new int[numItems];
		for(int index = 0; index < numItems; index++)
		{
			itemPermArray[index] = index;
		}

		orderPermutation(itemPermArray,permStartIndecies[Thread_ID],numItems);


		for(long long index = permStartIndecies[Thread_ID]; index < (permStartIndecies[Thread_ID + 1]); index++)
		{
			
			totSum = 0;
			totWeight = 0;
			int i = 0;
			for(int w = 0; w < numItems; w++)
			{

				totWeight = totWeight + weights[itemPermArray[w]];
				if(totWeight > maxWeight)
				{
					break;
				}
				totSum = totSum + values[itemPermArray[w]];
				
				i++;
			}

			
			if(totSum > bestValue)
			{
				bestValue = totSum;
				bestPerm = index;
		
			}

		
			nextPerm(itemPermArray, itemPermArray + numItems);
			
		
		}

		bestValueArray[Thread_ID] = bestValue;
		bestPermArray[Thread_ID] = bestPerm;

	}

	int finalValue;
	unsigned long long finalPerm = 0;
	int tempValue = 0;
	for(int index = 0; index < numThreads; index++)
	{
		if(bestValueArray[index] > tempValue)
		{
			tempValue = bestValueArray[index];
			finalPerm = bestPermArray[index];
		}
	}
	finalValue = tempValue;

	int* permArray = new int[numItems];
	for(int index = 0; index < numItems; index++)
	{
		permArray[index] = index;
	}

	orderPermutation(permArray, finalPerm, numItems);
	
	for(int index = 0; index < numItems; index++)
	{
		cout <<  permArray[index]<< ", ";
	}

	cout<< "best value for theft: "<< finalValue<<endl;
	

	/*gettimeofday(&endTime, NULL);
	long timeDelta, startSeconds, startUSeconds, stopSeconds, stopUSeconds, startTotal, stopTotal;
	startSeconds = startTime.tv_sec;
	stopSeconds = endTime.tv_sec;
	startUSeconds = startTime.tv_usec;
	stopUSeconds = endTime.tv_usec;
	startTotal = (startSeconds * 1000) + (startUSeconds / 1000);
	stopTotal = (stopSeconds * 1000) + (stopUSeconds / 1000);


	timeDelta = stopTotal - startTotal;
	cout << "Time: " << timeDelta << " milliseconds" << endl;
	*/

	
}