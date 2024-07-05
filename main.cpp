#include <igl/opengl/glfw/Viewer.h>

static float size = 1.0f;
//x = v
static float v = sqrt(3) * size;
//y = h
static float h = 1.5f * size;

int main(int argc, char *argv[])
{
  // Inline mesh of a hex
  const Eigen::MatrixXd V= (Eigen::MatrixXd(7,3)<<
    1.0f * v, 0.5f * h, 0.0f, //centr 0 - 0
    1.0f * v, 0.0f, 0.0f, // 1 - 1
    1.0f * v, 1.0f * h, 0.0f, // 4 - 2
    0.5f * v, 0.25f * h, 0.0f, // 6 - 3
    1.5f * v, 0.25f * h, 0.0f, // 2 - 4
    0.5f * v, 0.75f * h, 0.0f, // 5 - 5
    1.5f * v, 0.75f * h, 0.0f // 3 - 6
    ).finished();
  const Eigen::MatrixXi F = (Eigen::MatrixXi(6,3)<<
    0,1,4,
    0,4,6,
    0,6,2,
    0,2,5,
    0,5,3,
    0,3,1
    ).finished();

  // Plot the mesh
  igl::opengl::glfw::Viewer viewer;
  viewer.data().set_mesh(V, F);
  viewer.data().set_face_based(true);
  viewer.launch();
}
