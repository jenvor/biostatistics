/*	
	Copyright 2012 The Trustees of Indiana University.  All rights reserved.
	CGL MapReduce Framework on GPUs and CPUs
	Code Name: Panda 0.1
	File: Panda.h 
	Time: 2012-07-01 
	Developer: Hui Li (lihui@indiana.edu)

	This is the source code for Panda, a MapReduce runtime on GPUs and CPUs.
*/

/*
#ifdef WIN32 
#include <windows.h> 
#endif 
#include <pthread.h>
*/

#ifndef __PANDA_H__
#define __PANDA_H__

//#include <unistd.h>
//#include <sys/mman.h>

//#include <cutil.h>
//#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>

//helper for shared that are common to CUDA SDK samples
//#include <shrUtils.h>
//#include <sdkHelper.h>  
//#include <shrQATest.h>  


#define _DEBUG 0x01
#define CEIL(n,m) (n/m + (int)(n%m !=0))
#define THREAD_CONF(grid, block, gridBound, blockBound) do {\
	    block.x = blockBound;\
	    grid.x = gridBound; \
		if (grid.x > 65535) {\
		   grid.x = (int)sqrt((double)grid.x);\
		   grid.y = CEIL(gridBound, grid.x); \
		}\
	}while (0)

#define THREADS_PER_BLOCK (blockDim.x*blockDim.y)
#define BLOCK_ID	(gridDim.y	* blockIdx.x  + blockIdx.y)
#define THREAD_ID	(blockDim.x	* threadIdx.y + threadIdx.x)
#define TID (BLOCK_ID * THREADS_PER_BLOCK + THREAD_ID)
//#define TID (BLOCK_ID * blockDim.x + THREAD_ID)

//NOTE NUM_THREADS*NUM_BLOCKS > STRIDE !
#define NUM_THREADS	128
#define NUM_BLOCKS	4
#define STRIDE	32


#define checkCudaErrors(err)  __checkCudaErrors (err, __FILE__, __LINE__)

extern "C"
void __checkCudaErrors(cudaError err, const char *file, const int line );


//used for unsorted values
typedef struct
{
   void *key;
   void *val;
   int keySize;
   int valSize;
} keyval_t;

typedef struct
{
   int keySize;
   int valSize;
   int keyPos;
   int valPos;
} keyval_pos_t;


typedef struct
{
   int valSize;
   int valPos;
} val_pos_t;

typedef struct
{
   int keySize;
   int keyPos;

   int val_arr_len;
   val_pos_t * val_pos_arr;
} sorted_keyval_pos_t;


//two direction - bounded share buffer
//  from left to right  key val buffer
// from right to left  keyval_t buffer
typedef struct
{
   void *buff;
   int *total_arr_len;

   int buff_len;
   int buff_pos;

   //int keyval_pos;
   int arr_len;
   keyval_t *arr;

} keyval_arr_t;

//used for sorted or partial sorted values
typedef struct
{
   void * val;
   int valSize;
} val_t;

typedef struct
{
   void * key;
   int keySize;
   int val_arr_len;
   val_t * vals;
} keyvals_t;


typedef struct 
{
	int num_input_record;
	keyval_t * input_keyval_arr;
	int num_mappers;
	int num_reducers;
	int num_gpus;
	int num_cpus;

}job_configuration;


typedef struct 
{	
  bool configured;

  int num_mappers;
  int num_reducers;

  void *d_input_keys_shared_buff;
  void *d_input_vals_shared_buff;
  keyval_pos_t *d_input_keyval_pos_arr;

  //data for input results
  int num_input_record;

  keyval_t * h_input_keyval_arr;
  keyval_t * d_input_keyval_arr;

  //data for intermediate results
  int *d_intermediate_keyval_total_count;
  int d_intermediate_keyval_arr_arr_len;		//number of elements of d_intermediate_keyval_arr_arr
  //keyval_arr_t *d_intermediate_keyval_arr_arr;  //data structure to store intermediate keyval pairs in device
  keyval_arr_t **d_intermediate_keyval_arr_arr_p;
  //keyval_arr_t *h_intermediate_keyval_arr_arr;
  keyval_t* d_intermediate_keyval_arr;			//data structure to store flattened intermediate keyval pairs

  void *d_intermediate_keys_shared_buff;
  void *d_intermediate_vals_shared_buff;
  keyval_pos_t *d_intermediate_keyval_pos_arr;

  void *h_intermediate_keys_shared_buff;
  void *h_intermediate_vals_shared_buff;
  keyval_pos_t *h_intermediate_keyval_pos_arr;


  //data for sorted intermediate results
  int d_sorted_keyvals_arr_len;
  //keyvals_t *d_sorted_keyvals_arr;
  //keyvals_t *h_sorted_keyvals_arr;

  void *h_sorted_keys_shared_buff;
  void *h_sorted_vals_shared_buff;
  int totalKeySize;
  int totalValSize;
  sorted_keyval_pos_t *h_sorted_keyval_pos_arr;

  void *d_sorted_keys_shared_buff;
  void *d_sorted_vals_shared_buff;
  keyval_pos_t *d_keyval_pos_arr;
  int *d_pos_arr_4_sorted_keyval_pos_arr;

  //data for reduce results
  int d_reduced_keyval_arr_len;
  keyval_t* d_reduced_keyval_arr;

} gpu_context;

typedef struct {
	char *file_name;
	char *device_name;
	int tid;	   //accelerator group id
	int num_gpus;  //num_processors
	char device_type;
	gpu_context* d_g_state;

} thread_info_t;


#define GPU_ACC		0x01
#define CPU_ACC		0x02
#define CELL_ACC	0x03

//------------------------------------------------------
//PandaSort.cu
//------------------------------------------------------
typedef int4 cmp_type_t;


__global__ void printData(gpu_context d_g_state );
__global__ void printData2(gpu_context d_g_state );
__global__ void printData3(gpu_context d_g_state );
__global__ void printData4(int index, int j, val_t *p);


extern "C"
void sort_CPU(gpu_context *d_g_state);

extern "C"
void Shuffle4GPUOutput(gpu_context *d_g_state);

extern "C"
void InitGPUMapReduce(gpu_context* d_g_state);

extern "C"
void InitGPUDevice(thread_info_t* thread_info);


extern "C"
int StartGPUMap(gpu_context *d_g_state);

extern "C"
int StartCPUMap(gpu_context *d_g_state);

extern "C"
void StartGPUShuffle(gpu_context *d_g_state);

extern "C"
void StartGPUReduce(gpu_context *d_g_state);

extern "C"
void DestroyDGlobalState(gpu_context * d_g_state);


extern "C"
void *Panda_Map(void *ptr);

extern "C"
void Panda_Shuffle_Merge(gpu_context *d_g_state_0, gpu_context *d_g_state_1);

extern "C"
void *Panda_Reduce(void *ptr);

//------------------------------------------------------
//PandaScan.cu
//------------------------------------------------------
extern "C"
void saven_initialPrefixSum(unsigned int maxNumElements);


extern "C"
void prescanArray(int *outArray, int *inArray, int numElements);

extern "C"
int prefexSum( int* d_inArr, int* d_outArr, int numRecords );


//-------------------------------------------------------
//PandaLib.cu
//-------------------------------------------------------

#define DEFAULT_DIMBLOCK	256
#define DEFAULT_NUMTASK		1

#define MAP_ONLY		0x01
#define MAP_GROUP		0x02
#define MAP_REDUCE		0x03


typedef struct
{
	//for input data on host
	char*		inputKeys;
	char*		inputVals;
	int4*		inputOffsetSizes;
	int		inputRecordCount;

	//for intermediate data on host
	char*		interKeys;
	char*		interVals;
	int4*		interOffsetSizes;
	int2*		interKeyListRange;
	int		interRecordCount;
	int		interDiffKeyCount;
	int		interAllKeySize;
	int		interAllValSize;

	//for output data on host
	char*		outputKeys;
	char*		outputVals;
	int4*		outputOffsetSizes;
	int2*		outputKeyListRange;
	int		outputRecordCount;
	int		outputAllKeySize;
	int		outputAllValSize;
	int		outputDiffKeyCount;

	//user specification
	char		workflow;
	char		outputToHost;

	int		numRecTaskMap;
	int		numRecTaskReduce;
	int		dimBlockMap;
	int		dimBlockReduce;
	//char* myKeys;

} Spec_t;


__device__ void Emit2 (void*		key, 
						void*		val, 
						int		keySize, 
						int		valSize,
		           gpu_context *d_g_state);




__device__ void EmitIntermediate2(void *key, 
								  void *val, 
								  int keySize, 
								  int valSize, 
								  gpu_context *d_g_state,
								  int map_task_idx
								  );




__device__ void *GetVal(void *vals, int4* interOffsetSizes, int index, int valStartIndex);
__device__ void *GetKey(void *key, int4* interOffsetSizes, int index, int valStartIndex);



extern __shared__ char sbuf[];
#define GET_OUTPUT_BUF(offset) (sbuf + threadIdx.x * 5 * sizeof(int) + offset)
#define GET_VAL_FUNC(vals, index) GetVal(vals, interOffsetSizes, index, valStartIndex)
#define GET_KEY_FUNC(key, index) GetKey(key, interOffsetSizes, index, valStartIndex)

extern "C"
gpu_context *GetDGlobalState();

extern "C"
void AddMapInputRecord2(gpu_context*		spec, 
		   void*		key, 
		   void*		val, 
		   int		keySize, 
		   int		valSize);



extern "C"
void MapReduce2(gpu_context *d_g_state);




extern "C"
void FinishMapReduce2(gpu_context* state);



//-------------------------------------------------------
//MarsUtils.cu
//-------------------------------------------------------
//typedef struct timeval TimeVal_t;

/*
extern "C"
void startTimer(TimeVal_t *timer);

extern "C"
void endTimer(char *info, TimeVal_t *timer);
*/


#ifdef _DEBUG
#define DoLog(...) do{printf("[PandaLog][%s]\t\t",__FUNCTION__);printf(__VA_ARGS__);printf("\n");}while(0)
#else
#define DoLog(...) //do{printf(__VA_ARGS__);printf("\n");}while(0)
#endif

typedef void (*PrintFunc_t)(void* key, void* val, int keySize, int valSize);
void PrintOutputRecords(Spec_t* spec, int num, PrintFunc_t printFunc);

#endif //__PANDA_H__
