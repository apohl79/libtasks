/*
 * Copyright (c) 2013-2014 Andreas Pohl <apohl79 at gmail.com>
 *
 * This file is part of libtasks.
 * 
 * libtasks is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libtasks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with libtasks.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _BITSET_H_
#define _BITSET_H_

#include <atomic>
#include <vector>
#include <sstream> // for to_string
#include <cassert>

namespace tasks {
namespace tools {

/*
 * A thread safe lock free bitset.
 */
class bitset {    
public:
    typedef uint64_t int_type;
    typedef std::atomic<int_type> data_type;
    static constexpr int_type int_size = sizeof(int_type);
    static constexpr int_type bits_count = int_size * 8;

    bitset(int_type bits = bits_count)
        : m_bitset(std::vector<data_type>(bits % bits_count?
                                          bits / bits_count + 1:
                                          bits / bits_count)),
          m_bits(bits) {}

    inline int_type bits() const {
        return m_bits;
    }

    inline size_t buckets() const {
        return m_bitset.size();
    }

    inline void toggle(int_type p) {
        assert(p < m_bits);
        int_type idx = p/bits_count;
        int_type bit = int_type(1) << p%bits_count;
        int_type oldval;
        do {
            oldval = m_bitset[idx];
        } while (m_bitset[idx].exchange(oldval ^ bit) != oldval);
    }

    inline void set(int_type p) {
        assert(p < m_bits);
        int_type idx = p/bits_count;
        int_type bit = int_type(1) << p%bits_count;
        int_type oldval;
        do {
            oldval = m_bitset[idx];
        } while (m_bitset[idx].exchange(oldval | bit) != oldval);
    }

    inline void unset(int_type p) {
        if (test(p)) {
            toggle(p);
        }
    }

    inline bool test(int_type p) const {
        assert(p < m_bits);
        int_type idx = p/bits_count;
        int_type bit = int_type(1) << p%bits_count;
        return m_bitset[idx] & bit;
    }

    // Returns true if at least one bit is set
    inline bool any(int_type& res, int_type& offset) const {
        offset = 0;
        for (auto& bs : m_bitset) {
            res = bs;
            if (res > 0) {
                offset *= bits_count; // calc bit offset
                return true;
            }
            offset++;
        }
        return false;
    }

    // Returns true if a set bit was found. The index of the first bit is stored to idx.
    inline bool first(int_type& idx) const {
        int_type bs;
        int_type offset;
        if (any(bs, offset)) {
            for (int_type p = 0; p + offset < m_bits; p++) {
                int_type bit = int_type(1) << p;
                if (bs & bit) {
                    idx = p + offset;
                    return true;
                }
            }
        }
        return false;
    }

    // Returns true if a set bit was found. The index of the first bit is stored to idx.
    // Uses start as starting point to search. The full bitset will be searched as
    // worst case.
    inline bool next(int_type& idx, int_type start = 0) const {
        assert(start < m_bits);
        int_type chk_idx = start;
        int_type chk_cnt = 0;
        do {
            if (test(chk_idx)) {
                idx = chk_idx;
                return true;
            }
            chk_idx++;
            // overflow check
            if (chk_idx == m_bits) {
                chk_idx = 0;
            }
            chk_cnt++;
        } while (chk_cnt < m_bits);
        return false;
    }
    
    std::string to_string() {
        std::stringstream out;
        for (int_type i = 0; i < m_bits; i++) {
            out << (test(i)? "1": "0");
            if (((i + 1) % bits_count) == 0) {
                out << " ";
            }
        }
        return out.str();
    }
    
private:
    std::vector<data_type> m_bitset;
    int_type m_bits;
};

} // tools
} // tasks

#endif // _BITSET_H_
