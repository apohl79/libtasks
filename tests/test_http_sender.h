#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <tasks/dispatcher.h>
#include <tasks/io_task.h>
#include <tasks/net/http_sender.h>

#include <mutex>
#include <condition_variable>

class test_handler : public tasks::net::http_response_handler {
public:
    bool handle_response(std::shared_ptr<tasks::net::http_response> response);
};

class test_http_sender : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(test_http_sender);
    CPPUNIT_TEST(requests);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp();
    void tearDown();

protected:
    void requests();

private:
    std::condition_variable m_cond;
    std::mutex m_mutex;
};
