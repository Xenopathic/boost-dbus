// Copyright (c) Benjamin Kietzman (github.com/bkietz)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_ASIO_DBUS_WATCH_TIMEOUT_HPP
#define BOOST_ASIO_DBUS_WATCH_TIMEOUT_HPP

#include <boost/asio/dbus/chrono.hpp>
#include <dbus/dbus.h>
#include <boost/asio/dbus/error.hpp>
#include <boost/asio/generic/stream_protocol.hpp>
#include <boost/asio/steady_timer.hpp>

namespace boost {
namespace asio {
namespace dbus {
namespace detail {

using boost::asio::io_service;
using boost::asio::steady_timer;
using boost::asio::generic::stream_protocol;
using boost::asio::null_buffers;

static void watch_toggled(DBusWatch *dbus_watch, void *data);
struct watch_handler
{
  DBusWatchFlags flags;
  DBusWatch* dbus_watch;
  watch_handler(DBusWatchFlags f, DBusWatch* w):
    flags(f), dbus_watch(w) {}
  
  void operator()(std::error_code ec, size_t)
  {
    if(ec) return;
    dbus_watch_handle(dbus_watch, flags);

    stream_protocol::socket& socket = 
      *static_cast<stream_protocol::socket *>(dbus_watch_get_data(dbus_watch));

    watch_toggled(dbus_watch, &socket.get_io_service());
  }
};
static void watch_toggled(DBusWatch *dbus_watch, void *data)
{
  stream_protocol::socket& socket = 
    *static_cast<stream_protocol::socket *>(dbus_watch_get_data(dbus_watch));

  if(dbus_watch_get_enabled(dbus_watch)) {

    if(dbus_watch_get_flags(dbus_watch) & DBUS_WATCH_READABLE)
      socket.async_read_some(null_buffers(),
        watch_handler(DBUS_WATCH_READABLE, dbus_watch));
    
    if(dbus_watch_get_flags(dbus_watch) & DBUS_WATCH_WRITABLE)
      socket.async_write_some(null_buffers(), 
        watch_handler(DBUS_WATCH_WRITABLE, dbus_watch));

  } else {
    socket.cancel();
  }
}

static dbus_bool_t add_watch(DBusWatch *dbus_watch, void *data)
{
  if(!dbus_watch_get_enabled(dbus_watch)) return TRUE;

  io_service& io = *static_cast<io_service *>(data);

  int fd = dbus_watch_get_unix_fd(dbus_watch);

  if(fd == -1)
    // socket based watches
    fd = dbus_watch_get_socket(dbus_watch);

  stream_protocol::socket& socket =
    *new stream_protocol::socket(io);

  socket.assign(stream_protocol(0,0), fd);

  dbus_watch_set_data(dbus_watch, &socket, NULL);
  
  watch_toggled(dbus_watch, &io);
  return TRUE;
}

static void remove_watch(DBusWatch *dbus_watch, void *data)
{
  delete static_cast<stream_protocol::socket *>(dbus_watch_get_data(dbus_watch));
}




struct timeout_handler
{
  DBusTimeout* dbus_timeout;
  timeout_handler(DBusTimeout* t):
    dbus_timeout(t) {}
  
  void operator()(std::error_code ec)
  {
    if(ec) return;
    dbus_timeout_handle(dbus_timeout);
  }
};

static void timeout_toggled(DBusTimeout *dbus_timeout, void *data)
{
  steady_timer& timer = 
    *static_cast<steady_timer *>(dbus_timeout_get_data(dbus_timeout));

  if(dbus_timeout_get_enabled(dbus_timeout)) {
    steady_timer::duration interval = std::chrono::milliseconds(dbus_timeout_get_interval(dbus_timeout));
    timer.expires_from_now(interval);
    timer.cancel();
    timer.async_wait(timeout_handler(dbus_timeout));
  } else {
    timer.cancel();
  }
}

static dbus_bool_t add_timeout(DBusTimeout *dbus_timeout, void *data)
{
  if(!dbus_timeout_get_enabled(dbus_timeout)) return TRUE;

  io_service& io = *static_cast<io_service *>(data);

  steady_timer& timer = 
    *new steady_timer(io);

  dbus_timeout_set_data(dbus_timeout, &timer, NULL);
  
  timeout_toggled(dbus_timeout, &io);
  return TRUE;
}

static void remove_timeout(DBusTimeout *dbus_timeout, void *data)
{
  delete static_cast<steady_timer *>(dbus_timeout_get_data(dbus_timeout));
}


struct dispatch_handler
{
  boost::asio::io_service& io;
  DBusConnection *conn;
  dispatch_handler(boost::asio::io_service& i, DBusConnection *c): io(i), conn(c) {}
  void operator()() {
    if(dbus_connection_dispatch(conn) == DBUS_DISPATCH_DATA_REMAINS)
      io.post(dispatch_handler(io, conn));
  }
};

static void dispatch_status(DBusConnection *conn, DBusDispatchStatus new_status, void *data)
{
  io_service& io = *static_cast<io_service *>(data);
  if(new_status == DBUS_DISPATCH_DATA_REMAINS)
    io.post(dispatch_handler(io, conn));
}

static void set_watch_timeout_dispatch_functions(DBusConnection *conn, boost::asio::io_service& io)
{
    dbus_connection_set_watch_functions(conn,
      &add_watch, &remove_watch, &watch_toggled, &io, NULL);

    dbus_connection_set_timeout_functions(conn,
      &add_timeout, &remove_timeout, &timeout_toggled, &io, NULL);

    dbus_connection_set_dispatch_status_function(conn,
      &dispatch_status, &io, NULL);
}

} // namespace detail
} // namespace dbus
} // namespace asio
} // namespace boost

#endif // BOOST_ASIO_DBUS_WATCH_TIMEOUT_HPP
