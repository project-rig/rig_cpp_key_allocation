#pragma once

#include <cstdint>
#include <vector>

namespace BitSet
{
  class BitSet
  {
    public:
      BitSet(const unsigned int size);  // Create a set of the given size
      BitSet(const unsigned int size, const bool fill);

      inline void Add(const unsigned int elem);  // Add an element to the set
      inline bool Contains(const unsigned int elem) const;  // Check for an element
      inline void Remove(const unsigned int elem);  // Remove an element

      void Fill(const bool fill);  // Fill with ones or zeros

      inline bool Any(void) const;  // Are any elements in the set
      inline unsigned int Count() const;  // Count the elements in the set

      // Determine if two bit sets are disjoint (have a null intersection)
      inline bool IsDisjoint(const BitSet& other) const;

      // Call a function with each element that is in the set
      template<typename Lambda> inline void ForEach(Lambda f) const;

      // Call a function with each element that is in this set but is not
      // within another set.
      template<typename Lambda> inline void DifferenceForEach(
          const BitSet& other, Lambda func) const;

    protected:
      unsigned int m_n_bits;  // Number of bits in the set
      std::vector<uint32_t> m_data;  // Data elements
  };

  // Add an element to a bit set
  inline void BitSet::Add(const unsigned int elem)
  {
    const unsigned int word = elem / 32;
    const unsigned int bit = elem % 32;
    m_data[word] |= (1 << bit);
  }

  // Determine if an element is in the set
  inline bool BitSet::Contains(const unsigned int elem) const
  {
    const unsigned int word = elem / 32;
    const unsigned int bit = elem % 32;
    return m_data[word] & (1 << bit);
  }

  // Remove an element from the set
  inline void BitSet::Remove(const unsigned int elem)
  {
    const unsigned int word = elem / 32;
    const unsigned int bit = elem % 32;
    m_data[word] &= ~(1 << bit);
  }

  // See if any elements are contained within the set
  inline bool BitSet::Any(void) const
  {
    for (const auto word : m_data)
    {
      if (word)
      {
        return true;
      }
    }

    return false;
  }

  // Count the number of elements in the set
  inline unsigned int BitSet::Count(void) const
  {
    unsigned int count = 0;  // Number of elements

    for (const auto word : m_data)
    {
      count += __builtin_popcount(word);
    }

    return count;
  }

  // Find if two bit vectors are disjoint (have a null intersection).
  inline bool BitSet::IsDisjoint(const BitSet& other) const
  {
    // Iterate over elements in the two vector
    auto a = m_data.cbegin();  // Iterator over this vector
    const auto a_end = m_data.cend();  // End of the this vector
    auto b = other.m_data.cbegin();  // Iterator over the other vector
    const auto b_end = other.m_data.cend();  // End of the other vector
    for (; (a != a_end) && (b != b_end); a++, b++)
    {
      // If this intersection of these elements is not null then the sets
      // cannot be disjoint.
      if (((*a) & (*b)) != 0)
      {
        return false;
      }
    }

    return true;  // Sets must be disjoint
  }

  template<typename Lambda> inline void ForEachBit(
      uint32_t word,  // Word over which to iterate
      unsigned int index,  // Initial index
      Lambda func  // Function to call
  )
  {
    while (word != 0)  // While there are bits in the word
    {
      // Skip the empty bits at the start of the word
      const unsigned int skip = __builtin_ctz(word);

      // Call the function with this element
      func(index + skip);

      // Progress the index and shift the word
      index += skip + 1;
      word >>= (skip + 1);
    }
  }

  // Call a function for each element contained within the bit set
  template<typename Lambda> inline void BitSet::ForEach(Lambda func) const
  {
    unsigned int index = 0;  // Index of currently inspected element

    for (const auto word : m_data)
    {
      // Call the function for each set bit in this word then update the index.
      ForEachBit(word, index * 32, func);
      index++;
    }
  }

  // Call a function for each element contained within the bit set which is not
  // contained within the other bit set.
  template<typename Lambda> inline void BitSet::DifferenceForEach(
      const BitSet& other, Lambda func) const
  {
    unsigned int index = 0;  // Index of currently inspected element

    // Iterate over elements in both vectors
    auto a = m_data.cbegin();  // Iterator over this vector
    const auto a_end = m_data.cend();  // End of the this vector
    auto b = other.m_data.cbegin();  // Iterator over the other vector
    const auto b_end = other.m_data.cend();  // End of the other vector
    for (; (a != a_end) && (b != b_end); a++, b++, index++)
    {
      // Compute the difference of the sets for this word
      const uint32_t word = (*a) & ~(*b);

      // Call the function for each set bit in this word
      ForEachBit(word, index * 32, func);
    }

    // Any remaining elements in the first vector cannot possibly be in the
    // second vector, so iterate over them.
    for (; a != a_end; a++, index++)
    {
      const uint32_t word = *a;
      ForEachBit(word, index * 32, func);
    }
  }
}
