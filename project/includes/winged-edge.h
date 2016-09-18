
//-----------------------------------------------------------------------------
#include <vector>

//-----------------------------------------------------------------------------

using namespace std;

//-----------------------------------------------------------------------------

struct vertex ;
struct face ;
struct wedge ;

struct vertex {
    vector< wedge* > edges_from_vertex;
};

struct face {
    vector< wedge* > edges_of_face;
};

struct wedge {
    vertex * v ;
    face *f ;
    wedge *prev , *next ;
    wedge *sym ;
};
