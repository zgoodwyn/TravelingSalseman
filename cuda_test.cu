#include<stdlib.h>
#include<stdio.h>
#include<cuda.h>
#include<cuda_runtime.h>//tehnically unnecessary

//indexes "threadIdx.x" elements into the array, adds threadId.x to it (effectively doubling it), then adding 1
__global__ void multiGo(float* arr)
{
	arr[threadIdx.x] += threadIdx.x + 1;
}

int main()
{
	int N = 5;
	size_t size = N * sizeof(float);//the size in bytes of all data transfers, used in cuda-specific calls and mallocs

	float* h_A = (float*)malloc(size);
	float* h_B = (float*)malloc(size);

	int index = 0;

	//fills h_A with the numbers 0-(n-1)
	for(index = 0; index < N; index = index + 1)
	{
		h_A[index] = index;
		printf("%f\n",h_A[index]);
	}

	float* d_A;
	cudaError_t problemo;//used for error checking, optional (cudamalloc returns a cudaError_t)
	problemo = cudaMalloc((void**)&d_A, size);
	if(problemo != cudaSuccess)
	{
		printf("%s\n", cudaGetErrorString(problemo));
	}
	//copies the contents of h_A (on the host) into d_A (on the device/GPU)
	cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);

	int id = 0;
	//initializes elements of h_B to -1
	for(id = 0; id < N; id = id + 1)
	{
		h_B[id] = -1;
		printf("%f\n",h_B[id]);
	}
	
	multiGo<<<1,5>>>(d_A);//Cuda function call
	
	//copies the cuda memory back into the h_B on the device
	cudaMemcpy(h_B, d_A, size, cudaMemcpyDeviceToHost);

	//prints out the elements of B
	int index_Two = 0;
	for(index_Two = 0; index_Two < N; index_Two = index_Two + 1)
	{
		printf("%f\n", h_B[index_Two]);
	}

	free(h_A);
	cudaFree(d_A);

}
