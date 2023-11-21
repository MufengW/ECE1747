#include "implementation.h"
#include "util_gpu_err_check.cuh"
#include <iostream>
#include "stdio.h"
#include <cuda_runtime.h>

#define MAX_THREADS_PER_BLOCK 256
#define BLOCKSZ (MAX_THREADS_PER_BLOCK * 2)
#define NUM_BANKS 16
#define LOG_NUM_BANKS 4
#define CONFLICT_FREE_OFFSET(n) ((n) >> NUM_BANKS + ((n) >> (2 * LOG_NUM_BANKS)))
#define MAX_SHARE_SIZE (BLOCKSZ + CONFLICT_FREE_OFFSET(BLOCKSZ - 1))

void printSubmissionInfo()
{
    // This will be published in the leaderboard on piazza
    // Please modify this field with something interesting
    char nick_name[] = "default-team";

    // Please fill in your information (for marking purposes only)
    char student_first_name[] = "John";
    char student_last_name[] = "Doe";
    char student_student_number[] = "00000000";

    // Printing out team information
    printf("*******************************************************************************************************\n");
    printf("Submission Information:\n");
    printf("\tnick_name: %s\n", nick_name);
    printf("\tstudent_first_name: %s\n", student_first_name);
    printf("\tstudent_last_name: %s\n", student_last_name);
    printf("\tstudent_student_number: %s\n", student_student_number);
}

__device__ int32_t device_newIdxWithBCAO(int32_t idx) {
    return idx + CONFLICT_FREE_OFFSET(idx);
}

void debug_print_list(int32_t* list, size_t size) {
    int32_t* h_list = new int32_t[size];
    cudaMemcpy(h_list, list, size * sizeof(int32_t), cudaMemcpyDeviceToHost);
    for (size_t i = 0; i < size; ++i) {
        printf("%u:%d ", i, h_list[i]);
    }
    std::cout << std::endl << std::endl;
    delete[] h_list;
}

__device__ void device_upSweep(int32_t *sharedData, int32_t threadId, int32_t totalLeaves, bool BCAO) {
    int32_t offset = 1;
    for (int32_t depth = totalLeaves >> 1; depth > 0; depth >>= 1) {
        if (threadId < depth) {
            int32_t leftIndex = offset * (2 * threadId + 1) - 1;
            int32_t rightIndex = offset * (2 * threadId + 2) - 1;

            if (BCAO) {
                sharedData[device_newIdxWithBCAO(rightIndex)] += sharedData[device_newIdxWithBCAO(leftIndex)];
            } else {
                sharedData[rightIndex] += sharedData[leftIndex];
            }
        }
        offset *= 2;
        __syncthreads();
    }
}

__device__ void device_downSweep(int32_t *sharedData, int32_t threadId, int32_t totalLeaves,
    int32_t *blockSums, int32_t blockId, bool BCAO) {

    for (unsigned int stride = totalLeaves >> 1; stride > 0; stride >>= 1) {
        int index = (threadId + 1) * stride * 2 - 1;
        if (index + stride < totalLeaves) {
            if (BCAO) {
                sharedData[device_newIdxWithBCAO(index + stride)] += sharedData[device_newIdxWithBCAO(index)];
            } else {
                sharedData[index + stride] += sharedData[index];
            }
        }
        __syncthreads();
    }
    if (threadId == 0) {
        int lastIdx = BCAO ? device_newIdxWithBCAO(totalLeaves - 1) : totalLeaves - 1;
        blockSums[blockId] = sharedData[lastIdx];
    }
}

__global__ void kernel_addPrefixSums(int32_t *prefixSum, int32_t *blockValues, size_t numElements)
{
    int threadId = threadIdx.x;
    int blockId = blockIdx.x;
    int blockOffset = (blockId+1) * BLOCKSZ;
    int leftIndex = threadId * 2 + blockOffset;
    int rightIndex = threadId * 2 + 1 + blockOffset;

    int32_t valueToAdd = blockValues[blockId];
    // Add the value to the elements of the prefix sum array
    if (leftIndex < numElements) {
        prefixSum[leftIndex] += valueToAdd;
    }
    if (rightIndex < numElements) {
        prefixSum[rightIndex] += valueToAdd;
    }
}

__global__ void kernel_parallelLargeScan(int32_t *data, int32_t *prefixSum, size_t N, int32_t *blockSums, bool BCAO) {

    __shared__ int32_t sharedPrefixSum[BLOCKSZ];
    int threadId = threadIdx.x;
    int blockId = blockIdx.x;
    int blockOffset = blockId * BLOCKSZ;
    int leafNum = BLOCKSZ;

    // Load data into shared memory
    if (BCAO) {
        int leftIndex = threadId;
        int rightIndex = threadId + (leafNum / 2);
        sharedPrefixSum[device_newIdxWithBCAO(leftIndex)] =
            (leftIndex + blockOffset < N) ? data[leftIndex + blockOffset] : 0;
        sharedPrefixSum[device_newIdxWithBCAO(rightIndex)] =
            (rightIndex + blockOffset < N) ? data[rightIndex + blockOffset] : 0;
    } else {
        sharedPrefixSum[threadId * 2] =
            (threadId * 2 + blockOffset < N) ? data[threadId * 2 + blockOffset] : 0;
        sharedPrefixSum[threadId * 2 + 1] =
            (threadId * 2 + 1 + blockOffset < N) ? data[threadId * 2 + 1 + blockOffset] : 0;
    }
    __syncthreads();

    device_upSweep(sharedPrefixSum, threadId, leafNum, BCAO);
    device_downSweep(sharedPrefixSum, threadId, leafNum, blockSums, blockId, BCAO);

    // Writing the results back to global memory
    if (BCAO) {
        if (threadId * 2 + blockOffset < N) {
            prefixSum[threadId * 2 + blockOffset] = sharedPrefixSum[device_newIdxWithBCAO(threadId * 2)];
        }
        if (threadId * 2 + 1 + blockOffset < N) {
            prefixSum[threadId * 2 + 1 + blockOffset] = sharedPrefixSum[device_newIdxWithBCAO(threadId * 2 + 1)];
        }
    } else {
        if (threadId * 2 + blockOffset < N) {
            prefixSum[threadId * 2 + blockOffset] = sharedPrefixSum[threadId * 2];
        }
        if (threadId * 2 + 1 + blockOffset < N) {
            prefixSum[threadId * 2 + 1 + blockOffset] = sharedPrefixSum[threadId * 2 + 1];
        }
    }
    __syncthreads();
}

void parallelPrefixSumLargeData(int32_t *d_data, int32_t *d_prefixSum, size_t size) {
    size_t blockNum = (size + BLOCKSZ - 1) / BLOCKSZ;

    int32_t *d_blockSums, *d_blockSumsPrefix;
    cudaMalloc(&d_blockSums, blockNum * sizeof(int32_t));
    cudaMalloc(&d_blockSumsPrefix, blockNum * sizeof(int32_t));

    // Bank Conflict Avoidance Optimization
    bool BCAO = false;
    kernel_parallelLargeScan<<<blockNum, MAX_THREADS_PER_BLOCK>>>(d_data, d_prefixSum, size, d_blockSums, BCAO);

    if (blockNum > 1) {
        parallelPrefixSumLargeData(d_blockSums, d_blockSumsPrefix, blockNum);
        kernel_addPrefixSums<<<blockNum-1, MAX_THREADS_PER_BLOCK>>>(d_prefixSum, d_blockSumsPrefix, size);
    }

    cudaFree(d_blockSums);
    cudaFree(d_blockSumsPrefix);
}

void parallelPrefixSumLargeData2(int32_t *d_data, int32_t *d_prefixSum, size_t size) {
    size_t blockNum = (size + BLOCKSZ - 1) / BLOCKSZ;

    int32_t *d_blockSums;
    cudaMalloc(&d_blockSums, blockNum * sizeof(int32_t));

    bool BCAO = false;

    // Step 1: Perform the scan on each block
    kernel_parallelLargeScan<<<blockNum, MAX_THREADS_PER_BLOCK>>>(d_data, d_prefixSum, size, d_blockSums, BCAO);
    // debug_print_list(d_prefixSum, size);

    // Handling the case when there are multiple blocks
    if (blockNum > 1) {
        // Step 2: Iteratively compute the scan of the block sums
        size_t currentSize = blockNum;
        int32_t *d_tempBlockSums, *d_swap;
        cudaMalloc(&d_tempBlockSums, blockNum * sizeof(int32_t));

        // Copy the initial block sums to the temporary array
        cudaMemcpy(d_tempBlockSums, d_blockSums, blockNum * sizeof(int32_t), cudaMemcpyDeviceToDevice);

        while (currentSize > 1) {
            size_t currentBlockNum = (currentSize + BLOCKSZ - 1) / BLOCKSZ;

            kernel_parallelLargeScan<<<currentBlockNum, MAX_THREADS_PER_BLOCK>>>(d_tempBlockSums, d_blockSums, currentSize, d_blockSums, BCAO);

            // Swap the pointers for the next iteration
            d_swap = d_tempBlockSums;
            d_tempBlockSums = d_blockSums;
            d_blockSums = d_swap;

            currentSize = currentBlockNum;
        }

        // Step 3: Add the scanned block sums to the prefix sum of each block
        // Be sure to use the correct array that holds the final scanned block sums
        kernel_addPrefixSums<<<blockNum - 1, MAX_THREADS_PER_BLOCK>>>(d_prefixSum, d_tempBlockSums, size);

        cudaFree(d_tempBlockSums);
    }

    cudaFree(d_blockSums);
}

void implementation(const int32_t *d_input, int32_t *d_output, size_t size) {
    // Perform the recursive scan
    parallelPrefixSumLargeData(const_cast<int32_t*>(d_input), d_output, size);
}
