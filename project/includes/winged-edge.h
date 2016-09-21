
//-----------------------------------------------------------------------------
#include <vector>
#include "chai3d.h"
//-----------------------------------------------------------------------------

// using namespace chai3d;
using namespace std;

//-----------------------------------------------------------------------------

struct vertex ;
struct face ;
struct winged_edge ;

/// representing vertex in 3d meshes 
struct vertex {
    int x;
    int y;
    int z;
    vector< winged_edge* > edges_from_vertex;
};

struct face {

    vector< winged_edge* > edges_of_face;
};

struct winged_edge {
    vertex * v ;
    face * f ;
    winged_edge *prev , *next ;
    winged_edge *sym ;
};

struct winged_edge_mesh{
    vector <winged_edge*> mesh_edges;
};
void clearAllWinged(winged_edge_mesh * wmesh);
bool wingedfromObj(winged_edge_mesh * wmesh , const std::string& a_filename) ;
