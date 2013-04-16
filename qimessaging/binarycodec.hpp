#pragma once
/*
**  Copyright (C) 2012 Aldebaran Robotics
**  See COPYING for the license
*/

#ifndef QIMESSAGING_BINARYCODEC_HPP
#define QIMESSAGING_BINARYCODEC_HPP

#include <qimessaging/api.hpp>
#include <boost/function.hpp>
#include <qi/buffer.hpp>
#include <qi/bufferreader.hpp>
#include <qitype/genericvalue.hpp>
#include <qitype/metaobject.hpp>
namespace qi {

  /// Informations passed when serializing an object
  struct ObjectSerializationInfo
  {
    MetaObject metaObject;
    unsigned int serviceId;
    unsigned int objectId;
  };

  /// Type of callback invoked by sdeerializer when it encounters an object
  typedef boost::function<ObjectPtr (const ObjectSerializationInfo&)> DeserializeObjectCallback;

  /// Type of callback invoked by serializer when it encounters an object.
  typedef boost::function<ObjectSerializationInfo (ObjectPtr)> SerializeObjectCallback;

  template <typename T>
  void encodeBinary(qi::Buffer *buf, const T &t, SerializeObjectCallback onObject=SerializeObjectCallback());

  template <typename T>
  void decodeBinary(qi::BufferReader *buf, T* value, DeserializeObjectCallback onObject=DeserializeObjectCallback());

    /** Encode content of \p gvp into \p buf.
  * @param onObject callback invoked each time an object is encountered.
  */
  template<> QIMESSAGING_API void encodeBinary(qi::Buffer *buf, const GenericValuePtr &gvp, SerializeObjectCallback onObject);
  template<> inline void encodeBinary(qi::Buffer *buf, const GenericValueRef &gvp, SerializeObjectCallback onObject)
  {
    encodeBinary(buf, (const GenericValuePtr&)gvp, onObject);
  }

  /** Decode content of \p buf into \p gvp.
  * @param buf buffer with serialized data
  * @param gvp initialized GenericValuePtr of correct type. Will be filled in.
  * @param onObject callback invoked each time an object is encountered.
  */
  QIMESSAGING_API void decodeBinary(qi::BufferReader *buf, GenericValuePtr gvp, DeserializeObjectCallback onObject=DeserializeObjectCallback());


  template <typename T>
  void encodeBinary(qi::Buffer *buf, const T &t, SerializeObjectCallback onObject) {
    encodeBinary(buf, (const GenericValuePtr&)GenericValuePtr(&t), onObject);
  }

  template <typename T>
  void decodeBinary(qi::BufferReader *buf, T* value, DeserializeObjectCallback onObject) {
    qi::GenericValuePtr gvp(value);
    decodeBinary(buf, gvp, onObject);
  }

}

#endif // QIMESSAGING_BINARYENCODER_HPP
