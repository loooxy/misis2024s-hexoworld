#include <igl/opengl/glfw/Viewer.h>
#include "src/hexagon.cpp"

int main(int argc, char* argv[])
{
  // Inline mesh of a hex


  Eigen::MatrixXd V = (Hex_Points({1.0f,0.0f}, 0)).HexMatrix_from_points();
  const Eigen::MatrixXi F = (Eigen::MatrixXi(6, 3) <<
    0, 1, 2,
    0, 2, 3,
    0, 3, 4,
    0, 4, 5,
    0, 5, 6,
    0, 6, 1
    ).finished();

  // Plot the mesh
  igl::opengl::glfw::Viewer viewer;
  viewer.data().set_mesh(V, F);
  viewer.data().set_face_based(true);
  viewer.launch();
}
