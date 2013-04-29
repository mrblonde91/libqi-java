/*
**
** Author(s):
**  - Cedric GESTES <gestes@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/
#include "pyobject.hpp"
#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>
#include "pyfuture.hpp"
#include "pysignal.hpp"
#include "pyproperty.hpp"
#include "gil.hpp"

qiLogCategory("qipy.object");

namespace qi { namespace py {


    struct PyQiFunctor {
    public:
      PyQiFunctor(const std::string &funName, qi::ObjectPtr obj)
        : _object(obj)
        , _funName(funName)
      {}

      boost::python::object operator()(boost::python::tuple pyargs, boost::python::dict pykwargs) {
        qi::GenericValue val = qi::GenericValue::from(pyargs);
        bool async = boost::python::extract<bool>(pykwargs.get("_async", false));
        std::string funN = boost::python::extract<std::string>(pykwargs.get("_overload", _funName));
        qiLogDebug() << "calling a method: " << funN << " args:" << qi::encodeJSON(val);

        qi::Future<qi::GenericValuePtr> fut;
        {
          //calling c++, so release the GIL.
          GILScopedUnlock _unlock;
          fut = _object->metaCall(funN, val.asDynamic().asTupleValuePtr());
        }
        if (!async)
          return fut.value().to<boost::python::object>();
        else
          return qi::py::makeFuture(fut);
      }

    public:
      qi::ObjectPtr _object;
      std::string   _funName;
    };


    void populateMethods(boost::python::object pyobj, qi::ObjectPtr obj) {
      qi::MetaObject::MethodMap           mm = obj->metaObject().methodMap();
      qi::MetaObject::MethodMap::iterator it;
      for (it = mm.begin(); it != mm.end(); ++it) {
        qi::MetaMethod &mem = it->second;
        //drop special methods
        if (mem.uid() < qiObjectSpecialMethodMaxUid)
          continue;
        std::vector<std::string> vs = qi::signatureSplit(mem.signature());
        qiLogDebug() << "adding method:" << mem.signature();
        boost::python::object fun = boost::python::raw_function(PyQiFunctor(vs[1].c_str(), obj));
        boost::python::api::setattr(pyobj, vs[1].c_str(), fun);// result
      }
    }

    void populateSignals(boost::python::object pyobj, qi::ObjectPtr obj) {
      qi::MetaObject::SignalMap           mm = obj->metaObject().signalMap();
      qi::MetaObject::SignalMap::iterator it;
      for (it = mm.begin(); it != mm.end(); ++it) {
        qi::MetaSignal &ms = it->second;
        //drop special methods
        if (ms.uid() < qiObjectSpecialMethodMaxUid)
          continue;
        std::vector<std::string> vs = qi::signatureSplit(ms.signature());
        qiLogDebug() << "adding signal:" << ms.signature();
        boost::python::object fun = qi::py::makePySignal(ms.signature());
        boost::python::api::setattr(pyobj, vs[1].c_str(), fun);
      }
    }

    void populateProperties(boost::python::object pyobj, qi::ObjectPtr obj) {
      qi::MetaObject::PropertyMap           mm = obj->metaObject().propertyMap();
      qi::MetaObject::PropertyMap::iterator it;
      for (it = mm.begin(); it != mm.end(); ++it) {
        qi::MetaProperty &mp = it->second;
        //drop special methods
        if (mp.uid() < qiObjectSpecialMethodMaxUid)
          continue;
        std::vector<std::string> vs = qi::signatureSplit(mp.signature());
        qiLogDebug() << "adding property:" << mp.signature();
        boost::python::object fun = qi::py::makePyProperty(mp.signature());
        boost::python::api::setattr(pyobj, vs[1].c_str(), fun);
      }
    }


    class PyQiObject {
    public:
      PyQiObject()
      {}

      PyQiObject(const qi::ObjectPtr &obj)
        : _object(obj)
      {}

      boost::python::object call(boost::python::str pyname, boost::python::tuple pyargs, boost::python::dict pykws) {
        return PyQiFunctor(boost::python::extract<std::string>(pyname), _object)(pyargs, pykws);
      }

      boost::python::object metaObject() {
        return qi::GenericValueRef(_object->metaObject()).to<boost::python::object>();
      }

    public:
      qi::ObjectPtr _object;
    };

    boost::python::object makePyQiObject(qi::ObjectPtr obj, const std::string &name) {
      boost::python::object result = boost::python::object(qi::py::PyQiObject(obj));
      qi::py::populateMethods(result, obj);
      qi::py::populateSignals(result, obj);
      qi::py::populateProperties(result, obj);
      return result;
    }

    void export_pyobject() {
      boost::python::class_<qi::py::PyQiObject>("Object")
          .def("call", &qi::py::PyQiObject::call, (boost::python::arg("name"), boost::python::arg("args") = boost::python::tuple(), boost::python::arg("kargs") = boost::python::dict()))
          .def("metaObject", &qi::py::PyQiObject::metaObject);
    }

  }
}