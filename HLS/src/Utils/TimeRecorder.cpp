/**
 * @file TimeRecorder.cpp
 * @author Mxcln (windayau233@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <Utils/Log.h>
#include <Utils/TimeRecorder.h>

using namespace hls::utils;

TimeRecorder::TimeRecorder() : running(false) {}

void TimeRecorder::start() {
  if (!running) {
    start_time = std::chrono::steady_clock::now();
    running = true;
  }
}

void TimeRecorder::stop() {
  if (running) {
    end_time = std::chrono::steady_clock::now();
    running = false;
  }
}

double TimeRecorder::duration() const {
  if (running) {
    return 0.0;
  } else {
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    return elapsed_time.count();
  }
}

void TimeRecorder::logDuration(const std::string& message) const {
  if (running) {
    LOG_ERROR("TimeRecorder is running, cannot log duration.");
  } else {
    auto elapsed_time = duration();
    LOG_INFO(message + std::to_string(elapsed_time) + "ms.");
  }
}

void TimeRecorder::logCurrentTime(const std::string& message) const {
  auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  LOG_INFO(message + std::ctime(&current_time));
}