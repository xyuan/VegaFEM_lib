/*************************************************************************
 *                                                                       *
 * Vega FEM Simulation Library Version 1.1                               *
 *                                                                       *
 * "volumetricMesh" library , Copyright (C) 2007 CMU, 2009 MIT, 2012 USC *
 * All rights reserved.                                                  *
 *                                                                       *
 * Code author: Jernej Barbic                                            *
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

#ifndef _VOLUMETRICMESH_H_
#define _VOLUMETRICMESH_H_

/*
  This abstract class can store a generic volumetric 3D mesh. 
  It stores the mesh geometric information (elements and vertices), 
  and also the material parameters of each individual mesh element 
  (Young's modulus, Poisson ratio, mass density). This is done by 
  organizing elements with the same material parameters into a "region".
  The class supports several geometric queries and interpolation to 
  an embedded triangle mesh ("Free-Form Deformation").  It also 
  supports exporting the mesh to an .ele or .node format (the format 
  used by the Stellar and TetGen mesh generation packages).  Derived classes 
  are the TetMesh (general tetrahedral meshes), and CubicMesh 
  (axis-aligned cubic "voxel" meshes). See description in tetMesh.h and cubicMesh.h.

  All quantities are 0-indexed, except the input mesh files where the 
  elements and vertices are 1-indexed (same as in TetGen and Stellar).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <set>
#include <string>
#include <map>
#include "minivector/minivector.h"

class VolumetricMesh
{
public:

  // Note: This class is abstract and cannot be instantiated; use the constructors in the derived classes (TetMesh, CubicMesh) to initialize a mesh

  // copy constructor, destructor
  VolumetricMesh(const VolumetricMesh & volumetricMesh);
  virtual VolumetricMesh * clone() = 0;
  virtual ~VolumetricMesh();

  // nested classes to store sets, materials and regions (declared later)
  class Set; 
  class Material; 
  class Region;

  // === save/export ===

  // saves the mesh to a text file (.veg file format, see examples and documentation)
  virtual int save(char * filename) const = 0; 
  // exports the mesh geometry to an .ele and .node file (TetGen and Stellar format)
  // if includeRegions=1, an extra column is added to output, identifying the region of each element
  int exportToEle(char * baseFilename, int includeRegions=0) const;
  // exports the mesh geometry to memory arrays (say, for external usage)
  // all parameters are output parameters
  // vertices and elements will be allocated inside the routine
  void exportMeshGeometry(int * numVertices, double ** vertices, int * numElements, int * numElementVertices, int ** elements) const;

  // === vertex and element access ===

  typedef enum { INVALID, TET, CUBIC } elementType;
  // opens the file and returns the element type of the volumetric mesh in the file; returns INVALID if no type information found
  static elementType getElementType(char * filename); 
  virtual elementType getElementType() const = 0; // calls the derived class to identify itself

  inline int getNumVertices() const { return numVertices; }
  inline Vec3d * getVertex(int i) const { return vertices[i]; }
  inline Vec3d * getVertex(int element, int vertex) const { return vertices[elements[element][vertex]]; }
  inline int getVertexIndex(int element, int vertex) const { return elements[element][vertex]; }
  inline Vec3d ** getVertices() const { return vertices;} // advanced, internal datastructure
  inline int getNumElements() const { return numElements; }
  inline int getNumElementVertices() const { return numElementVertices; } 

  // === materials access === 

  inline int getNumMaterials() const { return numMaterials; }
  inline Material * getMaterial(int i) const { return materials[i]; }
  inline Material * getElementMaterial(int el) const { return materials[elementMaterial[el]]; }
  void setMaterial(int i, Material & material) { *(materials[i]) = material; } // sets i-th material to the given material
  void setSingleMaterial(double E, double nu, double density); // erases all materials and creates a single material for the entire mesh
  static void getDefaultMaterial(double * E, double * nu, double * density);

  inline int getNumSets() const { return numSets; }
  inline Set * getSet(int i) const { return sets[i]; }

  inline int getNumRegions() const { return numRegions; }
  inline Region * getRegion(int i) const { return regions[i]; }

  // mass density of an element
  double getElementDensity(int el) const { return materials[elementMaterial[el]]->getDensity(); }
  // computes the mass matrix of a single element
  // note: to compute the mass matrix for the entire mesh, use generateMassMatrix.h
  virtual void computeElementMassMatrix(int element, double * massMatrix) const = 0; // massMatrix is numElementVertices_ x numElementVertices_

  // === geometric queries and transformations ===

  Vec3d getElementCenter(int el) const;

  // center of mass and inertia tensor
  double getVolume() const;
  virtual double getElementVolume(int el) const = 0;
  void getVertexVolumes(double * vertexVolumes) const; // compute the volume "belonging" to each vertex
  virtual void getElementInertiaTensor(int el, Mat3d & inertiaTensor) const = 0; // returns the inertia tensor of a single element, around its center of mass, with unit density
  void getInertiaParameters(double & mass, Vec3d & centerOfMass, Mat3d & inertiaTensor) const ; // center of mass and inertia tensor for the entire mesh

  // centroid is the geometric center of all vertices; radius is the tightest fitting sphere centered at the centroid
  void getMeshGeometricParameters(Vec3d & centroid, double * radius) const;

  // mesh 1-neighborhood queries
  void getVerticesInElements(std::vector<int> & elements, std::vector<int> & vertices) const;
  void getElementsTouchingVertices(std::vector<int> & vertices, std::vector<int> & elements) const;
  void getVertexNeighborhood(std::vector<int> & vertices, std::vector<int> & neighborhood) const;

  // proximity queries
  int getClosestElement(Vec3d pos) const; // finds the closest element to the given position (using linear scan); distance to a element is defined as distance to its center
  int getClosestVertex(Vec3d pos) const; // finds the closest vertex to the given position (using linear scan)
  int getContainingElement(Vec3d pos) const; // finds the element that containts the given position (using linear scan); if such element does not exist, -1 is returned
  virtual bool containsVertex(int element, Vec3d pos)  const = 0; // true if given element contain given position, false otherwise

  // computes the gravity vector (different forces on different mesh vertices due to potentially varying mass densities)
  // gravityForce must be a pre-allocated vector of length 3xnumVertices()
  void computeGravity(double * gravityForce, double g=9.81, bool addForce=false) const;

  // edge queries
  virtual int getNumElementEdges() const = 0;
  virtual void getElementEdges(int el, int * edgeBuffer) const = 0; // edgeBuffer must be pre-allocated, of size 2 x numElementEdges()

  // (permanently) applies the deformation to the vertices of the mesh
  void applyDeformation(double * u);
  void applyLinearTransformation(double * pos, double * R); // transforms every vertex as X |--> pos + R * X (R must be given row-major)

  // === submesh creation ===

  // (permanently) set this mesh to its submesh containing the specified elements (i.e., delete the mesh elements not on the given list of elements)
  // if vertexMap is non-null, it also returns a renaming datastructure: vertexMap[big mesh vertex] is the vertex index in the subset mesh
  void setToSubsetMesh(std::set<int> & subsetElements, int removeIsolatedVertices=1, std::map<int,int> * vertexMap = NULL);

  // === interpolation ===

  // the interpolant is a triple (numTargetLocations, vertices, weights)

  // Generates interpolation weights to transfer quantities from volumetric mesh to (embedded) surface meshes.
  // Input is a list of 3D target locations where the interpolant will be computed,
  // e.g., those could be vertices of a triangle mesh embedded into the volumetric mesh.
  // Each location is a 3-vector, i.e., 3 consecutive double-precision vectors.
  // If zeroThreshold is set positive, than for any target location that is 
  //   more than zeroThreshold away from the closest voxel, 
  //   all weights will be set to zero; this is useful, e.g. to 
  //   stabilize locations far away from your mesh.
  // Output: vertices and weights arrays
  // vertices: gives a list of integer indicies of the vertices of the element
  //   closest to the target location (numElementVertices entries per target location, one for each element vertex)
  //   note: if target location is inside a voxel, that voxel will be chosen as closest
  // weights: a list of numElementVertices_ weights, as per the numElementVertices_ vertices of each element (weights sum to 1)
  // If zeroThreshold >= 0, then the points that are further than zeroThreshold away from any volumetric mesh vertex, are assigned weights of 0.
  // If closestElementList is not NULL, the closest elements will be returned in the vector "closestElementList".
  // Returns the number of target points which do not lie inside any element.
  int generateInterpolationWeights(int numTargetLocations, double * targetLocations, int ** vertices, double ** weights, 
	double zeroThreshold = -1.0, std::vector<int> * closestElementList = NULL, int verbose=0) const;  
  static int getNumInterpolationElementVertices(char * filename); // looks at the first line of "filename" to determine "numElementVertices" for this particular interpolant
  static int loadInterpolationWeights(char * filename, int numTargetLocations, int numElementVertices, int ** vertices, double ** weights); // returns 0 on success
  static int saveInterpolationWeights(char * filename, int numTargetLocations, int numElementVertices, int * vertices, double * weights);
  // interpolates 3D vector data from vertices of the 
  //   volumetric mesh (data given in u) to the target locations (output goes into uTarget)
  //   e.g., use this to interpolate deformation from the volumetric mesh to a triangle mesh
  static void interpolate(double * u, double * uTarget, int numTargetLocations, int numElementVertices, int * vertices, double * weights);

  // computes barycentric weights of the given position with respect to the given element
  virtual void computeBarycentricWeights(int element, Vec3d pos, double * weights) const = 0;

  // computes the gradient of a 3D vector field (specified at the volumetric mesh vertices), at the location "pos"
  // "numFields" fields can be interpolated simultaneously; each is given as one column of the U matrix
  // U is a 3numVertices x numFields matrix; stored column-major
  // output: grad is 9 x numFields matrix, stored column-major; each column gives the gradient (3x3 matrix), stored row-major format
  // return: 0 if pos inside the mesh, 1 otherwise
  int interpolateGradient(const double * U, int numFields, Vec3d pos, double * grad) const;
  // in this version, the element containing the "pos" must be known, and prescribed directly
  virtual void interpolateGradient(int element, const double * U, int numFields, Vec3d pos, double * grad) const = 0;

  // === material-related nested classes ===

  // a set of integers, with a name (used for example, to store elements that share the same material properties)
  class Set
  {
  public:

    Set(const char * name);
    Set(const Set & set);

    inline const char * getName() const;
    inline int getNumElements() const;
    inline void getElements(std::set<int> & elements) const;
    inline bool isMember(int element) const;

    inline void insert(int element);
    inline void clear();

  protected:
    char name[24];
    std::set<int> elements;
  };

  // stores a material (abstract class)
  class Material
  {
  public:
    Material(const char * name, double density);
    Material(const Material & material);
    virtual Material * clone() = 0;

    inline const char * getName() const; // material name
    inline double getDensity() const; // density
    inline void setName(char * name);
    inline void setDensity(double density);

    // ENU = any material parameterized by E (Young's modulus), nu (Poisson's ratio)
    // MOONEYRIVLIN = Mooney-Rivlin material
    typedef enum { INVALID, ENU, MOONEYRIVLIN } materialType;
    virtual materialType getType() = 0;

  protected:
    char name[24];
    double density;
  };

  // material with E (Young's modulus), nu (Poisson's ratio) (defined in volumetricMeshENuMaterial.h)
  class ENuMaterial;
  // Mooney-Rivlin material (defined in volumetricMeshMooneyRivlinMaterial.h)
  class MooneyRivlinMaterial;

  // a volumetric mesh region, i.e., a set of elements sharing the same material
  class Region
  {
  public:
    Region(int materialIndex, int setIndex);
    inline int getMaterialIndex() const;
    inline int getSetIndex() const;

  protected:
    int setIndex, materialIndex;
  };

protected:
  int numVertices;
  Vec3d ** vertices;

  int numElementVertices;
  int numElements;
  int ** elements;

  int numMaterials;
  int numSets;
  int numRegions;
  Material ** materials;
  Set ** sets; // elements inside these sets are 1-indexed
  Region ** regions;
  int * elementMaterial;  // material index of each element

  // parses the mesh, and returns the mesh element type
  VolumetricMesh(char * filename, int numElementVertices, elementType * elementType_);
  VolumetricMesh(int numElementVertices_) { numElementVertices = numElementVertices_; }
  void PropagateRegionsToElements();

  // constructs a mesh from the given vertices and elements, 
  // with a single region and material ("E, nu" material)
  // "vertices" is double-precision array of length 3xnumVertices .
  // "elements" is an integer array of length numElements x numElementVertices
  VolumetricMesh(int numVertices, double * vertices, 
         int numElements, int numElementVertices, int * elements,
         double E=E_default, double nu=nu_default, double density=density_default); 

  // creates a submesh consisting of the specified elements of the given mesh
  // if vertexMap is non-null, it also returns a renaming datastructure: vertexMap[big mesh vertex] is the vertex index in the subset mesh
  VolumetricMesh(const VolumetricMesh & mesh, int numElements, int * elements, std::map<int,int> * vertexMap = NULL); 

  int save(char * filename, elementType elementType_) const;

  elementType temp; // auxiliary

  static double E_default;
  static double nu_default;
  static double density_default;

  friend class VolumetricMeshExtensions;

  static void nop(); // no operation
};

inline VolumetricMesh::Set::Set(const char * name_) { strcpy(name, name_); }
inline VolumetricMesh::Set::Set(const Set & set) { elements = set.elements; strcpy(name, set.getName()); }
inline const char * VolumetricMesh::Set::getName() const { return name; }
inline int VolumetricMesh::Set::getNumElements() const { return (int)(this->elements.size()); }
inline void VolumetricMesh::Set::getElements(std::set<int> & elements) const { elements = this->elements; }
inline bool VolumetricMesh::Set::isMember(int element) const {return (elements.find(element) != elements.end());}
inline void VolumetricMesh::Set::insert(int element) { elements.insert(element); }
inline void VolumetricMesh::Set::clear() { elements.clear(); }

inline VolumetricMesh::Material::Material(const char * name_, double density_): density(density_) { strcpy(name, name_); }
inline VolumetricMesh::Material::Material(const Material & material) : density(material.getDensity()) { strcpy(name, material.getName()); }
inline const char * VolumetricMesh::Material::getName() const { return name; }  // material name
inline double VolumetricMesh::Material::getDensity() const { return density; } // density
inline void VolumetricMesh::Material::setName(char * name_) { strcpy(name, name_); }
inline void VolumetricMesh::Material::setDensity(double density_) { density = density_; }

inline VolumetricMesh::Region::Region(int materialIndex_, int setIndex_): setIndex(setIndex_), materialIndex(materialIndex_) {}
inline int VolumetricMesh::Region::getMaterialIndex() const { return materialIndex; }
inline int VolumetricMesh::Region::getSetIndex() const { return setIndex; }

#endif

