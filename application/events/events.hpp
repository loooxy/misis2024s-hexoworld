#pragma once
#include <workwithmap/workwithmap.hpp>

enum TypeEvent {
	changeHeight,
	changeColor,
	changeRoadState,
	changeFarmState,
	changeFloodState,
	updateRiver,
	close
};

class Event {
public:
	Event() = default;
	virtual ~Event() = default;

	virtual TypeEvent type() = 0;
	virtual void execute(std::shared_ptr<WorkWithMap> wwm) = 0;
};

class ChangeHeight : public Event {
public:
	ChangeHeight() = default;
	ChangeHeight(const int row, const int col, const int new_height)
		: Event(), row(row), col(col), new_height(new_height) {}
	TypeEvent type() override { return changeHeight; }

	void execute(std::shared_ptr<WorkWithMap> wwm) override { wwm->set_hex_height(row, col, new_height); }

	int row, col, new_height;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(row, col, new_height);
	}
};

class ChangeColor : public Event {
public:
	ChangeColor(const int row, const int col, const int new_color)
		: Event(), row(row), col(col), new_color(new_color) {}
	TypeEvent type() override { return changeColor; }

	void execute(std::shared_ptr<WorkWithMap> wwm) override {
		wwm->set_hex_color(row, col, new_color);
	}

	int row, col, new_color;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(row, col, new_color);
	}
};

class ChangeRoadState : public Event {
public:
	ChangeRoadState(const int row, const int col, const bool road_state)
		: Event(), row(row), col(col), road_state(road_state) {}

	TypeEvent type() override { return changeRoadState; }

	void execute(std::shared_ptr<WorkWithMap> wwm) override {
		wwm->set_road_state_in_hex(row, col, road_state);
	}

	int row, col;
	bool road_state;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(row, col, road_state);
	}
};

class ChangeFarmState : public Event {
public:
	ChangeFarmState(int row, int col, bool farm_state)
		: Event(), row(row), col(col), farm_state(farm_state) {}

	TypeEvent type() override { return changeFarmState; }

	void execute(std::shared_ptr<WorkWithMap> wwm) override {
		wwm->set_farm_state_in_hex(row, col, farm_state);
	}

	int row, col;
	bool farm_state;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(row, col, farm_state);
	}
};

class ChangeFloodState : public Event {
public:
	ChangeFloodState(const int row, const int col, const bool flood_state)
		: Event(), row(row), col(col), flood_state(flood_state) {}

	TypeEvent type() override { return changeFloodState; }

	void execute(std::shared_ptr<WorkWithMap> wwm) override {
		wwm->set_flood_state_in_hex(row, col, flood_state);
	}

	int row, col;
	bool flood_state;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(row, col, flood_state);
	}
};

class UpdateRiver : public Event {
public:
	explicit UpdateRiver() : Event() {}

	TypeEvent type() override { return updateRiver; }

	void execute(std::shared_ptr<WorkWithMap> wwm) override {
		wwm->update_river();
	}
};

class Close : public Event {
public:
	explicit Close() : Event() {}

	TypeEvent type() override { return close; }

	void execute(std::shared_ptr<WorkWithMap> wwm) override {}
};