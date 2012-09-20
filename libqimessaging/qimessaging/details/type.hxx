#pragma once
/*
**  Copyright (C) 2012 Aldebaran Robotics
**  See COPYING for the license
*/

#ifndef _QIMESSAGING_DETAILS_TYPE_HXX_
#define _QIMESSAGING_DETAILS_TYPE_HXX_

#include <qi/types.hpp>
#include <cstring>
#include <map>
#include <vector>
#include <list>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/add_pointer.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/is_member_pointer.hpp>

/* This file contains the default-provided Type specialisations
 *
 */

/** Integral types.
 * Since long is neither int32 nor uint32 on 32 bit platforms,
 * use all known native types instead of size/signedness explicit
 * types.
 */
QI_TYPE_CONVERTIBLE_SERIALIZABLE(char);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(signed char);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(unsigned char);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(short);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(unsigned short);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(int);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(unsigned int);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(long);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(unsigned long);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(long long);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(unsigned long long);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(float);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(double);
QI_TYPE_CONVERTIBLE_SERIALIZABLE(std::string);

#define _CONTAINER(c) namespace qi {             \
  template<typename T> class TypeImpl<c<T> >:  \
  public DefaultTypeImpl<c<T>,               \
  TypeDefaultClone<c<T> >,                   \
  TypeDefaultValue<c<T> >,                   \
  TypeDefaultSerialize<c<T> >                \
  > {}; }

_CONTAINER(std::list);
_CONTAINER(std::vector);

#undef _CONTAINER

#define _MAP(c) namespace qi {                                \
  template<typename K, typename V> class TypeImpl<c<K,V> >:  \
  public DefaultTypeImpl<c<K,V>,               \
  TypeDefaultClone<c<K,V> >,                   \
  TypeDefaultValue<c<K,V> >,                   \
  TypeDefaultSerialize<c<K,V> >                \
  > {}; }

_MAP(std::map);
#undef _MAP

namespace qi {
  // void
  template<> class TypeImpl<void>: public Type
  {
  public:
    const std::type_info& info()
    {
      return typeid(void);
    }
    std::string signature()                  { return Signature::fromType(Signature::Type_Void).toString(); }
    void* clone(void*)                       { return 0;}
    void destroy(void* ptr)                  {}
    bool toValue(const void *ptr, detail::DynamicValue & v) {v.clear(); return true;}
    void* fromValue(const detail::DynamicValue & v) { return 0;}
    void serialize(ODataStream&, const void*){}
    void* deserialize(IDataStream&)          { return 0;}
  };

  //reference

  template<typename T> class TypeImpl<T&>
      : public TypeImpl<T> {};
}

namespace qi  {
  /* C array. We badly need this because the type of literal string "foo"
* is char[4] not char*
*
*/
  template<int I>
  class TypeCArrayClone
  {
  public:
    void* clone(void* src)
    {
      char* res = new char[I];
      memcpy(res, src, I);
      return res;
    }
    void destroy(void* ptr)
    {
      delete[]  (char*)ptr;
    }
  };

  template<int I>
  class TypeCArrayValue
  {
  public:
    bool toValue(const void* ptr, detail::DynamicValue& val)
    {
      val = std::string((const char*)ptr, I-1);
      return true;
    }
    void* fromValue(const detail::DynamicValue& val)
    {
      std::string s = val.toString();
      if (s.length() != I)
      {
        qiLogError("Type") << "C string cast fail between char["
                               << I  <<"] and " << s;
        return 0;
      }
      char* res = new char[I];
      memcpy(res, s.c_str(), I);
      return res;
    }
  };

  template<int I> class TypeCArraySerialize
  {
  public:
    void  serialize(ODataStream& s, const void* ptr)
    {
      s << (const char*)ptr;
    }
    void* deserialize(IDataStream& s)
    {
      std::string str;
      s >> str;
      if (str.length() >= I)
        return 0;
      char* res = new char[I];
      strncpy(res, str.c_str(), str.length());
      return res;
    }
    std::string signature()
    {
      return signatureFromType<std::string>::value();
    }
  };

  template<int I> class TypeImpl<char [I]>
      :public  DefaultTypeImpl<char[I],
      TypeCArrayClone<I>,
      TypeCArrayValue<I>,
      TypeCArraySerialize<I>
      >{};


  namespace detail {
    struct signature_function_arg_apply {
      signature_function_arg_apply(std::ostream* val)
        : val(*val)
      {}

      template<typename T> void operator()(T *x) {
        val << qi::typeOf<T>()->signature();
      }

      std::ostream &val;
    };

    template<typename T> struct RawFunctionSignature
    {
      static std::string makeSigreturn()
      {
        typedef typename boost::function_types::result_type<T>::type     ResultType;
        return typeOf<ResultType>()->signature();
      }
      static std::string makeSignature()
      {
        std::stringstream   signature;
        signature << '(';
        typedef typename boost::function_types::parameter_types<T>::type ArgsType;
        boost::mpl::for_each<
          boost::mpl::transform_view<ArgsType,
            boost::add_pointer<
              boost::remove_const<
                boost::remove_reference<boost::mpl::_1>
              >
            >
          >
        >
        (qi::detail::signature_function_arg_apply(&signature));
        signature << ')';
        return signature.str();
      }
    };
    template<typename T> struct MemberFunctionSignature
    {
      static std::string makeSigreturn()
      {
        typedef typename boost::function_types::result_type<T>::type     ResultType;
        return typeOf<ResultType>()->signature();
      }
      static std::string makeSignature()
      {
        // Reconstruct the boost::bind(instance, _1, _2...) signature
        typedef typename boost::function_types::result_type<T>::type     RetType;
        typedef typename boost::function_types::parameter_types<T>::type MemArgsType;
        typedef typename boost::mpl::pop_front< MemArgsType >::type                ArgsType;
        typedef typename boost::mpl::push_front<ArgsType, RetType>::type EffectiveType;
        typedef typename boost::function_types::function_type<EffectiveType>::type type;
        return RawFunctionSignature<type>::makeSignature();
      }
    };
    template<typename T> struct FunctionSignature
    {
      typedef typename  boost::mpl::if_<
        typename boost::function_types::is_member_pointer<T>,
        MemberFunctionSignature<T>,
        RawFunctionSignature<T>
        >::type Backend;
      static std::string signature()
      {
        static std::string result = Backend::makeSignature();
        return result;
      }
      static std::string sigreturn()
      {
        static std::string result = Backend::makeSigreturn();
        return result;
      }
    };
    template<typename T> struct FunctionSignature<boost::function<T> >
    : public FunctionSignature<T> {};

    template<typename T> inline
    std::string functionArgumentsSignature()
    {
      std::stringstream sigs;
      sigs << "(";
      typedef typename boost::function_types::parameter_types<T>::type ArgsType;
      boost::mpl::for_each<
      boost::mpl::transform_view<ArgsType,
      boost::add_pointer<
        boost::remove_const<
        boost::remove_reference<boost::mpl::_1> > > > > (qi::detail::signature_function_arg_apply(&sigs));
      sigs << ")";
      return sigs.str();
    }

  }

}
#endif  // _QIMESSAGING_DETAILS_TYPE_HXX_