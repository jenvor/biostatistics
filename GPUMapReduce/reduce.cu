/*	
	Copyright 2012 The Trustees of Indiana University.  All rights reserved.
	CGL MapReduce Framework on GPUs and CPUs
	Code Name: Panda 0.1
	File: reduce.cu 
	Time: 2012-07-01 
	Developer: Hui Li (lihui@indiana.edu)

	This is the source code for Panda, a MapReduce runtime on GPUs and CPUs.
 
 */

#ifndef __REDUCE_CU__
#define __REDUCE_CU__

#include "Panda.h"

//-------------------------------------------------------------------------
//Reduce Function in this application
//-------------------------------------------------------------------------


__device__ void gpu_reduce(void *KEY, val_t* VAL, int keySize, int valCount, gpu_context d_g_state){

		return;
		
}//reduce2

void cpu_reduce(void *KEY, val_t* VAL, int keySize, int valCount, cpu_context* d_g_state){

		return;
		
}//reduce2


#endif //__REDUCE_CU__
