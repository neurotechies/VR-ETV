//-----------------------------------------------------------------------------
#include "winged-edge.h"
#include <iostream>
//-----------------------------------------------------------------------------
using namespace std;
using namespace chai3d;

void addVertex(w_mesh * mesh , vertex * v) {
  mesh->vertices.push_back(v);
}
bool wingedFromMesh(w_mesh * wmesh , cMultiMesh * multimesh) {
    // cout << "start" << endl ;
    int num_meshes = multimesh->getNumMeshes();
    // tranversing all meshes inside the multimesh
    // for (int i = 0 ; i < num_meshes ; i++){
    //     // convering ith mesh object into winged object
    //     cMesh * iMesh = multimesh->getMesh(i);
    //     // getting all verices  , edges and triangles from iMesh
    //     cVertexArrayPtr vertices = iMesh->m_vertices ;
    //     cTriangleArrayPtr triangles = iMesh->m_triangles ;
    //     std::vector<cEdge> edges = iMesh->m_edges;

        // get vertices array
        // cVertexArray * verticesArr = cVertexArrayPtr->get();
        // cout << "start0" << endl ;

        for (int j = 0 ; j < multimesh->getNumVertices(); j++ ){
              // cout << "add0" << endl ;
              cVector3d gpos = multimesh->getVertexPos(j) ;
              // cout << "add1" << endl ;

              vertex *ver ;
              // cout << "add2" << endl ;
              cout << j << endl;
              ver->index = j  ;
              // cout << "add3" << end  l ;

              // addVertex(wmesh,ver);
        }
        cout << "start1" << endl ;
        cout << multimesh->getNumVertices() << endl;
        cout << "start2" << endl ;
        cout << wmesh->vertices.size() << endl ;

        // get edges arrays
        // for (int j = 0 ; j < edges.size(); j++ ){
        //       int v1 = edges[j]->m_vertex0;
        //       int v2 = edges[j]->m_vertex1;
        //
        //       w_edge e = new w_edge();
        //
        //       e->start = w_mesh->vertices[v1] ;
        //       e->end = w_mesh->vertices[v2] ;
        //
        //       w_mesh->edges.push_back(&e) ;
        //
        //
        //       w_mesh->vertices[v1]->edges.push_back(&e)
        //       w_mesh->vertices[v2]->edges.push_back(&e)
        // }

        // get vertices array
        // cTriangleArray * triangleArr = cVertexArrayPtr.get();
        //
        //
        // for (int j = 0 ; j < triangleArr->getNumVertices(); j++ ){
        //
        // }


    // }
    return true ;
};


void clearAllWinged(w_mesh * wmesh){

};
