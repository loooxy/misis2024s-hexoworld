#pragma once
#include <workwithmap/workwithmap.hpp>
#include <cereal/types/polymorphic.hpp>

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
	virtual void execute(std::shared_ptr<WorkWithMap>& wwm) = 0;
};

class ChangeHeight : public Event {
public:
	ChangeHeight() = default;
	ChangeHeight(const int row, const int col, const int new_height);
	TypeEvent type() override;

	void execute(std::shared_ptr<WorkWithMap>& wwm);

	int row = 0;
	int col = 0;
	int new_height = 0;

	template <class Archive>
	void serialize(Archive& ar);
};

class ChangeColor : public Event {
public:
	ChangeColor() = default;
	ChangeColor(const int row, const int col, const int new_color);
	TypeEvent type() override;
	void execute(std::shared_ptr<WorkWithMap>& wwm) override;

	int row = 0;
	int col = 0; 
	int new_color = 0;

	template <class Archive>
	void serialize(Archive& ar);
};

class ChangeRoadState : public Event {
public:
	ChangeRoadState() = default;
	ChangeRoadState(const int row, const int col, const bool road_state);
	TypeEvent type() override;
	void execute(std::shared_ptr<WorkWithMap>& wwm) override;

	int row = 0;
	int col = 0;
	bool road_state = false;

	template <class Archive>
	void serialize(Archive& ar);
};

class ChangeFarmState : public Event {
public:
	ChangeFarmState() = default;
	ChangeFarmState(const int row, const int col, const bool farm_state);
	TypeEvent type() override;
	void execute(std::shared_ptr<WorkWithMap>& wwm) override;

	int row = 0; 
	int col = 0;
	bool farm_state = false;

	template <class Archive>
	void serialize(Archive& ar);
};

class ChangeFloodState : public Event {
public:
	ChangeFloodState() = default;
	ChangeFloodState(const int row, const int col, const bool flood_state);
	TypeEvent type() override;
	void execute(std::shared_ptr<WorkWithMap>& wwm) override;

	int row = 0;
	int col = 0;
	bool flood_state = false;;

	template <class Archive>
	void serialize(Archive& ar);
};

class UpdateRiver : public Event {
public:
	explicit UpdateRiver();
	TypeEvent type() override;
	void execute(std::shared_ptr<WorkWithMap>& wwm) override;
};

class Close : public Event {
public:
	explicit Close();
	TypeEvent type() override;
	void execute(std::shared_ptr<WorkWithMap>& wwm) override;
};

std::shared_ptr<Event> loadEv(const std::string& data);

std::string saveEv(const std::shared_ptr<Event>& ev);