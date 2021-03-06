// Copyright (c) Benjamin Kietzman (github.com/bkietz)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_ASIO_DBUS_PACKER_IPP
#define BOOST_ASIO_DBUS_PACKER_IPP

#include <boost/utility/enable_if.hpp>
#include <boost/asio/dbus/element.hpp>

namespace boost {
namespace asio {
namespace dbus {

message::packer::packer(message& m)
{
  impl::message_iterator::init_append(m, iter_);
}

template<typename Element>
typename boost::enable_if<is_fixed_type<Element>, message::packer&>::type
operator<<(message::packer& p, const Element& e)
{
  p.iter_.append_basic(element<Element>::code, &e);
  return p;
}

message::packer& operator<<(message::packer& p, const char *c)
{
  p.iter_.append_basic(element<string>::code, &c);
  return p;
}

message::packer& operator<<(message::packer& p, const string& e)
{
  const char *c = e.c_str();
  return p << c;
}

} // namespace dbus
} // namespace asio
} // namespace boost

#endif // BOOST_ASIO_DBUS_PACKER_IPP
