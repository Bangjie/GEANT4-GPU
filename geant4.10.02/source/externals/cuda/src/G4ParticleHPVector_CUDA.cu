#include <cuda.h>
#include <cuda_runtime.h>
#include "G4ParticleHPVector_CUDA.h"

/***********************************************
*   CUDA functions
***********************************************/
__global__ void cudaTimes(double factor, G4ParticleHPDataPoint* cudaTheData, double* cudaTheIntegral) {
    int tid = blockIdx.x;
    cudaTheData[tid].xSec = cudaTheData[tid].xSec*factor;
    cudaTheIntegral[tid] = cudaTheIntegral[tid]*factor;
}

/***********************************************
*   Constructors, Setters
***********************************************/
G4ParticleHPVector_CUDA::G4ParticleHPVector_CUDA()      { }
G4ParticleHPVector_CUDA::G4ParticleHPVector_CUDA(int n) { }

G4ParticleHPVector_CUDA::~G4ParticleHPVector_CUDA() {
    if (cudaTheData) {
        cudaFree(cudaTheData);
    }
    if (cudaTheIntegral) {
       cudaFree(cudaTheIntegral);
    }
}

void G4ParticleHPVector_CUDA::SetNEntries(int * nEntriesPointer) {
    nEntries = nEntriesPointer;
}

void G4ParticleHPVector_CUDA::SetNPoints(int * nPointsPointer) {
    nPoints = nPointsPointer;
}

void G4ParticleHPVector_CUDA::SetTheData(G4ParticleHPDataPoint ** theDataPointer) {
    theData = theDataPointer;
}

void G4ParticleHPVector_CUDA::SetTheIntegral(double ** theIntegralPointer) {
    theIntegral = theIntegralPointer;
}

void G4ParticleHPVector_CUDA::SetTheDataChangedOnCpu() {
    theDataChangedOnCpuBool = true;
    theDataChangedOnGpuBool = false;
}

void G4ParticleHPVector_CUDA::SetTheDataChangedOnGpu() {
    theDataChangedOnCpuBool = false;
    theDataChangedOnGpuBool = true;
}

void G4ParticleHPVector_CUDA::CopyTheDataToGpuIfChanged() {
    if (theDataChangedOnCpuBool && theDataChangedOnGpuBool) {
        printf("BIG ERROR in CopyTheDataToGpuIfChanged: theDataChangedOnCpu and theDataChangedOnGpu are both true!\n\n");
        return;
    }

    if (theDataChangedOnCpuBool) {
        int theDataSize = *(nEntries) * sizeof(G4ParticleHPDataPoint);  
        if (cudaTheDataSize != theDataSize) {
            if (cudaTheDataSize != 0) {
                cudaFree(cudaTheData);
            }
            cudaMalloc(&cudaTheData, theDataSize);
            cudaTheDataSize = theDataSize;
        }
        cudaMemcpy(cudaTheData, *(theData), theDataSize, cudaMemcpyHostToDevice);
        
        theDataChangedOnCpuBool = false;
        theDataChangedOnGpuBool = false;
    }
}

void G4ParticleHPVector_CUDA::CopyTheDataToCpuIfChanged() {
    if (theDataChangedOnCpuBool && theDataChangedOnGpuBool) {
        printf("BIG ERROR in CopyTheDataToCpuIfChanged: theDataChangedOnCpu and theDataChangedOnGpu are both true!\n\n");
        return;
    }

    if (theDataChangedOnGpuBool) {
        // gpu never changes size of theData, so just copy it over
        int theDataSize = *(nEntries) * sizeof(G4ParticleHPDataPoint);  
        cudaMemcpy(*(theData), cudaTheData, theDataSize, cudaMemcpyDeviceToHost);
        
        theDataChangedOnCpuBool = false;
        theDataChangedOnGpuBool = false;
    }
}

/***********************************************
*   Ported Functions
***********************************************/
void G4ParticleHPVector_CUDA::Times(double factor) {
    CopyTheDataToGpuIfChanged();

    int theIntegralSize = *(nEntries) * sizeof(double);
    cudaMalloc(&cudaTheIntegral, theIntegralSize);    
    cudaMemcpy(cudaTheIntegral, *(theIntegral), theIntegralSize, cudaMemcpyHostToDevice);
    
    cudaTimes<<<*(nEntries),1>>>(factor, cudaTheData, cudaTheIntegral);
    SetTheDataChangedOnGpu();

    cudaMemcpy(*(theIntegral), cudaTheIntegral, theIntegralSize, cudaMemcpyDeviceToHost);
}

double G4ParticleHPVector_CUDA::GetXsec(double e) {
    printf("\nGetXsec");
    return -1;
}
