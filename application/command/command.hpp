#pragma once

struct Command {
  int dir;
  int deltaTime;

  template <class Archive>
  void serialize(Archive& ar) {
    ar(dir, deltaTime);
  }

  template <class Archive>
  void serialize(Archive& ar) const {
    ar(dir, deltaTime);
  }
};