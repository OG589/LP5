#include<iostream>
#include<bits/stdc++.h>
#include<cuda.h>
#define BLOCK_SIZE 16

using namespace std;



void initialize_matrix(int *array, int rows, int cols){
    for(int i = 0 ; i < rows; i++){
        for(int j = 0; j < cols; j++){
            array[i*cols + j] = rand() % 10;
        }
    }
}

void print_matrix(int *array, int rows, int cols){
    for(int i = 0 ; i < rows; i++){
        for(int j = 0; j < cols; j++){
            cout << array[i*cols + j] << " ";
        }
        cout << endl;
    }
}

void matrix_multiplication_cpu(int *a, int *b, int *c, int common, int c_rows,int c_cols){
    for(int i = 0; i < c_rows; i++){
        for(int j = 0; j < c_cols; j++){
            int sum = 0;
            for(int k = 0; k < common; k++){
                sum += a[i*common + k] * b[k*c_cols + j];
            }
            c[i*c_cols + j] = sum;
        }
    }
}



__global__ void matrix_multiply(int *a, int *b, int *c, int c_rows, int common, int c_cols)
{
    int row = blockIdx.y*blockDim.y + threadIdx.y;
    int col = blockIdx.x*blockDim.x + threadIdx.x;
    int sum=0;
   
    if(col < c_cols && row < c_rows) {
      for(int j = 0 ;j < common;j++)
      {
          sum += a[row*common+j] * b[j*c_cols+col];
      }
      c[c_cols*row+col]=sum;
    }
    
}


int main(){

    int A_rows, A_cols, B_rows, B_cols, C_rows, C_cols;
    cout << "Dimensions of matrix 1:\n";
    cout << "Rows: ";
    cin >> A_rows;
    cout << "Columns: ";
    cin >> A_cols;
    cout << "Dimensions of matrix 2:\n";
    cout << "Rows: " << A_cols << endl << "Columns: ";
    cin >> B_cols;
    B_rows = A_cols;
    C_rows = A_rows;
    C_cols = B_cols;

    int A_size = A_rows * A_cols;
    int B_size = B_rows * B_cols;
    int C_size = C_rows * C_cols;

    int *A, *B, *C;
    int *m1,*m2,*result;

    A = new int[A_size];
    B = new int[B_size];
    C = new int[C_size];

    initialize_matrix(A,A_rows,A_cols);
    cout << "Matrix 1\n";
    print_matrix(A,A_rows,A_cols);
    initialize_matrix(B,B_rows,B_cols);
    cout << "Matrix 2\n";
    print_matrix(B,B_rows,B_cols);

    cudaMallocManaged(&m1, A_size * sizeof(int));
    cudaMallocManaged(&m2, B_size * sizeof(int));
    cudaMallocManaged(&result, C_size * sizeof(int));

    cudaMemcpy(m1,A,A_size * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(m2,B,B_size * sizeof(int), cudaMemcpyHostToDevice);

    dim3 dimGrid(A_rows + BLOCK_SIZE  - 1 / BLOCK_SIZE, B_cols + BLOCK_SIZE - 1 / BLOCK_SIZE);
    dim3 dimBlock(BLOCK_SIZE,BLOCK_SIZE);

    float gpu_elapsed_time;
    cudaEvent_t gpu_start,gpu_stop;

    cudaEventCreate(&gpu_start);
    cudaEventCreate(&gpu_stop);
    cudaEventRecord(gpu_start);
    matrix_multiply<<<dimGrid,dimBlock>>>(m1,m2,result,C_rows,A_cols,C_cols);
    cudaEventRecord(gpu_stop);
    cudaEventSynchronize(gpu_stop);
    cudaEventElapsedTime(&gpu_elapsed_time, gpu_start, gpu_stop);
    cudaEventDestroy(gpu_start);
    cudaEventDestroy(gpu_stop);

    cudaMemcpy(C,result,C_size*sizeof(int),cudaMemcpyDeviceToHost);
    cout << "GPU result:\n";
    print_matrix(C,C_rows,C_cols);
    cout<<"GPU Elapsed time is: "<<gpu_elapsed_time<<" milliseconds"<<endl;

    cudaEventCreate(&gpu_start);
    cudaEventCreate(&gpu_stop);
    cudaEventRecord(gpu_start);
    matrix_multiplication_cpu(A,B,C,A_cols,C_rows,C_cols);
    cudaEventRecord(gpu_stop);
    cudaEventSynchronize(gpu_stop);
    cudaEventElapsedTime(&gpu_elapsed_time, gpu_start, gpu_stop);
    cudaEventDestroy(gpu_start);
    cudaEventDestroy(gpu_stop);

    cout << "CPU result:\n";
    print_matrix(C,C_rows,C_cols);
    cout<<"CPU Elapsed time is: "<<gpu_elapsed_time<<" milliseconds"<<endl;

    cudaFree(m1);
    cudaFree(m2);
    cudaFree(result);

    return 0;
}






###COMMANDS###

✅ 1. Check if your system supports CUDA

Run:

nvidia-smi
✔ If you see GPU info → good
❌ If command not found → CUDA not installed / no NVIDIA GPU
✅ 2. Install CUDA Toolkit (if not installed)
sudo apt update
sudo apt install nvidia-cuda-toolkit

Check installation:

nvcc --version
✅ 3. Save your file
nano matrix.cu

Paste your code → Save

✅ 4. Compile using nvcc (NOT g++)
nvcc matrix.cu -o matrix
✅ 5. Run the program
./matrix
⚠️ IMPORTANT BUGS in your code (fix these before running)

Your code has 2 critical issues that can give wrong results:

❌ 1. Grid dimension calculation is WRONG

Your line:

dim3 dimGrid(A_rows + BLOCK_SIZE  - 1 / BLOCK_SIZE, B_cols + BLOCK_SIZE - 1 / BLOCK_SIZE);

👉 Problem: Operator precedence → division happens first

✅ Fix:
dim3 dimGrid((B_cols + BLOCK_SIZE - 1) / BLOCK_SIZE,
             (C_rows + BLOCK_SIZE - 1) / BLOCK_SIZE);
❌ 2. You are mixing Unified Memory + cudaMemcpy

You used:

cudaMallocManaged(...)

👉 That means NO need for cudaMemcpy

✅ Fix: Remove these lines
cudaMemcpy(m1,A,...)
cudaMemcpy(m2,B,...)
cudaMemcpy(C,result,...)
Instead:
for(int i = 0; i < A_size; i++) m1[i] = A[i];
for(int i = 0; i < B_size; i++) m2[i] = B[i];

cudaDeviceSynchronize();  // after kernel

for(int i = 0; i < C_size; i++) C[i] = result[i];
❌ 3. Missing synchronization after kernel

Add this after kernel launch:

matrix_multiply<<<dimGrid,dimBlock>>>(...);
cudaDeviceSynchronize();
⚠️ 4. CPU timing mistake

You're using CUDA events for CPU timing ❌

👉 Instead use:

auto start = chrono::high_resolution_clock::now();
matrix_multiplication_cpu(...);
auto end = chrono::high_resolution_clock::now();
💡 Final Compile + Run
nvcc matrix.cu -o matrix
./matrix
