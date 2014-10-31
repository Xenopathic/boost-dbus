// Copyright (c) 2014 Robin McCorkell <rmccorkell@karoshi.org.uk>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_ASIO_DBUS_CHRONO_HPP
#define BOOST_ASIO_DBUS_CHRONO_HPP

// Boost.Chrono C++11 conversions
#define BOOST_CHRONO_DURATION_HPP
#define BOOST_CHRONO_SYSTEM_CLOCKS_HPP
#define BOOST_CHRONO_TIME_POINT_HPP
#include <chrono>
namespace boost {
namespace chrono {
  using std::chrono::duration;
  using std::chrono::system_clock;
  using std::chrono::steady_clock;
  using std::chrono::high_resolution_clock;
  using std::chrono::time_point;
} // namespace chrono
} // namespace boost

#endif
