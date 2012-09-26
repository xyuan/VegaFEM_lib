/*************************************************************************
 *                                                                       *
 * Vega FEM Simulation Library Version 1.1                               *
 *                                                                       *
 * "isotropic hyperelastic FEM" library , Copyright (C) 2012 USC         *
 * All rights reserved.                                                  *
 *                                                                       *
 * Code authors: Jernej Barbic, Fun Shing Sin                            *
 * http://www.jernejbarbic.com/code                                      *
 *                                                                       *
 * Research: Jernej Barbic, Fun Shing Sin, Daniel Schroeder,             *
 *           Doug L. James, Jovan Popovic                                *
 *                                                                       *
 * Funding: National Science Foundation, Link Foundation,                *
 *          Singapore-MIT GAMBIT Game Lab,                               *
 *          Zumberge Research and Innovation Fund at USC                 *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the BSD-style license that is            *
 * included with this library in the file LICENSE.txt                    *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the file     *
 * LICENSE.TXT for more details.                                         *
 *                                                                       *
 *************************************************************************/

#include <math.h>
#include "isotropicHyperelasticFEM/MooneyRivlinIsotropicMaterial.h"
#include "volumetricMesh/volumetricMeshMooneyRivlinMaterial.h"

MooneyRivlinIsotropicMaterial::MooneyRivlinIsotropicMaterial(TetMesh * tetMesh)
{
  int numElements = tetMesh->getNumElements();
  mu01_ = (double*) malloc (sizeof(double) * numElements);
  mu10_ = (double*) malloc (sizeof(double) * numElements);
  v1_ = (double*) malloc (sizeof(double) * numElements);

  for(int el=0; el<numElements; el++)
  {
    VolumetricMesh::Material * material = tetMesh->getElementMaterial(el);
    VolumetricMesh::MooneyRivlinMaterial * mooneyRivlinMaterial = downcastMooneyRivlinMaterial(material);
    if (mooneyRivlinMaterial == NULL)
    {
      printf("Error: mesh does not consist of Mooney-Rivlin materials.\n");
      throw 1;
    }

    mu01_[el] = mooneyRivlinMaterial->getmu01();
    mu10_[el] = mooneyRivlinMaterial->getmu10();
    v1_[el] = mooneyRivlinMaterial->getv1();
  }
}

MooneyRivlinIsotropicMaterial::~MooneyRivlinIsotropicMaterial() 
{
  free(mu01_);
  free(mu10_);
  free(v1_);
}

double MooneyRivlinIsotropicMaterial::ComputeEnergy(int elementIndex, double * invariants)
{
  double Ic = invariants[0];
  double IIc = invariants[1];
  double IIIc = invariants[2];

  double mu01 = mu01_[elementIndex];
  double mu10 = mu10_[elementIndex];
  double v1 = v1_[elementIndex];

  double energy = 0.5 * (-6.0 + (Ic * Ic - IIc) / pow(IIIc, 2.0 / 3.0)) * mu01 + 
                  (-3.0 + Ic / pow(IIIc, 1.0 / 3.0)) * mu10 + 
                  pow(-1.0 + sqrt(IIIc), 2.0) * v1;
  return energy;
}

void MooneyRivlinIsotropicMaterial::ComputeEnergyGradient(int elementIndex, double * invariants, double * gradient) // invariants and gradient are 3-vectors
{
  double Ic = invariants[0];
  double IIc = invariants[1];
  double IIIc = invariants[2];

  double mu01 = mu01_[elementIndex];
  double mu10 = mu10_[elementIndex];
  double v1 = v1_[elementIndex];

  gradient[0] = (Ic * mu01) / pow(IIIc, 2.0 / 3.0) + 
    mu10 / pow(IIIc, 1.0 / 3.0);
  gradient[1] = (-0.5 * mu01) / pow(IIIc, 2.0 / 3.0);
  gradient[2] = (-1.0 / 3.0 * (Ic * Ic - IIc) * mu01) / pow(IIIc, 5.0 / 3.0) - 
    (1.0 / 3.0 * Ic * mu10) / pow(IIIc, 4.0 / 3.0) + 
    ((-1.0 + sqrt(IIIc)) * v1) / sqrt(IIIc);
}

void MooneyRivlinIsotropicMaterial::ComputeEnergyHessian(int elementIndex, double * invariants, double * hessian) // invariants is a 3-vector, hessian is a 3x3 symmetric matrix, unrolled into a 6-vector, in the following order: (11, 12, 13, 22, 23, 33).
{
  double Ic = invariants[0];
  double IIc = invariants[1];
  double IIIc = invariants[2];

  double mu01 = mu01_[elementIndex];
  double mu10 = mu10_[elementIndex];
  double v1 = v1_[elementIndex];

  // 11
  hessian[0] = mu01 / pow(IIIc, 2.0 / 3.0);
  // 12
  hessian[1] = 0.0;
  // 13
  hessian[2] = (-2.0 / 3.0) * Ic * mu01 / pow(IIIc, 5.0 / 3.0) - 
               mu10 / (3.0 * pow(IIIc, 4.0 / 3.0));
  // 22
  hessian[3] = 0.0;
  // 23
  hessian[4] = mu01 / (3.0 * pow(IIIc, 5.0 / 3.0));
  // 33
  hessian[5] = (5.0 / 9.0) * (Ic * Ic - IIc) * mu01 / pow(IIIc, 8.0 / 3.0) + 
               ((4.0 / 9.0) * Ic * mu10) / pow(IIIc, 7.0 / 3.0) - 
               (-1.0 + sqrt(IIIc)) * v1 / (2.0 * pow(IIIc, 1.5)) + 
               v1 / (2.0 * IIIc);
}

