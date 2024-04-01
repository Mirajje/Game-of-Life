#pragma once

#include <thread-pool.hpp>

#include <array>
#include <functional>

inline constexpr std::size_t kThreadsAmount = 2;

template <std::size_t N, std::size_t M>
class Field {
  public:
    Field(const std::function<bool(const std::array<std::array<bool, M>, N>&, std::size_t y, std::size_t x)>& f)
      : rule(f) {}

    Field<N, M>& operator=(std::array<std::array<bool, M>, N>&& field) {
      field_ = std::move(field);
      
      return *this;
    }

    void AddCell(std::size_t y, std::size_t x) {
      field_[y][x] = true;
    }

    void RemoveCell(std::size_t y, std::size_t x) {
      field_[y][x] = false;
    }

    std::array<std::array<bool, M>, N> MoveOneStep(ThreadPool& tp) {
      std::array<std::array<bool, M>, N> new_field;
      std::vector<Data> results;
      results.reserve(kThreadsAmount);

      for (std::size_t thread_num = 0; thread_num < kThreadsAmount; ++thread_num) {
        results.push_back(tp.AddTasks({{[&new_field, this](std::size_t i){
          std::size_t until = i + N / kThreadsAmount;

          for (; i < until; ++i) {
            for (std::size_t j = 0; j < M; ++j) {
              new_field[i][j] = rule(field_, i, j);
            }
          }
        }, thread_num * N / kThreadsAmount}}));
      }

      for (auto& result : results)
        result.Get();

      return new_field;
    }

    bool Get(std::size_t y, std::size_t x) const {
      return field_[y][x];
    }

  private:
    std::function<bool(const std::array<std::array<bool, M>, N>&, std::size_t y, std::size_t x)> rule;
    std::array<std::array<bool, M>, N> field_;
};