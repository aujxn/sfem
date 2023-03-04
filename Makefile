ifeq ($(debug),1)
	CFLAGS += -O0 -g
	CXXFLAGS += -O0 -g
	CUFLAGS += -O0 -g
else ifeq ($(prof),1)
	CFLAGS += -O2 -g -DNDEBUG
	CXXFLAGS += -O2 -g -DNDEBUG
	CUFLAGS += -O2 -g -DNDEBUG 
else ifeq ($(asan), 1)
	ASAN_FLAGS += -fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O0
	CXXFLAGS += $(ASAN_FLAGS)
	CFLAGS += $(ASAN_FLAGS)
else
	CFLAGS += -Ofast -DNDEBUG
	CXXFLAGS += -Ofast -DNDEBUG
	CUFLAGS += -O3 -DNDEBUG 
endif

ifeq ($(avx512sort), 1)
	CXXFLAGS += -DSFEM_ENABLE_AVX512_SORT -Iexternal/x86-simd-sort/src -march=native
	CFLAGS += -march=native
endif

ifeq ($(avx2sort), 1)
	CXXFLAGS += -DSFEM_ENABLE_AVX2_SORT -Iexternal -march=core-avx2
	CFLAGS += -march=core-avx2
endif





CFLAGS += -pedantic -Wextra
# CFLAGS += -std=c99 

CXXFLAGS += -std=c++11
CXXFLAGS += -fno-exceptions -fno-rtti -static
CXXFLAGS += -fvisibility=hidden
CXXFLAGS += -fPIC
CUFLAGS += --compiler-options -fPIC -std=c++17 -arch=native 

# CUFLAGS += --compiler-options -fPIC -O0 -g -std=c++17

INCLUDES += -I$(PWD) -I$(PWD)/.. -I$(PWD)/../matrix.io 

# Assemble systems
GOALS = assemble assemble3 assemble4 

# Mesh manipulation
GOALS += partition select_submesh refine skin

# Algebra post process
GOALS += condense_matrix condense_vector idx_to_indicator remap_vector sgather

# Resampling
GOALS += pizzastack_to_mesh

DEPS = -L$(PWD)/../matrix.io/ -lmatrix.io -lstdc++

LDFLAGS += $(DEPS) -lm

MPICC ?= mpicc
CXX ?= c++
AR ?= ar
NVCC ?= nvcc

all : $(GOALS)

OBJS = \
	sortreduce.o \
	crs_graph.o \
	sortreduce.o \
	argsort.o \
	read_mesh.o  \
	mass.o \
	dirichlet.o \
	neumann.o \
	sfem_mesh.o \
	sfem_mesh_write.o \
	isotropic_phasefield_for_fracture.o


ifeq ($(cuda), 1)
	CUDA_OBJS = cuda_laplacian.o
	DEPS += -L/opt/cuda/lib64 -lcudart

	OBJS += $(CUDA_OBJS)
else
	SERIAL_OBJS = laplacian.o
# 	SERIAL_OBJS += neohookean.o 
	OBJS += $(SERIAL_OBJS)
endif

SIMD_OBJS = simd_neohookean.o
# SIMD_OBJS +=  simd_laplacian.o 

OBJS += $(SIMD_OBJS)

plugins: utopia_sfem.dylib

libsfem.a : $(OBJS)
	ar rcs $@ $^

assemble : assemble.o libsfem.a
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ; \

assemble3 : assemble3.o libsfem.a
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ; \

assemble4 : assemble4.o libsfem.a
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ; \

partition : partition.o libsfem.a
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ; \

select_submesh : select_submesh.o libsfem.a
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ; \

refine : refine.o libsfem.a
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ; \

skin : mesh_tools/skin.c extract_surface_graph.o libsfem.a
	$(MPICC) $(CFLAGS) $(INCLUDES)  -o $@ $^ $(LDFLAGS) ; \

extract_surface_graph.o : mesh_tools/extract_surface_graph.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

pizzastack_to_mesh: resampling/pizzastack_to_mesh.c pizzastack/grid.c libsfem.a
	$(MPICC) $(CFLAGS) $(INCLUDES)  -o $@ $^ $(LDFLAGS) ; \

condense_matrix : condense_matrix.o
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ; \

condense_vector : condense_vector.o
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ; \

idx_to_indicator : idx_to_indicator.o
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ; \

remap_vector : remap_vector.o
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ; \

sgather : sgather.o
	$(MPICC) $(CFLAGS) -o $@ $^ $(LDFLAGS) ; \
	
utopia_sfem.dylib : utopia_sfem_plugin.o  libsfem.a
	$(MPICC) -shared -o $@ $^ $(LDFLAGS)  

utopia_sfem_plugin.o : plugin/utopia_sfem_plugin.c
	$(MPICC) $(CFLAGS) $(INCLUDES) -c $<

sortreduce.o: sortreduce.cpp
	$(CXX) $(CXXFLAGS) -c $<

argsort.o: argsort.cpp
	$(CXX) $(CXXFLAGS) -c $<

%.o : %.c
	$(MPICC) $(CFLAGS) $(INCLUDES) -c $<

%.o : %.cu
	$(NVCC) $(CUFLAGS) $(INCLUDES) -c $<

.SUFFIXES :
.PRECIOUS :

clean:
	rm *.o *.a $(GOALS)

.SUFFIXES:

.PHONY: clean all
