#include <bigg/bigg.hpp>
#include <bx/string.h>
#include "bx/math.h"
#include <hexoworld/hexoworld.hpp>
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include "GLFW/glfw3.h"



bx::Vec3 conwert_vector(Eigen::Vector3d v) {
	return bx::Vec3(v.x(), v.y(), v.z());
}

Eigen::Vector3d at = { -11.0f, 4.0f,  14.0f };
Eigen::Vector3d eye = { -11.0f, 4.0f, 15.0f };
Eigen::Vector3d rotating_angles = { 0, 0, 0 };
void rotate(Eigen::Vector3d normal, float degree) {
	Eigen::Vector3d a = (at - eye).normalized();
	Eigen::Vector3d b = a.cross(normal).normalized();
	at += a * cos(degree) + b * sin(degree);
}
void move(Eigen::Vector3d direction, float step)
{
	direction.normalize();
	at += direction * step;
	eye += direction * step;
}

bgfx::ShaderHandle loadShader(std::string filename)
{
	std::string path = "../shaders/";

	switch (bgfx::getRendererType()) {
		case bgfx::RendererType::Noop:
		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12: path += "dx11/";  break;
		case bgfx::RendererType::Gnm:        path += "pssl/";  break;
		case bgfx::RendererType::Metal:      path += "metal/"; break;
		case bgfx::RendererType::OpenGL:     path += "glsl/";  break;
		case bgfx::RendererType::OpenGLES:   path += "essl/";  break;
		case bgfx::RendererType::Vulkan:     path += "spirv/"; break;
	}

	path += filename;

	FILE* file = fopen(path.c_str(), "rb");
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	const bgfx::Memory* mem = bgfx::alloc(fileSize + 1);
	fread(mem->data, 1, fileSize, file);
	mem->data[mem->size - 1] = '\0';
	fclose(file);

	return bgfx::createShader(mem);
}

Hexoworld generateField() {
	Hexoworld tmp(2.0f, Eigen::Vector3d(-2.0f, -2.0f, 0.0f),
	  Eigen::Vector3d(0, 0, -1), Eigen::Vector3d(-1, 0, 0), 1, 2, 6, 6);
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

	tmp.add_river({
	  {0, 3}, {0, 2}, {1, 2}, {2, 2}, {3, 2}
	  });

	return tmp;
}


class ExampleCubes : public bigg::Application
{
	void initialize( int _argc, char** _argv )
	{
		Hexoworld tmp = generateField();
		std::vector<PrintingPoint> Vertices;
		std::vector<uint16_t> TriList;
		tmp.print_in_vertices_and_triList(Vertices, TriList);

		bgfx::VertexLayout pcvDecl;
		pcvDecl.begin()
		  .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		  .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		  .end();
		mVbh =
		  bgfx::createVertexBuffer(
			bgfx::makeRef(Vertices.data(),
			  Vertices.size() * sizeof(PrintingPoint)),
			pcvDecl);
		 mIbh =
		  bgfx::createIndexBuffer(
			bgfx::makeRef(TriList.data(),
			  TriList.size() * sizeof(uint16_t)));

		bgfx::ShaderHandle vsh = loadShader("vs_cubes.bin");
		bgfx::ShaderHandle fsh = loadShader("fs_cubes.bin");
		 mProgram = bgfx::createProgram(vsh, fsh, true);

		mTime = 0.0f;
	}

	void onReset()
	{
		bgfx::setViewClear( 0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0 );
		bgfx::setViewRect( 0, 0, 0, uint16_t( getWidth() ), uint16_t( getHeight() ) );
	}

	void update( float dt )
	{
		ImGui::ShowDemoWindow();
		mTime += dt;
		float view[16];
		bx::mtxLookAt(view, conwert_vector(eye), conwert_vector(at));

		float proj[16];
		bx::mtxProj(proj, 60.0f,
		  float(getWidth()) / float(getHeight()),
		  0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
		bgfx::setViewTransform(0, view, proj);


		bgfx::setVertexBuffer(0, mVbh);
		bgfx::setIndexBuffer(mIbh);

		bgfx::submit(0, mProgram);
		bgfx::frame();
	}
public:
	ExampleCubes()
		: bigg::Application( "Cubes", 960, 720 ) {}
private:
	bgfx::ProgramHandle mProgram;
	bgfx::VertexBufferHandle mVbh;
	bgfx::IndexBufferHandle mIbh;
	float mTime;
};

int main( int argc, char** argv )
{
	ExampleCubes app;
	return app.run( argc, argv );
}
