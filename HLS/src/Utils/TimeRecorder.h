/**
 * @file TimeRecorder.h
 * @author Mxcln (windayau233@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef TIME_RECORDER_H
#define TIME_RECORDER_H

#include <chrono>
#include <string>

namespace hls::utils {

/// @brief TimeRecorder is a class to record the time between start and stop.
class TimeRecorder {

public:
  TimeRecorder();

  /// @brief Start the timer.
  void start();

  /// @brief Stop the timer.
  void stop();

  /// @brief Get the duration of the timer.
  double duration() const;

  /// @brief Log the duration of the timer.
  void logDuration(const std::string& message) const;

  void logCurrentTime(const std::string& message) const;

private:
  std::chrono::steady_clock::time_point start_time;
  std::chrono::steady_clock::time_point end_time;
  bool running;
}; // class TimeRecorder

} // namespace hls::utils

#endif // TIME_RECORDER_H