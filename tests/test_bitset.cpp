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

#include <tasks/tools/bitset.h>

#include "test_bitset.h"

using namespace tasks::tools;

void full_iteration(bitset& bs) {
    for (bitset::int_type i = 0; i < bs.bits(); i++) {
        bs.toggle(i);
    }
    for (bitset::int_type i = 0; i < bs.bits(); i++) {
        CPPUNIT_ASSERT_MESSAGE("bitset(" + std::to_string(bs.bits()) + ") toggle test 1: i=" + std::to_string(i),
                               bs.test(i));
    }

    for (bitset::int_type i = 0; i < bs.bits(); i++) {
        bs.toggle(i);
    }
    for (bitset::int_type i = 0; i < bs.bits(); i++) {
        CPPUNIT_ASSERT_MESSAGE("bitset(" + std::to_string(bs.bits()) + ") toggle test 2: i=" + std::to_string(i),
                               !bs.test(i));
    }

    for (bitset::int_type i = 0; i < bs.bits(); i++) {
        bs.set(i);
    }
    for (bitset::int_type i = 0; i < bs.bits(); i++) {
        CPPUNIT_ASSERT_MESSAGE("bitset(" + std::to_string(bs.bits()) + ") set test: i=" + std::to_string(i),
                               bs.test(i));
    }

    for (bitset::int_type i = 0; i < bs.bits(); i++) {
        bs.unset(i);
    }
    for (bitset::int_type i = 0; i < bs.bits(); i++) {
        CPPUNIT_ASSERT_MESSAGE("bitset(" + std::to_string(bs.bits()) + ") unset test: i=" + std::to_string(i),
                               !bs.test(i));
    }
}

void test_bitset::test() {
    bitset::int_type bit;

    bitset bs1(10);
    CPPUNIT_ASSERT(1 == bs1.buckets());
    full_iteration(bs1);
    bs1.set(0);
    bs1.set(3);
    bs1.set(6);
    bs1.set(9);
    CPPUNIT_ASSERT(bs1.first(bit));
    CPPUNIT_ASSERT(0 == bit);
    bs1.unset(0);
    CPPUNIT_ASSERT(bs1.first(bit));
    CPPUNIT_ASSERT(3 == bit);
    CPPUNIT_ASSERT(bs1.next(bit));
    CPPUNIT_ASSERT(3 == bit);
    CPPUNIT_ASSERT(bs1.next(bit, 3));
    CPPUNIT_ASSERT(3 == bit);
    CPPUNIT_ASSERT(bs1.next(bit, 4));
    CPPUNIT_ASSERT(6 == bit);
    CPPUNIT_ASSERT(bs1.next(bit, 7));
    CPPUNIT_ASSERT(9 == bit);
    bs1.unset(9);
    CPPUNIT_ASSERT(bs1.next(bit, 9));
    CPPUNIT_ASSERT(3 == bit);

    bitset bs2(64);
    CPPUNIT_ASSERT(1 == bs2.buckets());
    full_iteration(bs2);
    bs2.set(63);
    CPPUNIT_ASSERT(bs2.next(bit));
    CPPUNIT_ASSERT(63 == bit);
    CPPUNIT_ASSERT(bs2.next(bit, 62));
    CPPUNIT_ASSERT(63 == bit);
    CPPUNIT_ASSERT(bs2.next(bit, 63));
    CPPUNIT_ASSERT(63 == bit);

    bitset bs3(120);
    CPPUNIT_ASSERT(2 == bs3.buckets());
    full_iteration(bs3);
    bs3.set(3);
    bs3.set(63);
    bs3.set(64);
    bs3.set(118);
    CPPUNIT_ASSERT(bs3.next(bit));
    CPPUNIT_ASSERT(3 == bit);
    CPPUNIT_ASSERT(bs3.next(bit, 4));
    CPPUNIT_ASSERT(63 == bit);
    CPPUNIT_ASSERT(bs3.next(bit, 64));
    CPPUNIT_ASSERT(64 == bit);
    CPPUNIT_ASSERT(bs3.next(bit, 65));
    CPPUNIT_ASSERT(118 == bit);
    CPPUNIT_ASSERT(bs3.next(bit, 119));
    bs3.unset(3);
    bs3.unset(63);
    bs3.unset(64);
    CPPUNIT_ASSERT(bs3.next(bit));
    CPPUNIT_ASSERT(118 == bit);
    CPPUNIT_ASSERT(bs3.first(bit));
    CPPUNIT_ASSERT(118 == bit);

    bitset bs4(129);
    CPPUNIT_ASSERT(3 == bs4.buckets());
    full_iteration(bs4);
    bs4.set(128);
    CPPUNIT_ASSERT(bs4.next(bit));
    CPPUNIT_ASSERT(128 == bit);
    CPPUNIT_ASSERT(bs4.first(bit));
    CPPUNIT_ASSERT(128 == bit);
    bs4.unset(128);
    bs4.set(60);
    CPPUNIT_ASSERT(bs4.next(bit, 61));
    CPPUNIT_ASSERT(60 == bit);
}
