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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <tasks/dispatcher.h>
#include <tasks/net/http_sender.h>

using namespace boost::property_tree;
using namespace boost::property_tree::json_parser;

class json_handler : public tasks::net::http_response_handler {
public:
    bool handle_response(std::shared_ptr<tasks::net::http_response> response) {
        std::cout << "Got status " << response->status() << std::endl
                  << "Content Length: " << response->content_length() << std::endl
                  << "Response:" << std::endl << std::endl;
        if (response->content_length()) {
            try {
                ptree pt;
                read_json(response->content_istream(), pt);
                print_tree(pt);
            } catch (std::exception& e) {
                std::cerr << "error: " << e.what() << std::endl;
            }
        } else {
            std::cout << "empty response" << std::endl;
        }
        return false;
    }

    void print_tree(ptree& pt, std::string indent = "") {
        for (auto &v : pt) {
            std::cout << indent << v.first;
            if (v.second.empty()) {
                std::cout << " = " << v.second.data() << std::endl;
            } else {
                std::cout << std::endl;
                print_tree(v.second, indent + "  ");
            }
        }
    }
};

int main(int argc, char** argv) {
    // initialize the dispatcher first
    auto disp = tasks::dispatcher::instance();
    auto* sender = new tasks::net::http_sender<json_handler>();
    // after sending the request we terminate the dispatcher and exit
    sender->on_finish([]{
            tasks::dispatcher::instance()->terminate();
        });
    auto request = std::make_shared<tasks::net::http_request>("graph.facebook.com",
                                                              "/search?q=test");
    if (sender->send(request)) {
        disp->run(1, sender);
    } else {
        delete sender;
    }
    return 0;
}
