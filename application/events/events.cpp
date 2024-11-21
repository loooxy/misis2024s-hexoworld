#include "events.hpp"
#include <cereal/archives/portable_binary.hpp>

// ChangeHeight
ChangeHeight::ChangeHeight(const int row, const int col, const int new_height)
	: Event(), row(row), col(col), new_height(new_height) {}

TypeEvent ChangeHeight::type() { 
	return changeHeight;
}

void ChangeHeight::execute(std::shared_ptr<WorkWithMap>& wwm){
	wwm->set_hex_height(row, col, new_height); 
}

template <class Archive>
void ChangeHeight::serialize(Archive& ar) {
	ar(row, col, new_height);
}

// ChangeColor
ChangeColor::ChangeColor(const int row, const int col, const int new_color)
	: Event(), row(row), col(col), new_color(new_color) {}
TypeEvent ChangeColor::type()  {
	return changeColor; 
}

void ChangeColor:: execute(std::shared_ptr<WorkWithMap>& wwm) {
	wwm->set_hex_color(row, col, new_color);
}

template <class Archive>
void ChangeColor::serialize(Archive& ar) {
	ar(row, col, new_color);
}

//ChangeRoadState
ChangeRoadState::ChangeRoadState(const int row, const int col, const bool road_state)
	: Event(), row(row), col(col), road_state(road_state) {}

TypeEvent ChangeRoadState::type() { 
	return changeRoadState;
}

void ChangeRoadState::execute(std::shared_ptr<WorkWithMap>& wwm) {
	wwm->set_road_state_in_hex(row, col, road_state);
}

template <class Archive>
void ChangeRoadState::serialize(Archive& ar) {
	ar(row, col, road_state);
}

// ChangeFarmState
ChangeFarmState::ChangeFarmState(const int row, const int col, const bool farm_state)
: Event(), row(row), col(col), farm_state(farm_state) {}

TypeEvent ChangeFarmState::type() {
	return changeFarmState;
}
void ChangeFarmState::execute(std::shared_ptr<WorkWithMap>& wwm) {
	wwm->set_farm_state_in_hex(row, col, farm_state);
}
template <class Archive>
void ChangeFarmState::serialize(Archive& ar) {
	ar(row, col, farm_state);
}

// ChangeFloodState
ChangeFloodState::ChangeFloodState(const int row, const int col, const bool flood_state)
	: Event(), row(row), col(col), flood_state(flood_state) {}
TypeEvent ChangeFloodState::type() { 
	return changeFloodState;
}
void ChangeFloodState::execute(std::shared_ptr<WorkWithMap>& wwm) {
	wwm->set_flood_state_in_hex(row, col, flood_state);
}

template <class Archive>
void ChangeFloodState::serialize(Archive& ar) {
	ar(row, col, flood_state);
}

// UpdateRiver
UpdateRiver::UpdateRiver() : Event() {}
TypeEvent UpdateRiver::type() {
	return updateRiver; 
}
void UpdateRiver::execute(std::shared_ptr<WorkWithMap>& wwm) {
	wwm->update_river();
}

// Close
Close::Close() : Event() {}
TypeEvent Close::type() {
	return close;
}
void Close::execute(std::shared_ptr<WorkWithMap>& wwm) {}

CEREAL_REGISTER_TYPE(ChangeHeight)
CEREAL_REGISTER_TYPE(ChangeColor)
CEREAL_REGISTER_TYPE(ChangeRoadState)
CEREAL_REGISTER_TYPE(ChangeFarmState)
CEREAL_REGISTER_TYPE(ChangeFloodState)


CEREAL_REGISTER_POLYMORPHIC_RELATION(Event, ChangeHeight)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Event, ChangeColor)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Event, ChangeRoadState)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Event, ChangeFarmState)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Event, ChangeFloodState)