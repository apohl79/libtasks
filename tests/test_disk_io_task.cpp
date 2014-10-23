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

#include <test_disk_io_task.h>
#include <tasks/dispatcher.h>
#include <tasks/tools/buffer.h>
#include <tasks/disk_io_task.h>
#include <ostream>
#include <fcntl.h>

void test_disk_io_task::write() {
    tasks::tools::buffer buf;
    std::ostream os(&buf);
    os << "test1"
       << "test2" << std::endl << "test3" << 12345 << std::endl;
    buf.write("test4\ntest5\n", 12);

    std::atomic<uint16_t> count(m_total);

    for (int i = 0; i < m_total; i++) {
        std::string fname = "/tmp/disk_io_test" + std::to_string(i);
        int fd = open(fname.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        CPPUNIT_ASSERT(fd > -1);

        tasks::disk_io_task* task = new tasks::disk_io_task(fd, EV_WRITE, &buf);
        task->on_finish([this, task, fd, &buf, &count] {
            close(fd);
            // check the return of write()
            CPPUNIT_ASSERT(task->bytes() == static_cast<std::streamsize>(buf.size()));
            CPPUNIT_ASSERT(task->bytes() == 34);
            count--;
        });

        tasks::disk_io_task::add_task(task);
    }

    while (count > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void test_disk_io_task::read() {
    std::atomic<uint16_t> count(m_total);

    // test future
    std::string fname = "/tmp/disk_io_test0";
    int fd = open(fname.c_str(), O_RDONLY);
    CPPUNIT_ASSERT(fd > -1);
    tasks::tools::buffer* buf = new tasks::tools::buffer(1024);
    tasks::disk_io_task* task = new tasks::disk_io_task(fd, EV_READ, buf);
    auto future_bytes = tasks::disk_io_task::add_task(task);
    CPPUNIT_ASSERT(future_bytes.get() == 34);
    close(fd);
    delete buf;

    for (int i = 0; i < m_total; i++) {
        std::string fname = "/tmp/disk_io_test" + std::to_string(i);
        int fd = open(fname.c_str(), O_RDONLY);
        CPPUNIT_ASSERT(fd > -1);

        tasks::tools::buffer* buf = new tasks::tools::buffer(1024);
        tasks::disk_io_task* task = new tasks::disk_io_task(fd, EV_READ, buf);
        task->on_finish([this, task, fd, buf, &count, fname] {
            close(fd);
            // check the return of read()
            CPPUNIT_ASSERT(task->bytes() == 34);
            delete buf;
            unlink(fname.c_str());
            count--;
        });

        tasks::disk_io_task::add_task(task);
    }

    while (count > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
