################################################################################
# Build script for MPI implementation of cmeans using multi-core on Delta
# lihui@indiana.edu   last update 5/27/2012
################################################################################

all:mpi_cmeans

CC	= g++

.c.o:
	$(CC) -c $<
OBJ		= Volume.o cmeansMPICPU.o MDL.o
INCLUDE   = -I. -I/N/soft/intel/impi/4.0.0.028/include64/
LIBPATH = -L/N/soft/intel/impi/4.0.0.028/lib64/

MDL.o: MDL.cpp
	$(CC) -c MDL.cpp
Volume.o: Volume.cpp
	$(CC) -c Volume.cpp 
cmeansMPICPU.o: cmeansMPICPU.cpp
	$(CC) -c cmeansMPICPU.cpp -o cmeansMPICPU.o -fopenmp -lmpi $(LIBPATH) $(INCLUDE)

mpi_cmeans: $(OBJ)
	$(CC) -o mpi_cmeans_cpu $(OBJ) -fopenmp -lmpi $(LIBPATH)
clean:
	rm *.o mpi_cmeans_cpu
