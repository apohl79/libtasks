/*
 * Copyright (c) 2013 Andreas Pohl <apohl79 at gmail.com>
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
 *
 * NOTE:
 *
 * The code has been written by Chris Hopman and was copied from:
 *   
 *   http://ideone.com/GkPcy
 */

#ifndef _ITOSTR_H_
#define _ITOSTR_H_

#include <string>

namespace tasks {
namespace tools {

struct itostr_helper {
	static unsigned out[10000];
 
	itostr_helper() {
		for (int i = 0; i < 10000; i++) {
			unsigned v = i;
			char * o = (char*)(out + i);
			o[3] = v % 10 + '0';
			o[2] = (v % 100) / 10 + '0';
			o[1] = (v % 1000) / 100 + '0';
			o[0] = (v % 10000) / 1000;
			if (o[0]) o[0] |= 0x30;
			else if (o[1] != '0') o[0] |= 0x20;
			else if (o[2] != '0') o[0] |= 0x10;
			else o[0] |= 0x00;
		}
	}
};
 
template <typename T>
std::string itostr(T o) {
	typedef itostr_helper hlp;
 
	unsigned blocks[3], *b = blocks + 2;
	blocks[0] = o < 0 ? ~o + 1 : o;
	blocks[2] = blocks[0] % 10000; blocks[0] /= 10000;
	blocks[2] = hlp::out[blocks[2]];
 
	if (blocks[0]) {
		blocks[1] = blocks[0] % 10000; blocks[0] /= 10000;
		blocks[1] = hlp::out[blocks[1]];
		blocks[2] |= 0x30303030;
		b--;
	}
 
	if (blocks[0]) {
		blocks[0] = hlp::out[blocks[0] % 10000];
		blocks[1] |= 0x30303030;
		b--;
	}
 
	char* f = ((char*)b);
	f += 3 - (*f >> 4);
 
	char* str = (char*)blocks;
	if (o < 0) *--f = '-';
	return std::string(f, (str + 12) - f);
}

} // tools
} // tasks

#endif // _ITOSTR_H_
