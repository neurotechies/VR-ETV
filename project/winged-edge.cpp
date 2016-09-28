//-----------------------------------------------------------------------------
#include "winged-edge.h"
#include <iostream>
//-----------------------------------------------------------------------------
using namespace std;
using namespace chai3d;

bool wingedfromMesh(w_mesh * wmesh , cMultiMesh * multimesh) {
    int num_meshes = multimesh->getNumMeshes();
    // tranversing all meshes inside the multimesh
    for (int i = 0 ; i < num_meshes ; i++){
        // convering ith mesh object into winged object
        cMesh * iMesh = multimesh->getMesh(i);
        // getting all verices  , edges and triangles from iMesh
        cVertexArrayPtr vertices = iMesh->m_vertices ;
        cTriangleArrayPtr triangles = iMesh->m_triangles ;
        std::vector<cEdge> edges = iMesh->m_edges;

        // vertices


    }

};


void clearAllWinged(w_mesh * wmesh){

};
