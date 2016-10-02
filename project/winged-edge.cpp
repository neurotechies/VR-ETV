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

        // get vertices array
        cVertexArray * verticesArr = cVertexArrayPtr.get();

        for (int j = 0 ; j < verticesArr->getNumVertices(); j++ ){
              cVector3d gpos = verticesArr->getGlobalPos(i);
              Vertex ver = new Vertex();
              ver->x = gpos->x();
              ver->y = gpos->y();
              ver->z = gpos->x();
              w_mesh->vertices.push_back(&ver);
        }

        // get edges array
        for (int j = 0 ; j < edges.size(); j++ ){
              int v1 = edges[j]->m_vertex0;
              int v2 = edges[j]->m_vertex1;

              w_edge e = new w_edge();

              e->start = w_mesh->vertices[v1] ;
              e->end = w_mesh->vertices[v2] ;

              w_mesh->edges.push_back(&e) ;


              w_mesh->vertices[v1]->edges.push_back(&e)
              w_mesh->vertices[v2]->edges.push_back(&e)
        }

        // get vertices array
        cTriangleArray * triangleArr = cVertexArrayPtr.get();


        for (int j = 0 ; j < triangleArr->getNumVertices(); j++ ){

        }


    }

};


void clearAllWinged(w_mesh * wmesh){

};
