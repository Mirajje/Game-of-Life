#include "field.hpp"

#include <thread-pool.hpp>

#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glut.h>

constexpr std::size_t kWindowHeight = 1000; 
constexpr std::size_t kWindowWidth = 1000;
constexpr std::size_t kPointSize = 2;

static_assert(kWindowHeight != 0 && kWindowWidth != 0 && kPointSize != 0);
static_assert(kWindowHeight % kPointSize == 0 && kWindowWidth % kPointSize == 0);

constexpr std::size_t kHeight = kWindowHeight / kPointSize;
constexpr std::size_t kWidth = kWindowWidth / kPointSize;

static_assert(kHeight % kThreadsAmount == 0);

ThreadPool tp;

constexpr auto day_and_night = [](const std::array<std::array<bool, kWidth>, kHeight>& field, std::size_t y, std::size_t x) {
  std::size_t sum = 0;

  for (std::size_t i = y - 1; i <= y + 1; ++i) {
    for (std::size_t j = x - 1; j <= x + 1; ++j) {
      if (i == y && j == x)
        continue;

      if (i >= 0 && j >= 0 && i < kHeight && j < kWidth && field.at(i).at(j)) {
        ++sum;
      }
    }
  }

  return sum == 3 || sum == 6 || sum == 7 || sum == 8 || (field.at(y).at(x) && sum == 4);
};

constexpr auto game_of_life = [](const std::array<std::array<bool, kWidth>, kHeight>& field, std::size_t y, std::size_t x) {
  std::size_t sum = 0;

  for (std::size_t i = y - 1; i <= y + 1; ++i) {
    for (std::size_t j = x - 1; j <= x + 1; ++j) {
      if (i == y && j == x)
        continue;

      if (i >= 0 && j >= 0 && i < kHeight && j < kWidth && field.at(i).at(j)) {
        ++sum;
      }
    }
  }

  return sum == 3 || (field.at(y).at(x) && sum == 2);
};

Field<kHeight, kWidth> field(game_of_life);

void Render() {
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_POINTS);

  for (std::size_t i = 0; i < kHeight; ++i) {
    for (std::size_t j = 0; j < kWidth; ++j) {
      if (field.Get(i, j)) {
        glVertex2d(2 * static_cast<float>(j) / kWidth - 1, 2 * static_cast<float>(i) / kHeight - 1);
      }
    }
  }

  glEnd();
  glFlush();
}

void MainLoop() {
  for (;;) {
    auto result = tp.AddTasks({{&Field<kHeight, kWidth>::MoveOneStep, field, std::ref(tp)}});
    Render();
    field = std::any_cast<std::array<std::array<bool, kWidth>, kHeight>>(**result.Get()[0]);
  }
}

void Setup() {
  using namespace std::chrono_literals;

  glutMouseFunc([](int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      field.AddCell(kHeight - static_cast<float>(y) / kWindowHeight * kHeight, static_cast<float>(x) / kWindowWidth * kWidth);
      Render();
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
      field.RemoveCell(kHeight - static_cast<float>(y) / kWindowHeight * kHeight, static_cast<float>(x) / kWindowWidth * kWidth);
      Render();
    }
  });

  glutKeyboardFunc([](unsigned char code, int x, int y){
    switch (code) {
      case 114: {
        for (int i = 0; i < kHeight; ++i)
          for (int j = 0; j < kWidth; ++j)
            if (rand() % 2)
              field.AddCell(i, j);
      }
      case 13: {
        glutMouseFunc(NULL);
        MainLoop();
        break;
      }
    }
  });
}
 
int main(int argc, char** argv) {
  tp.ChangeWorkersAmount(kThreadsAmount);

  glutInit(&argc, argv);
  glutInitWindowSize(kWindowWidth, kWindowHeight);
  glutCreateWindow("Game of Life");

  glPointSize(kPointSize);
    
  glutDisplayFunc(Setup);

  glutMainLoop();
}