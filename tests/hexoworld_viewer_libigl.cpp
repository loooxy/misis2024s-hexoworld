#include <igl/opengl/glfw/Viewer.h>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iterator>

#include<hexoworld/hexoworld.hpp>

HexagonGrid generateField() {
  HexagonGrid tmp(2.0f, Eigen::Vector3d(-2.0f, -2.0f, 0.0f),
    Eigen::Vector3d(1, 0, 0), Eigen::Vector3d(0, 0, -1), 9, 9);

  tmp.generate_random_field();
  for (int i = 0; i < 9; ++i)
  {
    tmp.set_height(i, 0, 0);
    tmp.set_height(0, i, 0);
    tmp.set_height(i, 8, 0);
    tmp.set_height(8, i, 0);

    tmp.set_height(i, 1, 0);
    tmp.set_height(1, i, 0);
    tmp.set_height(i, 7, 0);
    tmp.set_height(7, i, 0);
  }
  return tmp;
}

Eigen::MatrixXd MatrixXd_points(std::vector<PrintingPoint>& Vertices) {
  Eigen::MatrixXd ans(Vertices.size(), 3);
  for (auto i = 0; i < Vertices.size(); ++i)
  {
    ans(i, 0) = Vertices[i].x;
    ans(i, 1) = Vertices[i].y;
    ans(i, 2) = Vertices[i].z;
  }
  return ans;
}
Eigen::MatrixXi MatrixXi_tri(std::vector<uint16_t> TriList) {
  Eigen::MatrixXi ans(TriList.size() / 3, 3);
  for (auto i = 0; i < TriList.size(); i += 3)
  {
    ans(i/3, 0) = TriList[i];
    ans(i/3, 1) = TriList[i + 1];
    ans(i/3, 2) = TriList[i + 2];
  }
  return ans;
}
int main()
{  
  HexagonGrid tmp = generateField();

  std::vector<PrintingPoint> Vertices;
  std::vector<uint16_t> TriList;
  tmp.print_in_vertices_and_triList(Vertices, TriList);

  Eigen::MatrixXd V = MatrixXd_points(Vertices);
  Eigen::MatrixXi F = MatrixXi_tri(TriList);
  // Plot the mesh
  igl::opengl::glfw::Viewer viewer;
  viewer.data().set_mesh(V, F);
  viewer.data().set_face_based(true);
  viewer.launch();
}