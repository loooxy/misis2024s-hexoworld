#pragma once
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <camera/camera.hpp>

#include <string>

class Command {
public:
  Command() = default;
  ~Command() = default;
  virtual void execute(Camera& cam) = 0;
};

class MovementCommand : public Command{ 
public:
  MovementCommand() = default;
  MovementCommand(const CameraMovement dir, const float deltaTime);
  void execute(Camera& cam) override;

  CameraMovement dir = FORWARD;
  float deltaTime = 0;

  template <class Archive>
  void serialize(Archive& ar) {
    ar(dir, deltaTime);
  }
};

class RotationCommand : public Command {
public:
  RotationCommand() = default;
  RotationCommand(const float xoffset, const float yoffset);
  void execute(Camera& cam) override;

  float xoffset = 0;
  float yoffset = 0;

  template <class Archive>
  void serialize(Archive& ar) {
    ar(xoffset, yoffset);
  }
};

std::shared_ptr<Command> loadCommand(const std::string& data);
std::string saveCommand(const std::shared_ptr<Command>& com);



