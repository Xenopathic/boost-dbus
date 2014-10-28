// Copyright (c) Benjamin Kietzman (github.com/bkietz)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_ASIO_DBUS_FILTER_IPP
#define BOOST_ASIO_DBUS_FILTER_IPP

namespace boost {
namespace asio {
namespace dbus {
namespace impl {

DBusHandlerResult filter_callback(
    DBusConnection *c,
    DBusMessage *m,
    void *userdata)
{
  try
  {
    filter& f = *static_cast<filter *>(userdata);
    message m_(m);
    if(f.offer(m_))
    {
      return DBUS_HANDLER_RESULT_HANDLED;
    }
  } catch(...) {
    // do not throw in C callbacks. Just don't.
  }

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

} // namespace impl

void connection_service::new_filter(implementation_type& impl,
    filter& f)
{
  dbus_connection_add_filter(impl,
      &impl::filter_callback, &f, NULL);
}
  
void connection_service::delete_filter(implementation_type& impl,
    filter& f)
{
  dbus_connection_remove_filter(impl,
      &impl::filter_callback, &f);
}

} // namespace dbus
} // namespace asio
} // namespace boost


#endif // BOOST_ASIO_DBUS_FILTER_IPP
