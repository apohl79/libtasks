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

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>

#include "test_http_sender.h"
#include "test_disk_io_task.h"
#include "test_socket.h"
#include "test_uwsgi_thrift.h"
#include "test_uwsgi_thrift_async.h"
#include "test_bitset.h"
#include "test_exec.h"

#include <tasks/dispatcher.h>
#include <tasks/executor.h>

#include <string>

using namespace tasks;

CPPUNIT_TEST_SUITE_REGISTRATION(test_http_sender);
CPPUNIT_TEST_SUITE_REGISTRATION(test_disk_io_task);
CPPUNIT_TEST_SUITE_REGISTRATION(test_socket);
CPPUNIT_TEST_SUITE_REGISTRATION(test_uwsgi_thrift);
CPPUNIT_TEST_SUITE_REGISTRATION(test_uwsgi_thrift_async);
CPPUNIT_TEST_SUITE_REGISTRATION(test_bitset);
CPPUNIT_TEST_SUITE_REGISTRATION(test_exec);

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "multi") {
        dispatcher::init_run_mode(dispatcher::mode::MULTI_LOOP);
    }
    // use 4 worker threads
    dispatcher::init_workers(4);
    // reduce the idle timeout for executor threads for the tests
    executor::set_timeout(5);

    dispatcher::instance()->start();

    // informs test-listener about testresults
    CPPUNIT_NS::TestResult testresult;

    // register listener for collecting the test-results
    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener(&collectedresults);

    // register listener for per-test progress output
    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener(&progress);

    // insert test-suite at test-runner by registry
    CPPUNIT_NS::TestRunner testrunner;
    testrunner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    testrunner.run(testresult);

    // output results in compiler-format
    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write();

    // Output XML for Jenkins CPPunit plugin
    std::ofstream xmlFileOut("libtasks_results.xml");
    CPPUNIT_NS::XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    tasks::dispatcher::destroy();

    // return 0 if tests were successful
    return collectedresults.wasSuccessful() ? 0 : 1;
}
