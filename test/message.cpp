// Copyright (c) Benjamin Kietzman (github.com/bkietz)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <boost/asio/dbus/message.hpp>
#include <boost/asio/dbus/error.hpp>


class MessageTest
  : public testing::Test
{
};


TEST_F(MessageTest, CallMessage)
{
  using namespace boost::asio::dbus;

  const message m = message::new_call(
    endpoint(
      "org.freedesktop.Avahi",
      "/",
      "org.freedesktop.Avahi.Server"),
    "GetHostName");

  ASSERT_EQ("org.freedesktop.Avahi", m.get_destination());
  ASSERT_EQ("/", m.get_path());
  ASSERT_EQ("org.freedesktop.Avahi.Server", m.get_interface());
  ASSERT_EQ("GetHostName", m.get_member());

  message m2 = message::new_call(
    endpoint(
      "org.freedesktop.Avahi",
      "/",
      "org.freedesktop.Avahi.Server"),
    "GetHostName");

  m2 << 1;
  int i;
  m2 >> i;
  ASSERT_EQ(i, 1);

  // m.get_sender();
}


// I actually don't know what to do with these yet.
/*
TEST_F(MessageTest, ErrorMessage)
{
  using namespace boost::asio::dbus;

  message m = message::new_call(
    endpoint(
      "org.freedesktop.Avahi",
      "/",
      "org.freedesktop.Avahi.Server"),
    "GetHostName");

  m.set_reply_serial(42);
  m.set_serial(43);

  message em = message::new_error(
    m,
    "com.skizizo.NoHostname",
    "No hostname for you!");

  const error e(em);

  e.throw_if_set();
}
*/
