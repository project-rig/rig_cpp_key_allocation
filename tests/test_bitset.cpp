#include <gtest/gtest.h>
#include "bitset.h"


class BitSetTest : public ::testing::Test
{
};


TEST(BitSetTest, test_add_remove_and_contains)
{
  // Test adding an element to a bitset and then checking that it is the only
  // element contained within the set.
  auto b = BitSet::BitSet(65);
  b.Add(1);
  b.Add(65);

  // Check that 1 and 65 are the only elements contained by the set
  for (unsigned int i = 0; i < 66; i++)
  {
    if (i == 1 || i == 65)
    {
      ASSERT_TRUE(b.Contains(i));
    }
    else
    {
      ASSERT_FALSE(b.Contains(i));
    }
  }

  // Remove one of these elements and add another
  b.Remove(65);
  b.Add(32);

  // Check that 1 and 32 are the only elements contained by the set
  for (unsigned int i = 0; i < 66; i++)
  {
    if (i == 1 || i == 32)
    {
      ASSERT_TRUE(b.Contains(i));
    }
    else
    {
      ASSERT_FALSE(b.Contains(i));
    }
  }
}


TEST(BitSetTest, test_any)
{
  // Test that the emptiness of a bitset is reported correctly.
  auto b = BitSet::BitSet(65);

  ASSERT_FALSE(b.Any());  // Bit set should be empty

  b.Add(0);
  ASSERT_TRUE(b.Any());  // No longer empty

  b.Remove(0);
  ASSERT_FALSE(b.Any());

  b.Add(64);
  ASSERT_TRUE(b.Any());
}


TEST(BitSetTest, test_count)
{
  // Test that the emptiness of a bitset is reported correctly.
  auto b = BitSet::BitSet(128);

  for (unsigned int i = 0; i < 128; i++)
  {
    ASSERT_EQ(b.Count(), i);
    b.Add(i);
  }
}


TEST(BitSetTest, test_fill_on_instantiate)
{
  // Test that a bit set can be filled with ones when created
  auto a = BitSet::BitSet(129, true);

  ASSERT_EQ(a.Count(), 129);

  for (unsigned int i = 0; i < 129; i++)
  {
    ASSERT_TRUE(a.Contains(i));
  }

  // Fill with zeros (clear)
  a.Fill(false);
  ASSERT_EQ(a.Count(), 0);
}


TEST(BitSetTest, test_is_disjoint)
{
  // Test that two bit sets can be compared for disjointness
  auto a = BitSet::BitSet(128);
  auto b = BitSet::BitSet(128);

  // Start with a and b being disjoint
  a.Add(0);
  a.Add(64);
  b.Add(1);
  b.Add(65);

  ASSERT_TRUE(a.IsDisjoint(b));
  ASSERT_TRUE(b.IsDisjoint(a));  // Should be symmetrical

  // Change so that a and b are no longer disjoint in the first word
  a.Add(1);
  ASSERT_FALSE(a.IsDisjoint(b));
  ASSERT_FALSE(b.IsDisjoint(a));

  // Change so that a and b are no longer disjoint in the second word
  a.Remove(1);
  b.Add(64);
  ASSERT_FALSE(a.IsDisjoint(b));
  ASSERT_FALSE(b.IsDisjoint(a));
}


TEST(BitSetTest, test_foreach)
{
  // Add a set of elements to a bit set, then loop over them using ForEach and
  // store them in a vector, ensure that the vector is as expected.
  static const unsigned int elems[] = {1, 31, 34, 64, 65, 123};

  const auto expected = std::vector<unsigned int>(
      elems, elems + (sizeof(elems) / sizeof(elems[0]))
  );

  auto b = BitSet::BitSet(128);
  for (const auto v : expected)
  {
    b.Add(v);
  }

  // Loop over the bit set and store the retrieved elements
  auto result = std::vector<unsigned int>();
  b.ForEach([&result] (const unsigned int v) {result.push_back(v);});

  // Check that the expected elements were read out
  ASSERT_EQ(expected, result);
}


TEST(BitSetTest, test_difference_foreach)
{
  // Create the first bit set
  auto a = BitSet::BitSet(128);

  static const unsigned int elems_a[] = {4, 31, 34, 64, 65, 123, 127};
  for (unsigned int i = 0; i < sizeof(elems_a) / sizeof(elems_a[0]); i++)
  {
    a.Add(elems_a[i]);
  }

  // Create the second bit set
  auto b = BitSet::BitSet(128);

  static const unsigned int elems_b[] = {2, 31, 65, 127};
  for (unsigned int i = 0; i < sizeof(elems_b) / sizeof(elems_b[0]); i++)
  {
    b.Add(elems_b[i]);
  }

  // Build a vector to represent the difference
  static const unsigned int diff[] = {4, 34, 64, 123};
  const auto expected = std::vector<unsigned int>(
      diff, diff + (sizeof(diff) / sizeof(diff[0]))
  );

  // Loop over the difference, storing the result
  auto result = std::vector<unsigned int>();
  a.DifferenceForEach(
    b, [&result] (const unsigned int v) {result.push_back(v);}
  );

  // Check that the result is as expected
  ASSERT_EQ(expected, result);
}


TEST(BitSetTest, test_difference_foreach_different_lengths)
{
  // Create the first bit set
  auto a = BitSet::BitSet(128);

  static const unsigned int elems_a[] = {4, 31, 34, 64, 65, 123, 127};
  for (unsigned int i = 0; i < sizeof(elems_a) / sizeof(elems_a[0]); i++)
  {
    a.Add(elems_a[i]);
  }

  // Create the second bit set
  auto b = BitSet::BitSet(65);

  static const unsigned int elems_b[] = {2, 31, 64};
  for (unsigned int i = 0; i < sizeof(elems_b) / sizeof(elems_b[0]); i++)
  {
    b.Add(elems_b[i]);
  }

  // Build a vector to represent the difference
  static const unsigned int diff[] = {4, 34, 65, 123, 127};
  const auto expected = std::vector<unsigned int>(
      diff, diff + (sizeof(diff) / sizeof(diff[0]))
  );

  // Loop over the difference, storing the result
  auto result = std::vector<unsigned int>();
  a.DifferenceForEach(
    b, [&result] (const unsigned int v) {result.push_back(v);}
  );

  // Check that the result is as expected
  ASSERT_EQ(expected, result);
}
