#pragma once
/*
*  Author(s):
*  - Chris  Kilner <ckilner@aldebaran-robotics.com>
*  - Cedric Gestes <gestes@aldebaran-robotics.com>
*
*  Copyright (C) 2010, 2011 Aldebaran Robotics
*/


#ifndef _QI_PERF_DATAPERFTIMER_HPP_
#define _QI_PERF_DATAPERFTIMER_HPP_

#include <qi/api.hpp>
#include <boost/timer.hpp>
#include <string>

namespace qi {
  namespace perf {


    /// <summary>
    /// This class is a utility to calculate the msg/sec
    /// and MB/sec performance of a function
    ///
    /// Usage:
    ///
    /// @code
    /// DataPerfTimer dpt();
    ///
    /// dpt.start(10, 512);
    /// for (int i = 0; i < 10; ++i) {
    ///   send(512octect);
    /// }
    /// dpt.stop();
    /// @endcode
    /// </summary>
    class QI_API DataPerfTimer
    {
    public:

      /// <summary> Constructor. </summary>
      /// <param name="testDescription"> Information describing the test. </param>
      /// <param name="showHeader"> true to show, false to hide the header. </param>
      DataPerfTimer(const std::string& testDescription = "", bool showHeader = true);

      /// <summary> Print header. </summary>
      /// <param name="testDescription"> Information describing the test. </param>
      void printHeader(const std::string& testDescription = "");

      /// <summary> Starts the timer </summary>
      /// <param name="loopCount"> Number of loops. </param>
      /// <param name="msgSize"> Size of the message. </param>
      void start(const unsigned long loopCount = 1, const unsigned long msgSize = 0);

      /// <summary> Stops the timer and optionaly prints results </summary>
      /// <param name="shouldPrint"> true if should print. </param>
      void stop(bool shouldPrint = true);

      /// <summary> Prints this object. </summary>
      void print();

    protected:

      /// <summary> The timer </summary>
      boost::timer  rt;

      /// <summary> Number of loops </summary>
      unsigned long fLoopCount;

      /// <summary> Size of the message </summary>
      unsigned long fMsgSize;

      /// <summary> The elapsed </summary>
      double        fElapsed;

      /// <summary> The message ps </summary>
      double        fMsgPs;

      /// <summary> The mgb ps </summary>
      double        fMgbPs;
    };
  }
}


#endif  // _QI_PERF_DATAPERFTIMER_HPP_
