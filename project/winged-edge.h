
//-----------------------------------------------------------------------------
#include <vector>
#include "chai3d.h"
//-----------------------------------------------------------------------------

using namespace chai3d;
using namespace std;

//-----------------------------------------------------------------------------

struct vertex ;
struct face ;
struct w_edge ;

/// representing vertex in 3d meshes
struct vertex {
    // public :
      int index ;
      vector < w_edge* > edges;

      // void setIndex( int index1 ){
      //   cout << "ver1" << endl ;
      //   index = index1 ;
      //   cout << "ver2" << endl ;
      //
      // }
};

struct face {
    int ind ;
    vector < w_edge* > edge;
};

struct w_edge {
    int ind ;
    vertex *start , *end ;
    face  *left, *right ;
    w_edge *left_prev , *left_next ;
    w_edge *right_prev , *right_next ;
};
// =-------------------------

struct w_mesh{
    vector < vertex * > vertices;
    vector < w_edge* > edges ;
    vector < face* > faces ;
};
// void addVertex(w_mesh * mesh , vertex * v);
void clearAllWinged(w_mesh * wmesh);
bool wingedFromMesh(w_mesh * wmesh , cMultiMesh * multimesh) ;
