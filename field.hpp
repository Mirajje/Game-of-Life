#pragma once

#include <array>
#include <functional>

template <std::size_t N, std::size_t M>
class Field {
  public:
    Field(std::function<bool(const Field<N, M>&, std::size_t y, std::size_t x)>&& f)
      : rule(f) {}

    void AddCell(std::size_t y, std::size_t x) {
      field_[y][x] = true;
    }

    void MoveOneStep() {
      std::array<std::array<bool, M>, N> new_field;
      for (std::size_t i = 0; i < N; ++i)
        for (std::size_t j = 0; j < M; ++j) {
          new_field[i][j] = rule(*this, i, j); 
        }
      field_ = std::move(new_field);
    }

    bool Get(std::size_t y, std::size_t x) const {
      return field_[y][x];
    }

  private:
    std::function<bool(const Field<N, M>&, std::size_t y, std::size_t x)> rule;
    std::array<std::array<bool, M>, N> field_;
};