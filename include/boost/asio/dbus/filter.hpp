// Copyright (c) Benjamin Kietzman (github.com/bkietz)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_ASIO_DBUS_FILTER_HPP
#define BOOST_ASIO_DBUS_FILTER_HPP

#include <functional>
#include <boost/asio/dbus/error.hpp>
#include <boost/asio/dbus/detail/queue.hpp>
#include <boost/asio/dbus/message.hpp>
#include <boost/asio/dbus/connection.hpp>
#include <boost/asio.hpp>

namespace boost {
namespace asio {
namespace dbus {

/// Represents a filter of incoming messages.
/**
 * Filters examine incoming messages, demuxing them to multiple queues.
 */
class filter
{
  connection& connection_;
  std::function<bool(message&)> predicate_;
  detail::queue<message> queue_;

public:

  bool offer(message& m)
  { 
    bool filtered = predicate_(m);
    if(filtered) queue_.push(m);
    return filtered; 
  }

  template<typename MessagePredicate>
  filter(connection& c,
      BOOST_ASIO_MOVE_ARG(MessagePredicate) p)
    : connection_(c),
      predicate_(BOOST_ASIO_MOVE_CAST(MessagePredicate)(p)),
      queue_(connection_.get_io_service())
  {
    connection_.new_filter(*this);
  }

  ~filter()
  {
    connection_.delete_filter(*this);
  }
 
  template<typename MessageHandler>
  inline BOOST_ASIO_INITFN_RESULT_TYPE(MessageHandler,
      void(std::error_code, message))
  async_dispatch(
      BOOST_ASIO_MOVE_ARG(MessageHandler) handler)
  {
    // begin asynchronous operation
    connection_.get_implementation().start(
      connection_.get_io_service());

    return queue_.async_pop(
      BOOST_ASIO_MOVE_CAST(MessageHandler)(handler));
  }

};
} // namespace dbus
} // namespace asio
} // namespace boost


#include <boost/asio/dbus/impl/filter.ipp>
#endif // BOOST_ASIO_DBUS_FILTER_HPP
