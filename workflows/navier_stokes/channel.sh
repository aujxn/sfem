#!/usr/bin/env bash

set -e

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

PATH=$SCRIPTPATH:$PATH
PATH=$SCRIPTPATH/../..:$PATH
PATH=$SCRIPTPATH/../../python:$PATH
PATH=$SCRIPTPATH/../../python/mesh:$PATH
PATH=$SCRIPTPATH/../../python/algebra:$PATH
PATH=$SCRIPTPATH/../../data/benchmarks/meshes:$PATH

export ISOLVER_LSOLVE_PLUGIN=$CODE_DIR/utopia/utopia/build_shared/libutopia.dylib
# export ISOLVER_LSOLVE_PLUGIN=$CODE_DIR/utopia/utopia/build_shared/libutopia.so

export OMP_NUM_THREADS=8
export OMP_PROC_BIND=true

if [[ -z "$ISOLVER_LSOLVE_PLUGIN" ]]
then
	echo "Error! Please define ISOLVER_LSOLVE_PLUGIN=<path_to_plugin.dylib>"
	exit -1
fi

SFEM_MESH_DIR=mesh
# create_box_2D_p2.sh 3
# rm -f $mesh/z.raw
nvars=3

sleft=$SFEM_MESH_DIR/sidesets_aos/sleft.raw
sright=$SFEM_MESH_DIR/sidesets_aos/sright.raw
sbottom=$SFEM_MESH_DIR/sidesets_aos/sbottom.raw
stop=$SFEM_MESH_DIR/sidesets_aos/stop.raw

export SFEM_VELOCITY_DIRICHLET_NODESET="$sleft,$sleft,$sbottom,$sbottom,$stop,$stop"
export SFEM_VELOCITY_DIRICHLET_VALUE="1,0,0,0,0,0,"
export SFEM_VELOCITY_DIRICHLET_COMPONENT="0,1,0,1,0,1"

# python3 -c "import numpy as np; np.array([0]).astype(np.int32).tofile('pbc.int32.raw')"
# export SFEM_PRESSURE_DIRICHLET_NODESET="pbc.int32.raw"

export SFEM_PRESSURE_DIRICHLET_NODESET="$sright"
export SFEM_PRESSURE_DIRICHLET_VALUE="0"
export SFEM_PRESSURE_DIRICHLET_COMPONENT="0"

export SFEM_DT=0.00001
export SFEM_MAX_TIME=0.00001
export SFEM_RTOL=1e-14
export SFEM_MAX_IT=4000

export SFEM_DYNAMIC_VISCOSITY=100
export SFEM_MASS_DENSITY=1

mkdir -p out
set -x

# lldb -- 
taylor_hood_navier_stokes $SFEM_MESH_DIR out

raw_to_db.py $SFEM_MESH_DIR out.vtk --point_data="out/v.*.raw"
raw_to_db.py $SFEM_MESH_DIR debug.vtk --point_data="out/c.*.raw"

raw_to_db.py $SFEM_MESH_DIR/p1 out_pressure.vtk  --point_data="out/p.raw,out/div.raw"
raw_to_db.py $SFEM_MESH_DIR/p1 p_debug.vtk --point_data="out/tp.raw"
