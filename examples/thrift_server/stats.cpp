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

#include <iostream>
#include "stats.h"

std::atomic<int> stats::m_req_count;
std::atomic<int> stats::m_clients;

bool stats::handle_event(tasks::worker* worker, int events) {
	std::time_t now = std::time(nullptr);
	std::time_t diff = now - m_last;
	int count;
	count = m_req_count.exchange(0, std::memory_order_relaxed);
	m_last = now;
	int qps = count / diff;
	std::cout << qps << " req/s, num of clients " << m_clients << std::endl;
	return true;
}
