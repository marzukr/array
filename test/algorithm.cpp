// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "array.h"
#include "test.h"

namespace nda {

TEST(algorithm_equal) {
  dense_array<int, 3> a1({10, 20, {0, 30, 205}});
  generate(a1, rand);
  dense_array<int, 3> a2 = make_compact_copy(a1);
  dense_array<int, 3> b(a2);
  fill(b, 0);

  ASSERT(a1 != a2);
  ASSERT(equal(a1, a2));
  ASSERT(!equal(a1, b));
}

#ifndef NDARRAY_NO_EXCEPTIONS
const int copy_crop_tests[] = {0, 1, -1};
#define TRY try
#define CATCH_OUT_OF_RANGE                                                                         \
  catch (std::out_of_range&) {                                                                     \
  }
#else
const int copy_crop_tests[] = {0, 1};
#define TRY
#define CATCH_OUT_OF_RANGE
#endif

TEST(algorithm_copy) {
  array_of_rank<int, 2> a({10, 20});
  generate(a, rand);

  int succeeded = 0;
  for (int crop_min : copy_crop_tests) {
    for (int crop_max : copy_crop_tests) {
      int x_min = a.shape().x().min() + crop_min;
      int x_max = a.shape().x().max() - crop_max;
      int y_min = a.shape().y().min() + crop_min;
      int y_max = a.shape().y().max() - crop_max;
      shape_of_rank<2> b_shape({{x_min, x_max - x_min + 1}, {y_min, y_max - y_min + 1}});
      array_of_rank<int, 2> b(b_shape);

      TRY {
        copy(a, b);
        ASSERT(equal(a(b.x(), b.y()), b));
        succeeded++;
      }
      CATCH_OUT_OF_RANGE;
    }
  }
  ASSERT_EQ(succeeded, 4);
}

TEST(algorithm_move) {
  array_of_rank<int, 2> a({10, 20});
  generate(a, rand);

  int succeeded = 0;
  for (int crop_min : copy_crop_tests) {
    for (int crop_max : copy_crop_tests) {
      int x_min = a.shape().x().min() + crop_min;
      int x_max = a.shape().x().max() - crop_max;
      int y_min = a.shape().y().min() + crop_min;
      int y_max = a.shape().y().max() - crop_max;
      shape_of_rank<2> b_shape({{x_min, x_max - x_min + 1}, {y_min, y_max - y_min + 1}});
      array_of_rank<int, 2> b(b_shape);

      TRY {
        move(a, b);
        // The lifetime of moved elements is tested in array_lifetime.cpp.
        ASSERT(equal(a(b.x(), b.y()), b));
        succeeded++;
      }
      CATCH_OUT_OF_RANGE;
    }
  }
  ASSERT_EQ(succeeded, 4);
}

TEST(algorithm_copy_scalar) {
  array_of_rank<int, 0> a;
  generate(a, rand);

  array_of_rank<int, 0> b;
  copy(a, b);
  ASSERT(a == b);
}

TEST(algorithm_move_scalar) {
  array_of_rank<int, 0> a;
  generate(a, rand);

  array_of_rank<int, 0> b;
  copy(a, b);
  ASSERT(a == b);
}

} // namespace nda
