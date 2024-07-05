//
// Created by egorf on 01.07.2024.
//
#include <iostream>
#include <set>
#include "src/hexagon.cpp"
#include <Eigen/Dense>
#include <igl/opengl/glfw/Viewer.h>
#include <Eigen/Core>

int main() {
  Hex central_hex(0, 0, 0);
  vector<Hex> hexes;
  hexes.push_back(central_hex);

  for (int i = 0; i < 6; ++i) {
    hexes.push_back(hex_neighbor(central_hex, i));
    hexes.push_back(hex_diagonal_neighbor(central_hex, i));
  }
  for (const auto& hex : hexes) {
    std::cout << "Hex coordinates: (" << hex.q << ", " << hex.r << ", " << hex.s << ")" << std::endl;
  }

  Layout layout = Layout(
    Orientation(3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0), 2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0, 0.0),
    Point(10.0, 10.0),
    Point(50.0, 50.0)
  );

  std::set<Point> all_vertices;
  for (const auto& hex : hexes) {
    all_vertices.insert(hex_to_pixel(layout, hex));
    all_vertices.insert(hex_to_pixel(layout, hex));
    all_vertices.insert(hex_to_pixel(layout, hex));
    all_vertices.insert(hex_to_pixel(layout, hex));
    all_vertices.insert(hex_to_pixel(layout, hex));
    all_vertices.insert(hex_to_pixel(layout, hex));
  }

  Eigen::MatrixXd V;
  Eigen::MatrixXi F;

  V.conservativeResize(all_vertices.size(), 2);
  F.conservativeResize(all_vertices.size(), 2);
  int i = 0;
  for (auto& vertex : all_vertices) {
    V.row(i) << vertex.x, vertex.y;
    i++;
  }

  unsigned int index = 0;
  for (size_t i = 0; i < all_vertices.size(); i += 3) {
    F.row(index++) << i, i + 1, i + 2;
  }
}