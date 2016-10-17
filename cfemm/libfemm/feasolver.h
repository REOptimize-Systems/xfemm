/*
   This code is a modified version of an algorithm
   forming part of the software program Finite
   Element Method Magnetics (FEMM), authored by
   David Meeker. The original software code is
   subject to the Aladdin Free Public Licence
   version 8, November 18, 1999. For more information
   on FEMM see www.femm.info. This modified version
   is not endorsed in any way by the original
   authors of FEMM.

   This software has been modified to use the C++
   standard template libraries and remove all Microsoft (TM)
   MFC dependent code to allow easier reuse across
   multiple operating system platforms.

   Date Modified: 2011 - 11 - 10
   By: Richard Crozier
   Contact: richard.crozier@yahoo.co.uk
*/

// feasolver.h : interface of the generic feasolver class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef FEASOLVER_H
#define FEASOLVER_H

#include <string>

#include "fparse.h"
#include "mesh.h"
#include "spars.h"
#include "CBoundaryProp.h"
#include "CCommonPoint.h"

enum LoadMeshErr
{
  BADFEMFILE,
  BADNODEFILE,
  BADPBCFILE,
  BADELEMENTFILE,
  BADEDGEFILE,
  MISSINGMATPROPS
};

template< class PointPropT
          , class BoundaryPropT
          , class BlockPropT
          , class CircuitPropT
          , class BlockLabelT
          , class NodeT
          >
class FEASolver
{

// Attributes
public:

    FEASolver();
    virtual ~FEASolver();

    // General problem attributes
    double FileFormat; ///< \brief format version of the file
    double Frequency;  ///< \brief Frequency for harmonic problems [Hz]
    double Precision;  ///< \brief Computing precision within FEMM
    double MinAngle;   ///< \brief angle restriction for triangulation [deg]
    double Depth;      ///< \brief typical length in z-direction [lfac]
    femm::LengthUnit  LengthUnits;  ///< \brief Unit for lengths. Also referred to as \em lfac.
    femm::CoordsType  Coords;  ///< \brief definition of the coordinate system
    femm::ProblemType ProblemType; ///< \brief The 2D problem is either planar or axisymmetric
    // axisymmetric external region parameters
    double extZo;  ///< \brief center of exterior [lfac], only valid for axisymmetric problems
    double extRo;  ///< \brief radius of exterior [lfac], only valid for axisymmetric problems
    double extRi;  ///< \brief radius of interior [lfac], only valid for axisymmetric problems
    std::string comment; ///< \brief Problem description

    int		ACSolver;
    bool    DoForceMaxMeshArea;
    bool    bMultiplyDefinedLabels;


    // CArrays containing the mesh information
    int	BandWidth;
    femm::CElement *meshele;

    int NumNodes;
    int NumEls;

    // lists of properties
    int NumBlockProps;
    int NumPBCs;
    int NumLineProps;
    int NumPointProps;
    int NumCircProps;
    int NumBlockLabels;

    femm::CCommonPoint	*pbclist;

    // string to hold the location of the files
    std::string PathName;

    //std::vector< PointPropT > nodeProps;
    //std::vector< BoundaryPropT > lineProps;
    //std::vector< BlockPropT > blockProps;
    //std::vector< CircuitPropT > circProps;
    //std::vector< BlockLabelT > labels;
    //std::vector< NodeT > nodes;
// Operations
public:

    virtual int LoadMesh(bool deleteFiles=true) = 0;
    virtual int LoadProblemFile () = 0;
    int Cuthill(bool deleteFiles=true);
    int SortElements();

    // pointer to function to call when issuing warning messages
    void (*WarnMessage)(const char*);

private:

    virtual void SortNodes (int* newnum) = 0;

};

/////////////////////////////////////////////////////////////////////////////

double Power(double x, int n);

#endif
