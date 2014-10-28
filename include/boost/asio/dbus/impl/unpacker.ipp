// Copyright (c) Benjamin Kietzman (github.com/bkietz)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_ASIO_DBUS_UNPACKER_IPP
#define BOOST_ASIO_DBUS_UNPACKER_IPP

#include <boost/utility/enable_if.hpp>
#include <boost/asio/dbus/element.hpp>

namespace boost {
namespace asio {
namespace dbus {

message::unpacker::unpacker(message& m)
{
  impl::message_iterator::init(m, iter_);
}

template<typename Element>
typename boost::enable_if<is_fixed_type<Element>, message::unpacker&>::type
operator>>(message::unpacker& u, Element& e)
{
  u.iter_.get_basic(&e);
  u.iter_.next();
  return u;
}

message::unpacker&
operator>>(message::unpacker& u, string& s)
{
  const char *c;
  u.iter_.get_basic(&c);
  s.assign(c);
  u.iter_.next();
  return u;
}

} // namespace dbus
} // namespace asio
} // namespace boost

#endif // BOOST_ASIO_DBUS_UNPACKER_IPP
