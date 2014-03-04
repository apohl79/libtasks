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
#include <bitset>  // for to_string

namespace tasks {
namespace tools {

/*
 * A thread safe lock free bitset.
 */
class bitset {    
public:
    typedef uint16_t int_type;
    typedef std::atomic<int_type> data_type;
    static constexpr int int_size = sizeof(int_type);
    static constexpr int bits_count = int_size * 8;

    bitset(int_type bits = 16)
        : m_bitset(std::vector<data_type>(bits % bits_count?
                                          bits / bits_count + 1:
                                          bits / bits_count)) {}

    inline void toggle(int_type p) {
        int_type idx = p/bits_count;
        int_type bit = 1 << p%bits_count;
        m_bitset[idx] ^= bit;
    }

    inline void set(int_type p) {
        int_type idx = p/bits_count;
        int_type bit = 1 << p%bits_count;
        m_bitset[idx] |= bit;
    }

    inline void unset(int_type p) {
        int_type idx = p/bits_count;
        int_type bit = 1 << p%bits_count;
        if (m_bitset[idx] & bit) {
            m_bitset[idx] ^= bit;
        }
    }

    inline bool test(int_type p) {
        int_type idx = p/bits_count;
        int_type bit = 1 << p%bits_count;
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

    // Returns true if a was bit found. The index of the first bit is stored to idx.
    inline bool first(int_type& idx) const {
        int_type bs;
        int_type offset;
        if (any(bs, offset)) {
            for (int_type p = 0;; p++) {
                int_type bit = 1 << p;
                if (bs & bit) {
                    idx = p + offset;
                    return true;
                }
            }
        }
        return false;
    }
    
    std::string to_string() {
        std::stringstream out;
        for (auto& bs : m_bitset) {
            out << std::bitset<bits_count>(bs) << " ";
        }
        return out.str();
    }
    
private:
    std::vector<data_type> m_bitset;
};

} // tools
} // tasks

#endif // _BITSET_H_
