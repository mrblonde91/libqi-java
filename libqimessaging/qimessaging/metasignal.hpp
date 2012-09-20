#pragma once
/*
**  Copyright (C) 2012 Aldebaran Robotics
**  See COPYING for the license
*/

#ifndef _QIMESSAGING_METASIGNAL_HPP_
#define _QIMESSAGING_METASIGNAL_HPP_

#include <qimessaging/signature.hpp>
#include <qimessaging/datastream.hpp>

namespace qi {


  /// Representation of a Signal in an GenericObject.
  class QIMESSAGING_API MetaSignal {
  public:
    MetaSignal(unsigned int uid, const std::string &sig);
    MetaSignal();
    ~MetaSignal();

    const std::string &signature() const;
    unsigned int       uid() const;

  protected:
    unsigned int _uid;
    std::string  _signature;
  };

  QIMESSAGING_API qi::ODataStream &operator<<(qi::ODataStream &stream, const MetaSignal &meta);
  QIMESSAGING_API qi::IDataStream &operator>>(qi::IDataStream &stream, MetaSignal &meta);
  QIMESSAGING_API qi::SignatureStream &operator&(qi::SignatureStream &stream, const MetaSignal &meta);

}; // namespace qi

#endif  // _QIMESSAGING_METASIGNAL_HPP_