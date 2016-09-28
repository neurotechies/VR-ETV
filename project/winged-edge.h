
//-----------------------------------------------------------------------------
#include <vector>
#include "chai3d.h"
//-----------------------------------------------------------------------------

using namespace chai3d;
// using namespace std;

//-----------------------------------------------------------------------------

struct vertex ;
struct face ;
struct w_edge ;

/// representing vertex in 3d meshes
struct vertex {
    float x,y,z;
    w_edge *edge;

  };
struct face {
    w_edge *edge;
};

struct w_edge {
    vertex *start , *end ;
    face  *left, *right ;
    w_edge *left_prev , *left_next ;
    w_edge *right_prev , *right_next ;
};
// =-------------------------

struct w_mesh{
    w_edge *start ;
};

void clearAllWinged(w_mesh * wmesh);
bool wingedfromMesh(w_mesh * wmesh , cMultiMesh * multimesh) ;
