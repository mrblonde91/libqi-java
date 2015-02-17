/*
**
** Author(s):
**  - Pierre ROULLON <proullon@aldebaran-robotics.com>
**
** Copyright (C) 2012, 2013 Aldebaran Robotics
*/

#ifndef _JAVA_JNI_APPLICATION_HPP_
#define _JAVA_JNI_APPLICATION_HPP_

#include <jni.h>

jlong createApplication(JNIEnv* env, jobjectArray jargs, const boost::function<qi::Application*(int& argc, char**& argv)>& fn);

extern "C"
{
  JNIEXPORT jlong Java_com_aldebaran_qimessaging_Application_qiApplicationCreate(JNIEnv *env, jclass QI_UNUSED(jobj), jobjectArray jargs, jstring jdefaultUrl, jboolean listen);
  JNIEXPORT jlong Java_com_aldebaran_qimessaging_Application_qiApplicationGetSession(JNIEnv *,jclass, jlong pApplication);
  JNIEXPORT void  Java_com_aldebaran_qimessaging_Application_qiApplicationDestroy(JNIEnv *env, jclass, jlong pApplication);
  JNIEXPORT void  Java_com_aldebaran_qimessaging_Application_qiApplicationStart(JNIEnv *, jclass, jlong pApplication);
  JNIEXPORT void  Java_com_aldebaran_qimessaging_Application_qiApplicationRun(JNIEnv *env, jclass, jlong pApplication);
  JNIEXPORT void  Java_com_aldebaran_qimessaging_Application_qiApplicationStop(JNIEnv *env, jclass, jlong pApplication);
  JNIEXPORT void  Java_com_aldebaran_qimessaging_Application_setLogCategory(JNIEnv *env, jclass, jstring category, jlong verbosity);
} // !extern "C"

#endif // !_JAVA_JNI_APPLICATION_HPP_
