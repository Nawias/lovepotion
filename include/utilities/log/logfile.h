/*
** logger.h
** @brief   : Logs shit when enabled
*/

#pragma once

#include <stdarg.h>
#include <stdio.h>

#include <utilities/threads/threads.hpp>

class LogFile
{
  public:
    static LogFile& Instance()
    {
        static LogFile instance;
        return instance;
    }

    void LogOutput(const char* func, size_t line, const char* format, ...);

    ~LogFile();

  private:
    static constexpr const char* LOG_FORMAT = "%s:%zu:\n%s\n\n";

    LogFile();

    FILE* file;
    love::mutex mutex;
};

#define LOG(format, ...) \
    LogFile::Instance().LogOutput(__PRETTY_FUNCTION__, __LINE__, format, ##__VA_ARGS__)
