#ifndef _VEGAFEM_H_
#define _VEGAFEM_H_

// Master include file for all the Vega FEM classes

#include "include/lapack-headers.h"
#include "include/macros.h"
#include "include/matrixMultiplyMacros.h"
#include "include/openGL-hearders.h"
#include "include/triple.h"


#include "camara/camera.h"

#include "configFile/configFile.h"

#include "corotationalLinearFEM.h"
#include "corotationalLinearFEMMT.h"

#include "elasticForceModel/corotationalLinearFEMForceModel.h"
#include "elasticForceModel/isotropicHyperelasticFEMForceModel.h"
#include "elasticForceModel/linearFEMForceModel.h"
#include "elasticForceModel/massSpringSystemForceModel.h"
#include "elasticForceModel/StVKForceModel.h"

#include "forceModel/forceModel.h"

#include "getopts/getopts.h"

#include "graph/graph.h"

#include "insertRows.h"

#include "integrator/integrators.h"

#include "isotropicHyperelasticFEM/isotropicMaterial.h"
#include "isotropicHyperelasticFEM/MooneyRivlinIsotropicMaterial.h"
#include "isotropicHyperelasticFEM/neoHookeanIsotropicMaterial.h"
#include "isotropicHyperelasticFEM/StVKIsotropicMaterial.h"
#include "isotropicHyperelasticFEM/homogeneousMooneyRivlinIsotropicMaterial.h"
#include "isotropicHyperelasticFEM/homogeneousStVKIsotropicMaterial.h"
#include "isotropicHyperelasticFEM/homogeneousNeoHookeanIsotropicMaterial.h"
#include "isotropicHyperelasticFEM/isotropicHyperelasticFEM.h"
#include "isotropicHyperelasticFEM/isotropicHyperelasticFEMMT.h"

#include "lighting/lighting.h"

#include "loadList/loadList.h"

#include "massSpringSystem/massSpringSystemFromObjMeshConfigFile.h"
#include "massSpringSystem/massSpringSystemFromObjMesh.h"
#include "massSpringSystem/massSpringSystemFromTetMeshConfigFile.h"
#include "massSpringSystem/massSpringSystemFromTetMesh.h"
#include "massSpringSystem/massSpringSystem.h"
#include "massSpringSystem/massSpringSystemMT.h"
#include "massSpringSystem/renderSprings.h"
#include "massSpringSystem/massSpringSystemFromCubicMesh.h"
#include "massSpringSystem/massSpringSystemFromCubicMeshConfigFile.h"

#include "matrix/matrixIO.h"
#include "matrix/matrixMacros.h"

#include "minivector/minivector.h"

#include "objMesh/objMeshBinaryLoader.h"
#include "objMesh/objMesh.h"
#include "objMesh/objMeshRender.h"
#include "objMesh/objMeshEncode.h"

#include "performanceCounter/performanceCounter.h"

#include "polarDecomposition/polarDecompositionGradient.h"
#include "polarDecomposition/polarDecomposition.h"

#include "sceneObject/sceneObjects.h"

#include "sparseMatrix/sparseMatrix.h"
#include "sparseMatrix/sparseMatrixMT.h"

#include "sparseSolver/sparseSolvers.h"

#include "stvk/StVKCubeABCD.h"
#include "stvk/StVKElementABCD.h"
#include "stvk/StVKElementABCDLoader.h"
#include "stvk/StVKHessianTensor.h"
#include "stvk/StVKInternalForces.h"
#include "stvk/StVKInternalForcesMT.h"
#include "stvk/StVKStiffnessMatrix.h"
#include "stvk/StVKStiffnessMatrixMT.h"
#include "stvk/StVKTetABCD.h"
#include "stvk/StVKTetHighMemoryABCD.h"

#include "volumetricMesh/volumetricMeshParser.h"
#include "volumetricMesh/generateInterpolationMatrix.h"
#include "volumetricMesh/generateMassMatrix.h"
#include "volumetricMesh/generateSurfaceMesh.h"
#include "volumetricMesh/generateMeshGraph.h"
#include "volumetricMesh/cubicMesh.h"
#include "volumetricMesh/tetMesh.h"
#include "volumetricMesh/volumetricMesh.h"
#include "volumetricMesh/volumetricMeshLoader.h"
#include "volumetricMesh/volumetricMeshENuMaterial.h"
#include "volumetricMesh/volumetricMeshMooneyRivlinMaterial.h"
#include "volumetricMesh/volumetricMeshExtensions.h"

#endif