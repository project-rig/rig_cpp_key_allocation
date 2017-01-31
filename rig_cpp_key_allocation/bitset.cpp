#include "bitset.h"

namespace BitSet
{
  // Create a new bitset with enough space the given number of elements
  BitSet::BitSet(const unsigned int size) : BitSet::BitSet(size, false)
  {
  }

  // Create a new bitset with enough space the given number of elements and set
  // all of those elements to the value of fill.
  BitSet::BitSet(const unsigned int size, const bool fill)
  {
    // Create a vector of the appropriate size and store the number of bits.
    auto n_words = (size / 32) + ((size % 32) ? 1 : 0);
    m_data = std::vector<uint32_t>(n_words, (uint32_t) 0x0);
    m_n_bits = size;

    // Fill the elements if required
    Fill(fill);
  }

  void BitSet::Fill(const bool fill)
  {
    if (fill)
    {
      // Fill with 1s (set all)
      const unsigned int n_words = m_n_bits / 32;
      const unsigned int last_word = m_n_bits % 32;

      // Fill all complete words
      for (unsigned int i = 0; i < n_words; i++)
      {
        m_data[i] = UINT32_MAX;
      }

      // Fill the last, partial word
      if (last_word)
      {
        m_data[n_words] = (1 << last_word) - 1;
      }
    }
    else
    {
      // Fill with 0s (clear)
      for (auto& word : m_data)
      {
        word = 0x0;
      }
    }
  }
}
