#include "command.hpp"
#include <cereal/archives/portable_binary.hpp>

CEREAL_REGISTER_TYPE(MovementCommand)
CEREAL_REGISTER_TYPE(RotationCommand)

CEREAL_REGISTER_POLYMORPHIC_RELATION(Command, MovementCommand)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Command, RotationCommand)

MovementCommand::MovementCommand(const CameraMovement dir, const float deltaTime) : Command(), dir(dir), deltaTime(deltaTime) {};
RotationCommand::RotationCommand(const float xoffset, const float yoffset) : Command(), xoffset(xoffset), yoffset(yoffset) {};

void MovementCommand::execute(Camera& cam) {
  cam.ProcessKeyboard(dir, deltaTime);
}

void RotationCommand::execute(Camera& cam) {
  cam.ProcessMouseMovement(xoffset, yoffset);
}

std::shared_ptr<Command> loadCommand(const std::string& data) {
  std::shared_ptr<Command> com;
  std::istringstream iss(data);
  cereal::PortableBinaryInputArchive archive(iss);
  archive(com);
  return com;
}

std::string saveCommand(const std::shared_ptr<Command>& com) {
  std::ostringstream oss;
  cereal::PortableBinaryOutputArchive archive(oss);
  archive(com);
  return oss.str();
}


