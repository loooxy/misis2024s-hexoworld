#include <igl/opengl/glfw/Viewer.h>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iterator>

#include<hexoworld/hexoworld.hpp>
#include<hexoworld/texture_grid.hpp>
#include<hexoworld/hexagon_grid.hpp>

Hexoworld generateField() {
  Hexoworld tmp(2.0f, Eigen::Vector3d(-2.0f, -2.0f, 0.0f),
    Eigen::Vector3d(0, 0, -1), Eigen::Vector3d(-1, 0, 0), 6, 6);
  Eigen::Vector4i grass(53, 200, 45, 255);
  Eigen::Vector4i sand(252, 221, 50, 255);
  Eigen::Vector4i sea(0, 100, 255, 255);
  Eigen::Vector4i snow(255, 255, 255, 255);

  Eigen::Matrix<int, 6, 6> colors = (Eigen::MatrixXi(6, 6) <<
    1, 2, 1, 2, 2, 2,
    0, 2, 1, 1, 2, 1,
    0, 1, 1, 1, 3, 0,
    1, 0, 0, 0, 0, 0,
    1, 3, 3, 0, 1, 3,
    2, 3, 3, 3, 0, 2).finished();

  Eigen::Matrix<int, 6, 6> heights = (Eigen::MatrixXi(6, 6) <<
    1, 0, 1, 0, 0, 0,
    2, 0, 1, 1, 0, 1,
    2, 1, 1, 1, 3, 2,
    1, 2, 2, 2, 2, 2,
    1, 3, 3, 2, 1, 3,
    0, 3, 3, 3, 2, 0).finished();

  for (int i = 0; i < 6; ++i)
    for (int j = 0; j < 6; ++j)
    {
      switch (colors(i, j))
      {
      case 0: tmp.set_hex_color(i, j, grass); break;
      case 1: tmp.set_hex_color(i, j, sand); break;
      case 2: tmp.set_hex_color(i, j, sea); break;
      case 3: tmp.set_hex_color(i, j, snow); break;
      }

      tmp.set_hex_height(i, j, heights(i, j));
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
    ans(i / 3, 0) = TriList[i];
    ans(i / 3, 1) = TriList[i + 1];
    ans(i / 3, 2) = TriList[i + 2];
  }
  return ans;
}
int main()
{
  Hexoworld tmp = generateField();

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