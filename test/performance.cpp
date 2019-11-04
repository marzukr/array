#include "array.h"
#include "test.h"
#include "performance.h"

#include <iostream>

namespace array {

int pattern(int x, int y, int c) {
  return y * 10000 + x * 1000 + c;
}

template <typename T, typename Shape>
void fill_pattern(array<T, Shape>& a) {
  for_all_indices(a.shape(), [&](int x, int y, int c) {
    a(x, y, c) = pattern(x, y, c);
  });
}

template <typename T, typename Shape>
void check_pattern(const array<T, Shape>& a) {
  for_all_indices(a.shape(), [&](int x, int y, int c) {
    ASSERT_EQ(a(x, y, c), pattern(x, y, c));
  });
}

TEST(performance_dense_copy) {
  dense_array<int, 3> a({100, 100, 100}, 3);
  fill_pattern(a);
  dense_array<int, 3> b(a.shape());
  double copy_time = benchmark([&]() {
    copy(a, b);
  });
  check_pattern(b);

  dense_array<int, 3> c(b.shape());
  double memcpy_time = benchmark([&] {
    memcpy(&c(0, 0, 0), &a(0, 0, 0), a.size() * sizeof(int));
  });
  check_pattern(c);

  // copy should be about as fast as memcpy.
  ASSERT_LT(copy_time, memcpy_time * 1.2);
}

TEST(performance_dense_cropped_copy) {
  dense_array<int, 3> a({100, 100, 100});
  fill_pattern(a);

  dense_array<int, 3> b({dense_dim<>(1, 98), dim<>(1, 98), dim<>(1, 98)});
  double copy_time = benchmark([&]() {
    copy(a, b);
  });
  check_pattern(b);

  dense_array<int, 3> c(b.shape());
  double memcpy_time = benchmark([&] {
    for (int z : c.z()) {
      for (int y : c.y()) {
	memcpy(&c(c.x().min(), y, z), &a(c.x().min(), y, z),
	       c.x().extent() * sizeof(int));
      }
    }
  });
  check_pattern(c);

  // copy should be about as fast as memcpy.
  ASSERT_LT(copy_time, memcpy_time * 1.2);
}

TEST(performance_copy) {
  array_of_rank<int, 3> a({dim<>(0, 100, 10000), dim<>(0, 100, 100), dim<>(0, 100, 1)});
  fill_pattern(a);

  array_of_rank<int, 3> b(a.shape());
  double copy_time = benchmark([&]() {
    copy(a, b);
  });
  check_pattern(b);

  array_of_rank<int, 3> c(b.shape());
  double loop_time = benchmark([&] {
    for (int z : c.z()) {
      for (int y : c.y()) {
	for (int x : c.x()) {
	  c(x, y, z) = a(x, y, z);
	}
      }
    }
  });
  check_pattern(c);

  // copy should be faster than badly ordered loops.
  ASSERT_LT(copy_time, loop_time * 0.5);
}

TEST(performance_for_each_value) {
  array_of_rank<int, 3> a({dim<>(0, 100, 10000), dim<>(0, 100, 100), dim<>(0, 100, 1)});
  int counter = 0;
  double loop_time = benchmark([&]() {
    for (int z : a.z()) {
      for (int y : a.y()) {
	for (int x : a.x()) {
	  a(x, y, z) = counter++;
	}
      }
    }
  });
  assert_used(a);

  counter = 0;
  array_of_rank<int, 3> b(a.shape());
  double for_each_value_time = benchmark([&]() {
    b.for_each_value([&](int& x) { x = counter++; });
  });
  assert_used(b);

  // The optimized for_each_value should be quite a bit faster.
  ASSERT_LT(for_each_value_time, loop_time * 0.5);
}

}  // namespace array
