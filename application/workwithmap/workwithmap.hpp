#pragma once
#include <hexoworld/includes.hpp>
#include <data_pool/data_pool.hpp>

class WorkWithMap {
public:
	WorkWithMap();
	~WorkWithMap();

	int get_n_cols();
	int get_n_rows();
	Eigen::Vector4i get_color(int color_id);
	int get_id_color(Eigen::Vector4i color);
	int heights(int row, int col);
	int colors(int row, int col);
	bool roads(int row, int col);
	bool farms(int row, int col);
	bool flood(int row, int col);

	void set_hex_height(int row, int col, int new_height);
	void set_hex_color(int row, int col, int color_id);
	void set_road_state_in_hex(int row, int col, bool road_state);
	void set_farm_state_in_hex(int row, int col, bool farm_state);
	void set_flood_state_in_hex(int row, int col, bool flood_state);
	void update_river();
	
	void get_data(data_pool& data);

	enum ColorsName
	{
		se, sa, gr, mo, sn, te, Colors_COUNT
	};
	const char* elems_names[6];

private:
	void generateField(std::shared_ptr<Hexoworld>& map);

	std::shared_ptr<Hexoworld> map;
	int n_rows, n_cols;

	Eigen::Vector4i grass;
	Eigen::Vector4i sand;
	Eigen::Vector4i sea;
	Eigen::Vector4i snow;
	Eigen::Vector4i mount;
	Eigen::Vector4i test;
};