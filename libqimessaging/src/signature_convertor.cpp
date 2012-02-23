/*
*  Author(s):
*  - Cedric Gestes <gestes@aldebaran-robotics.com>
*  - Chris  Kilner <ckilner@aldebaran-robotics.com>
*
*  Copyright (C) 2010 Aldebaran Robotics
*/

#include <iostream>
#include <cstring>
#include <qi/log.hpp>
#include "signature_convertor.hpp"
#include <qimessaging/signature.hpp>

namespace qi {

  enum PrivateQiSignatureType {
    QI_STRING     = 0,
    QI_LIST       = 1,
    QI_LIST_END   = 2,
    QI_MAP        = 3,
    QI_MAP_END    = 4,
  };

  static const char *gStl[] = {
    "string",
    "std::vector<",
    ">",
    "std::map<",
    ">"
  };

  static const char *gStlConst[] = {
    "const string &",
    "const std::vector<",
    ">",
    "const std::map<",
    ">"
  };

  static const char *gQt[] = {
    "QString",
    "QList<",
    ">",
    "QMap<",
    ">"
  };

  static const char *gQtConst[] = {
    "const QString &",
    "const QList<",
    ">",
    "const QMap<",
    ">"
  };


  SignatureConvertor::SignatureConvertor(qi::Signature *sig, SignatureType type, bool constify)
    : _type(type),
      _done(false),
      _constify(constify),
      _sig(sig)
  {}

  const char *SignatureConvertor::elementTypeSTL(int idx, bool constify)
  {
    if (constify) {
      if (_type == Qt)
        return gQtConst[idx];
      if (_type == STL)
        return gStlConst[idx];
    } else {
      if (_type == Qt)
        return gQt[idx];
      if (_type == STL)
        return gStl[idx];
    }
    return 0;
  }

  const std::string &SignatureConvertor::signature() {
    if (!_done) {
      visit(_sig);
      _done = true;
    }
    return _result;
  }

  void SignatureConvertor::visit(qi::Signature *sig) {

    qi::Signature::iterator it;

    for (it = sig->begin(); it != sig->end(); ++it) {
      visitSingle(&it, _constify);
    }
  }

  void SignatureConvertor::visitSingle(qi::Signature::iterator *it, bool constify) {
    switch(it->type()) {
      case '[':
        visitList(it, constify);
        break;
      case '{':
        visitMap(it, constify);
        break;
      case '(':
        visitTuple(it, constify);
        break;
      default:
        visitSimple(it, constify);
        break;
    }
  }

  void SignatureConvertor::visitSimple(qi::Signature::iterator *it, bool constify) {
    switch(it->type()) {
      case 'b':
        _result += "bool";
        break;
      case 'i':
        _result += "int";
        break;
      case 'f':
        _result += "float";
        break;
      case 'd':
        _result += "double";
        break;
      case 'v':
        _result += "void";
        break;
      case 's':
          _result += elementTypeSTL(QI_STRING, constify);
        break;
      default:
        _result += "Unknown";
        break;
    }
    //add necessary pointer
    for (int i = 0; i < it->pointer(); ++i) {
      _result += "*";
    }
  }

  void SignatureConvertor::visitList(qi::Signature::iterator *it, bool constify) {
    _result += elementTypeSTL(QI_LIST, constify);
    qi::Signature sig = it->children();
    qi::Signature::iterator it2 = sig.begin();
    visitSingle(&it2, false);
    _result += elementTypeSTL(QI_LIST_END, constify);
    //add necessary pointer
    for (int i = 0; i < it->pointer(); ++i) {
      _result += "*";
    }
    if (constify)
      _result += "&";
  }

  void SignatureConvertor::visitMap(qi::Signature::iterator *it, bool constify) {
    _result += elementTypeSTL(QI_MAP, constify);
    qi::Signature sig = it->children();
    qi::Signature::iterator it2 = sig.begin();
    visitSingle(&it2, false);
    _result += ",";
    ++it2;
    visitSingle(&it2, false);
    _result += elementTypeSTL(QI_MAP_END, constify);
    //add necessary pointer
    for (int i = 0; i < it->pointer(); ++i) {
      _result += "*";
    }
    if (constify)
      _result += "&";
  }

  //not really tuple but fonction arguments
  void SignatureConvertor::visitTuple(qi::Signature::iterator *it, bool constify) {
    _result += "(";
    qi::Signature sig = it->children();

    qi::Signature::iterator it2 = sig.begin();
    while (it2 != sig.end()) {
      visitSingle(&it2, constify);
      ++it2;
      if (it2 != sig.end())
        _result += ",";
      else
        break;
    }
    _result += ")";
  }


};