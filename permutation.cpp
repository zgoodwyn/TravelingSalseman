#include<iostream>

using namespace std;

<<<<<<< HEAD
=======
const int numCities = 4;
>>>>>>> 508d87361e50f1e791b6781cc91983be9e0f491b

void convertFact(unsigned long long, int*);
void orderPermutation(int*, unsigned long long, int);
unsigned long long factorial(unsigned long long);

int main()
{
<<<<<<< HEAD
	int numCities;
	int numCores;

	cout << "Enter number of cities" << endl;
	cin >> numCities;
	cout << "Enter number of cores" << endl;
	cin >> numCores;
	unsigned long long totPaths = factorial(numCities);

	for(int q = 0; q < numCores; q++)
	{
		//system("PAUSE");
		int* nums = new int[numCities];
=======
	for(int q = 0; q < factorial(numCities); q++)
	{
		//system("PAUSE");
		int nums[numCities];
>>>>>>> 508d87361e50f1e791b6781cc91983be9e0f491b
		for(int t = 0; t < numCities; t++)
		{
			nums[t] = t;
		}
		int digits[100];
		cout << q << ": ";
		convertFact(q, digits);
		for(int y = digits[0] - 1; y > 0; y--)
		{
			cout << digits[y] << ",";
		}
		cout << "0\t\t";
<<<<<<< HEAD
		orderPermutation(nums,totPaths / numCores * q,numCities);
=======
		orderPermutation(nums,q,numCities);
>>>>>>> 508d87361e50f1e791b6781cc91983be9e0f491b
		for(int z = 0; z < numCities; z++)
		{
			cout << nums[z] << ",";
		}
		cout << "\b " << endl;
<<<<<<< HEAD
		delete [] nums;
=======
>>>>>>> 508d87361e50f1e791b6781cc91983be9e0f491b
	}
}

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
	digits[0] = numDigits;
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
//	cout << endl << "size: " << size << "\tswaps[0]: " << swaps[0] << endl; 
	int loc = swaps[0] - 1;
	while(loc > 0)
	{
//		cout << "loc location swaps[loc]: " << loc << "," << location << "," << swaps[loc] << endl;
		int temp = toOrder[location + swaps[loc]];
//		cout << "temp: " << temp << endl;
		for(int x = location+swaps[loc]; x > location; x--)//moves all the other numbers up
		{
			toOrder[x] = toOrder[x - 1];
		}
		toOrder[location] = temp;
		location++;
		loc--;
//		cout << endl << endl;
	}
	
}