BACKEND     ?= cuda
GPU_TARGET  ?= Turing Ampere Ada Hopper

OPENBLASDIR ?= /usr
CUDADIR     ?= /opt/cuda
ROCM_PATH   ?= /opt/rocm

ifeq (,$(findstring $(BACKEND),hip cuda))
    $(error "'BACKEND' should be either 'cuda' or 'hip' (got $(BACKEND))")
endif

CC          ?= gcc
CXX         ?= g++
FORT        ?= gfortran
HIPCC       ?= hipcc
NVCC        ?= $(CUDADIR)/bin/nvcc
DEVCC       ?= NONE

ifeq ($(BACKEND),cuda)
    DEVCC = $(NVCC)
else ifeq ($(BACKEND),hip)
    DEVCC = $(HIPCC)
endif

ARCH      = ar
ARCHFLAGS = cr
RANLIB    = ranlib

ifeq ($(BACKEND),cuda)
    GPU_TARGET ?= Volta Turing Ampere
else ifeq ($(BACKEND),hip)
    GPU_TARGET ?= gfx900 gfx906 gfx908
endif

FPIC      = -fPIC

CFLAGS      = -O3 $(FPIC) -DNDEBUG -DADD_ -Wall -fopenmp -std=c99
CXXFLAGS    = -O3 $(FPIC) -DNDEBUG -DADD_ -Wall -fopenmp -std=c++11
FFLAGS      = -O3 $(FPIC) -DNDEBUG -DADD_ -Wall -Wno-unused-dummy-argument
F90FLAGS    = -O3 $(FPIC) -DNDEBUG -DADD_ -Wall -Wno-unused-dummy-argument -x f95-cpp-input
LDFLAGS     =     $(FPIC)                       -fopenmp

DEVCCFLAGS  = -O3         -DNDEBUG -DADD_

LIB       += -lopenblas -lgomp

LIBDIR    += -L$(OPENBLASDIR)/lib
INC       +=

-include make.check-openblas

ifeq ($(BACKEND),cuda)
    -include make.check-cuda

    DEVCCFLAGS += -Xcompiler "$(FPIC)" -std=c++11

    INC    += -I$(CUDADIR)/include
    LIBDIR += -L$(CUDADIR)/lib64
    LIB    += -lcublas -lcusparse -lcudart -lcudadevrt
endif

ifeq ($(BACKEND),hip)
    -include make.check-hip

    DEVCCFLAGS += $(FPIC) -std=c++11

    INC    += -I$(ROCM_PATH)/include
    LIBDIR += -L$(ROCM_PATH)/lib
    LIB    += -lhipblas -lhipsparse
endif


