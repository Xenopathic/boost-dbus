// Copyright (c) Benjamin Kietzman (github.com/bkietz)
// Copyright (c) 2014 Robin McCorkell <rmccorkell@karoshi.org.uk>
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_ASIO_DBUS_ERROR_HPP
#define BOOST_ASIO_DBUS_ERROR_HPP

// Boost.System C++11 conversion
#define BOOST_SYSTEM_ERROR_HPP
#define BOOST_ERROR_CODE_HPP
#include <system_error>
namespace boost {
namespace system {
  using std::system_error;
  using std::error_code;
  using std::error_category;
  using std::error_condition;
  using std::errc;

  using std::system_category;
  using std::generic_category;

  template <typename T> struct is_error_code_enum : std::is_error_code_enum<T> {};

#ifndef BOOST_SYSTEM_NO_DEPRECATED
  typedef errc posix;
  typedef errc posix_error;
  inline const error_category& get_system_category() { return system_category(); }
  inline const error_category& get_generic_category() { return generic_category(); }
  inline const error_category& get_posix_category() { return generic_category(); }
  static const error_category& posix_category = generic_category();
  static const error_category& errno_ecat     = generic_category();
  static const error_category& native_ecat    = system_category();
#endif
} // namespace system
} // namespace boost

#include <boost/asio/error.hpp>

namespace std {
  template <> struct is_error_code_enum<boost::asio::error::basic_errors> : std::true_type {};
  template <> struct is_error_code_enum<boost::asio::error::netdb_errors> : std::true_type {};
  template <> struct is_error_code_enum<boost::asio::error::addrinfo_errors> : std::true_type {};
  template <> struct is_error_code_enum<boost::asio::error::misc_errors> : std::true_type {};
}

#include <dbus/dbus.h>
#include <boost/asio/dbus/element.hpp>
#include <boost/asio/dbus/message.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

namespace boost {
namespace asio {
namespace dbus {

class error
  : public std::error_category
{
  DBusError error_;

public:
  error()
  {
    dbus_error_init(&error_);
  }

  error(DBusError *src)
  {
    dbus_error_init(&error_);
    dbus_move_error(src, &error_);
  }

  error(dbus::message& m)
  {
    dbus_error_init(&error_);
    dbus_set_error_from_message(&error_, m);
  }

  ~error()
  {
    dbus_error_free(&error_);
  }

  const char *name() const noexcept
  {
    return error_.name;
  }

  string message(int value) const
  {
    if (error_.message) {
      return error_.message;
	} else {
	  return "";
	}
  }

  bool is_set() const
  {
    return dbus_error_is_set(&error_);
  }

  operator const DBusError *() const
  {
    return &error_;
  }

  operator DBusError *()
  {
    return &error_;
  }

  std::error_code error_code() const;
  std::system_error system_error() const;
  void throw_if_set() const;
};

std::error_code error::error_code() const
{
  return std::error_code(
      is_set(),
      *this);
}

std::system_error error::system_error() const
{
  return std::system_error(error_code());
}

void error::throw_if_set() const
{
  if(is_set()) throw system_error();
}

} // namespace dbus
} // namespace asio
} // namespace boost

#endif // BOOST_ASIO_DBUS_ERROR_HPP
