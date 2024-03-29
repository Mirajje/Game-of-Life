#include "field.hpp"

#include <thread>

#include <thread-pool.hpp>

#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glut.h>

constexpr std::size_t kWindowHeight = 1000; 
constexpr std::size_t kWindowWidth = 1000;
constexpr std::size_t kPointSize = 50.0;

static_assert(kWindowHeight != 0 && kWindowWidth != 0 && kPointSize != 0);
static_assert(kWindowHeight % kPointSize == 0 && kWindowWidth % kPointSize == 0);

constexpr std::size_t kHeight = kWindowHeight / kPointSize;
constexpr std::size_t kWidth = kWindowWidth / kPointSize;

Field<kHeight, kWidth> field([](const Field<kHeight, kWidth>& field, std::size_t y, std::size_t x) {
  return field.Get(y, x) && y + x < 5;
});

void DrawField() {
  using namespace std::chrono_literals;

  for (;;) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS);

    for (std::size_t i = 0; i < kHeight; ++i) {
      for (std::size_t j = 0; j < kWidth; ++j) {
        if (field.Get(i, j)) {
          glVertex2d((float) j / kWidth, (float) i / kHeight);
        }
      }
    }

    glEnd();
    glFlush();
    field.MoveOneStep();
    std::this_thread::sleep_for(1s);
  }
}
 
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(kWindowWidth, kWindowHeight);
  glutCreateWindow("Game of Life");

  for (int i = 0; i < 20; ++i)
    field.AddCell(0, i);

  glPointSize(kPointSize);

  glutDisplayFunc(DrawField);

  glutMainLoop();
  return 0;
}