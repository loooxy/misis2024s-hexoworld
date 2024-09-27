//
// Created by egorf on 26.09.2024.
//

#ifndef APP_H
#define APP_H
#include <string>


class App {
public:
  explicit App(const std::string *name_) {
    name{name_};
  }

  virtual ~App();

  virtual void openglInit();

  virtual void imguiInit();

  virtual void openglRender();

  virtual void imguiRender();

  virtual void openglDestroy();

  virtual void imguiDestroy();

private:
  std::string name;
};


#endif //APP_H