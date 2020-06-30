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

// Test compile-time constant splits that divide the extents of an array.
TEST(split_even_constant) {
  dense_array<int, 2> a({8, 9});
  for (auto yo : split<3>(a.y())) {
    for (auto xo : split<4>(a.x())) {
      auto a_inner = a(xo, yo);
      // Compile-time constant splits should always be the same size.
      ASSERT_EQ(a_inner.size(), 12);
      fill_pattern(a_inner);
    }
  }
  check_pattern(a);
}

// Test compile-time constant splits that do not divide the extents of an array.
TEST(split_uneven_constant) {
  dense_array<int, 2> a({8, 9});
  for (auto yo : split<4>(a.y())) {
    for (auto xo : split<5>(a.x())) {
      auto a_inner = a(xo, yo);
      // Compile-time constant splits should always be the same size.
      ASSERT_EQ(a_inner.size(), 20);
      fill_pattern(a_inner);
    }
  }
  check_pattern(a);
}

// Test splits that divide the extents of an array.
TEST(split_even_nonconstant) {
  dense_array<int, 2> a({8, 9});
  index_t total_size = 0;
  for (auto yo : split(a.y(), 3)) {
    for (auto xo : split(a.x(), 4)) {
      auto a_inner = a(xo, yo);
      total_size += a_inner.size();
      fill_pattern(a_inner);
    }
  }
  // The total number of items in the inner splits should be equal to the size
  // of the array (no overlap among inner splits).
  ASSERT_EQ(total_size, a.size());
  check_pattern(a);
}

// Test splits that do not divide the extents of an array.
TEST(split_uneven_nonconstant) {
  dense_array<int, 2> a({8, 9});
  index_t total_size = 0;
  for (auto yo : split(a.y(), 4)) {
    for (auto xo : split(a.x(), 5)) {
      auto a_inner = a(xo, yo);
      total_size += a_inner.size();
      fill_pattern(a_inner);
    }
  }
  // The total number of items in the inner splits should be equal to the size
  // of the array (no overlap among inner splits).
  ASSERT_EQ(total_size, a.size());
  check_pattern(a);
}

}  // namespace nda