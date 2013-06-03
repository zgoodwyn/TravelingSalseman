#include<stdlib.h>
#include<stdio.h>
#include<cuda.h>
#include<cuda_runtime.h>

__global__ void multiGo(float* arr)
{
	arr[threadIdx.x] += threadIdx.x + 1;
}

int main()
{
	int N = 5;
	size_t size = N * sizeof(float);

	float* h_A = (float*)malloc(size);
	float* h_B = (float*)malloc(size);

	int index = 0;

	for(index = 0; index < N; index = index + 1)
	{
		h_A[index] = index;
		printf("%f\n",h_A[index]);
	}

	float* d_A;
	cudaError_t problemo;
	printf("here\n");
	fflush(stdout);
	problemo = cudaMalloc((void**)&d_A, size);
	if(problemo != cudaSuccess)
	{
		printf("%s\n", cudaGetErrorString(problemo));
	}

	cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);

	int id = 0;
	for(id = 0; id < N; id = id + 1)
	{
		h_B[id] = -1;
		printf("%f\n",h_B[id]);
	}
	
	multiGo<<<1,5>>>(d_A);
	
	cudaMemcpy(h_B, d_A, size, cudaMemcpyDeviceToHost);

	int index_Two = 0;
	for(index_Two = 0; index_Two < N; index_Two = index_Two + 1)
	{
		printf("%f\n", h_B[index_Two]);
	}

	free(h_A);
	cudaFree(d_A);

}
