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
 */

#include <iostream>
#include <memory>

#include <tasks/dispatcher.h>
#include <tasks/net/http_sender.h>

class test_handler : public tasks::net::http_response_handler {
public:
    bool handle_response(std::shared_ptr<tasks::net::http_response> response) {
        std::cout << "Got status " << response->status() << std::endl;
        if (response->content_length()) {
            std::cout << "Content:" << std::endl << response->content_p() << std::endl;
        }
        return false;
    }
};

int main(int argc, char** argv) {
    std::cout << "Press ctrl+c to quit" << std::endl << std::endl;
    
    // initialize the dispatcher first
    std::shared_ptr<tasks::dispatcher> disp = tasks::dispatcher::instance();
    tasks::net::http_sender<test_handler>* sender = new tasks::net::http_sender<test_handler>();
    std::shared_ptr<tasks::net::http_request> request =
        std::make_shared<tasks::net::http_request>("www.google.com", "/");
    if (sender->send(request)) {
        disp->run(1, sender);
    } else {
        delete sender;
    }
    return 0;
}
