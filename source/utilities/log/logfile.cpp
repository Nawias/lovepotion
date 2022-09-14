#include <utilities/log/logfile.h>

LogFile::LogFile()
{
    this->file = freopen("love.log", "w", stderr);
}

LogFile::~LogFile()
{
    fclose(this->file);
}

void LogFile::LogOutput(const char* func, size_t line, const char* format, ...)
{
    if (!this->file)
        return;

    std::unique_lock lock(this->mutex);

    va_list args;
    va_start(args, format);
    char buffer[255];

    vsnprintf(buffer, sizeof(buffer), format, args);
    fprintf(this->file, LOG_FORMAT, func, line, buffer);

    fflush(this->file);
}
