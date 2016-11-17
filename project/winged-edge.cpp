//-----------------------------------------------------------------------------
#include "winged-edge.h"
#include <iostream>
//-----------------------------------------------------------------------------
using namespace std;
using namespace chai3d;

void addVertex(w_mesh * mesh , vertex * v) {
  mesh->vertices.push_back(v);
}
void getVertex(w_mesh * mesh , int i , vertex *& v) {
  v = mesh->vertices[i];
}
int getIndex(cMultiMesh * m_meshes , cMesh * mesh , int verIndex ){
    unsigned int index = 0;
    unsigned int i, numMeshes;
    numMeshes = (unsigned int)(m_meshes->getNumMeshes());
    for (i=0; i<numMeshes; i++)
    {
        cMesh* nextMesh = m_meshes->getMesh(i);
        if (nextMesh == mesh )
        {
            index += verIndex ;
        }
        else{
          index += nextMesh->getNumVertices() ;
    }
  }
    return index ;
}
bool wingedFromMesh(w_mesh *& wmesh , cMultiMesh * multimesh) {
    // cout << "start" << endl ;

        // get vertices array
        // cVertexArray * verticesArr = cVertexArrayPtr->get();
        // cout << "start0" << endl ;

        for (int j = 0 ; j < multimesh->getNumVertices(); j++ ){
              // cout << "add0" << endl ;
              // cVector3d gpos = multimesh->getVertexPos(j) ;
              // cout << "add1" << endl ;
              vertex * ver ;
              ver = new vertex() ;
              // cout << j << endl;
              ver->index = j  ;
              // cout << "add3" << end  l ;
              // mesh->vertices.push_back(ver);
              addVertex(wmesh,ver);
        };
        cout << "vertex done!" << endl ;
        // get Triangle array
        for (int j = 0 ; j < multimesh->getNumTriangles(); j++ ){
            face * fc ;
            fc = new face() ;
            fc->index = j ;
            /// Adding  vertices in face object ;
            unsigned int index =1  ;
            cMesh * mesh ;
            multimesh->getTriangle(j , mesh ,index) ;
            cTriangleArray * triangles = ((std::shared_ptr< cTriangleArray >)(mesh->m_triangles)).get() ;
            // cout << "index : " << index << " " << mesh << endl ;
            unsigned int v1 , v2 , v0 ;
            v0 = triangles->getVertexIndex0(index) ;
            v1 = triangles->getVertexIndex1(index) ;
            v2 = triangles->getVertexIndex2(index) ;
            // cout << "vetex in Triangle : " << v0 << " "  << v1 << " " << v2 << endl ;
            v0 = getIndex(multimesh , mesh , v0 );
            v1 = getIndex(multimesh , mesh , v1 );
            v2 = getIndex(multimesh , mesh , v2 );
            // cout << "vetex in multimesh : " << v0 << " "  << v1 << " " << v2 << endl ;
            fc->vertices.push_back(wmesh->vertices[v0]);
            fc->vertices.push_back(wmesh->vertices[v1]);
            fc->vertices.push_back(wmesh->vertices[v2]);
            ///
            wmesh->faces.push_back(fc) ;
        };
        cout << "triangles done!" << endl ;


        int num_meshes = multimesh->getNumMeshes();
        // tranversing all meshes inside the multimesh
        // cout << "edges ";
        // cout <<  num_meshes << endl ;
        for (int i = 0 ; i < num_meshes ; i++){
          //  cout << "edge: " ;
          //  cout << i << endl ;

            // convering ith mesh object into winged object
            cMesh * iMesh = multimesh->getMesh(i);
            // getting all verices  , edges and triangles from iMesh
            // cVertexArrayPtr vertices = iMesh->m_vertices ;
            // cTriangleArrayPtr triangles = iMesh->m_triangles ;
            // cout << "start" << endl ;
            iMesh->computeAllEdges(40.0) ;
            std::vector<cEdge> edges = iMesh->m_edges;

            // get edges arrays
            // cout << "size" << endl ;
            // cout << edges.size() << endl ;
            for (int j = 0 ; j < edges.size(); j++ ){
                  int vv1 = edges[j].m_vertex0;
                  int vv2 = edges[j].m_vertex1;

                  int v1 = getIndex(multimesh , iMesh , vv1);
                  int v2 = getIndex(multimesh , iMesh , vv2);

                  w_edge * e ;
                  e = new w_edge();

                  // getVertex(wmesh,v1,e->start );
                  e->start = wmesh->vertices[v1] ;
                  e->end = wmesh->vertices[v2] ;

                  wmesh->edges.push_back(e) ;

                  wmesh->vertices[v1]->edges.push_back(e);
                  wmesh->vertices[v2]->edges.push_back(e);


            }

        }

        // adding adjacent edges
        //
        cout << "edges : "  << wmesh->edges.size() << endl;
        for (int i = 0 ; i < wmesh->edges.size() ; i++){
            vertex * v1 = wmesh->edges[i]->start ;
            vertex * v2 = wmesh->edges[i]->end ;
            int ct = 0 ;
            for (int j = 0 ; j < v1->edges.size(); j++){
                if(v1->edges[j] != wmesh->edges[i]){
                  if(ct= 0 ){
                    wmesh->edges[i]->left_prev = v1->edges[j] ;
                    ct++;
                  }
                  else{
                    wmesh->edges[i]->right_prev = v1->edges[j] ;
                    break;
                  }
                }
            }
            ct = 0;
            for (int j = 0 ; j < v2->edges.size(); j++){
                if(v2->edges[j] != wmesh->edges[i]){
                  if(ct= 0 ){
                    wmesh->edges[i]->left_next = v2->edges[j] ;
                    ct++;
                  }
                  else{
                    wmesh->edges[i]->right_next = v2->edges[j] ;
                    break;
                  }
                }
            }
        }

        cout << "adjacent edges done!" << endl ;
        //////////////////////////////////////////////
        // // adding adjacent faces
        // cout << "edges to faces : " << wmesh->edges.size() << endl ;
        for (int i = 0 ; i < wmesh->edges.size() ; i++){
            // cout << "i : " << i << endl ;
            vertex * v1 = wmesh->edges[i]->start ;
            vertex * v2 = wmesh->edges[i]->end ;
            bool first = false ;
            for (int j = 0 ; j < wmesh->faces.size() ; j++){
                vertex * tv1 = wmesh->faces[j]->vertices[0];
                vertex * tv2 = wmesh->faces[j]->vertices[1];
                vertex * tv3 = wmesh->faces[j]->vertices[2];

                bool match = ((! (v1 == tv1) ) && (!(v1==tv2) ) && (!(v1==tv3))) || ((!(v2==tv1)) && (!(v2==tv2)) && (!(v2==tv3))) ;

                // note : check before adding edges ;
                if(!match){
                    if(!first){
                        wmesh->edges[i]->left = wmesh->faces[j] ;
                    }
                    else {
                      wmesh->edges[i]->right = wmesh->faces[j] ;
                      break ;
                    }
                }
            }
        }
        cout << "adjacent faces done!" << endl ;

    return true ;
};

void clearAllWinged(w_mesh * wmesh){

};
