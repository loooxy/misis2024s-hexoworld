#include <igl/opengl/glfw/Viewer.h>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iterator>
#include "src/hexagon.cpp"



int main(int argc, char* argv[])
{
  /*Inline mesh of a hex*/
  Hex_Points P = Hex_Points({ 0.0,0.0,0.0 });
  Hex_Points P1 = Hex_Points({ 4.0,5.0,0.0 });
  Hex_map map;
  map.Append(P);
  map.Append(P1);
  Eigen::Matrix3d V = map.Matrixd_points();
  Eigen::Matrix3i F = map.Matrixi_tri();
  // Plot the mesh
  igl::opengl::glfw::Viewer viewer;
  viewer.data().set_mesh(V, F);
  viewer.data().set_face_based(true);
  viewer.launch();
}
