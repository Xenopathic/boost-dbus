// Copyright (c) Benjamin Kietzman (github.com/bkietz)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/asio/dbus/connection.hpp>
#include <boost/asio/dbus/message.hpp>
#include <boost/asio/dbus/endpoint.hpp>
#include <boost/asio/dbus/filter.hpp>
#include <boost/asio/dbus/match.hpp>

#include <gtest/gtest.h>
#include <unistd.h>
#include <functional>


using namespace boost::asio;
using namespace boost::asio::dbus;
using std::error_code;


class AvahiTest
  : public testing::Test
{
protected:
  static void SetUpTestCase()
  {
  }
  static io_service io;
  static string browser_path;
  static endpoint avahi_daemon;
};
// It seems like these should be non-static,
// but I get a mysterious SEGFAULT for io
//   ¿related: http://stackoverflow.com/questions/18009156/boost-asio-segfault-no-idea-why
io_service AvahiTest::io;
string AvahiTest::browser_path;
endpoint AvahiTest::avahi_daemon(
  "org.freedesktop.Avahi",
  "/",
  "org.freedesktop.Avahi.Server");


TEST_F(AvahiTest, GetHostName)
{
  connection system_bus(io, bus::system);
  string avahi_hostname;
  string unix_hostname;

  {
    // get hostname from a system call
    char c[1024];
    gethostname(c, 1024);
    unix_hostname = c;
  }

  // get hostname from the Avahi daemon
  message m = message::new_call(
    avahi_daemon,
    "GetHostName");

  system_bus.async_send(m, [&](error_code ec, message r){  
    r.unpack(avahi_hostname);

    // this is only usually accurate
    EXPECT_EQ(unix_hostname, avahi_hostname);

    // eventually, connection should stop itself
    io.stop();
  });

  io.run();
}


TEST_F(AvahiTest, ServiceBrowser)
{
  connection system_bus(io, bus::system);

  // create new service browser
  message m = message::new_call(
    avahi_daemon,
    "ServiceBrowserNew");

  m.pack<int32>(-1)
   .pack<int32>(-1)
   .pack<string>("_http._tcp")
   .pack<string>("local")
   .pack<uint32>(0);

  message r = system_bus.send(m);

  r.unpack(browser_path);

  // RegEx match browser_path
  // catch a possible exception
}


TEST_F(AvahiTest, BrowseForHttp)
{
  connection system_bus(io, bus::system);

  match m(system_bus, "type='signal',path='" + browser_path + "'");
  filter f(system_bus, [](message& m){
    return m.get_member() == "ItemNew"; });

  std::function<void(error_code, message)> h;
  h = [&] (error_code ec, message m) {};
  f.async_dispatch(h);
  io.run();
}
