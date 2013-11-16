/**
 * SignalHandler.h
 *
 * Simple signal handler to clean up properly
 * Author: Dennis J. McWherter, Jr.
 */

#ifndef SIGNALHANDLER_H__
#define SIGNALHANDLER_H__

class Config;

class SignalHandler
{
public:
  /**
   * Constructor
   *
   * @param config    Configuration options - mainly for thread control
   */
  SignalHandler(Config& config);

  /**
   * Destructor
   */
  virtual ~SignalHandler();

  /**
   * Install the signal handler
   */
  virtual void install();

  /**
   * Signal handler
   *  Catches CTRL-C to try to exit gracefully
   *
   * @param signal    Signal caught
   */
  static void signalCallback(int signal);

private:
  static Config* config;
};

#endif /** SIGNALHANDLER_H__ */
