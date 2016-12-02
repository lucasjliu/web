//
//  Logger.cpp
//
//  by jiahuan.liu
//  11/24/2016
//

#include "Logger.h"

std::string Logger::levelName[] =
{
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

Logger::Logger(std::string file, int line, Level level)
:_output(defaultOutput)
{
    init(level, file, line);
    _stream << "]";
}

Logger::Logger(std::string file, int line, std::string func, Level level)
:_output(defaultOutput)
{
    init(level, file, line);
    _stream << func << "]";
}

Logger::Logger(FILE* ofile, std::string file, int line, Level level)
{
    init(level, file, line);
    _stream << "]";
    _output = std::bind(fileOutput, ofile, std::placeholders::_1);
}

Logger::Logger(FILE* ofile, std::string file, int line, std::string func, Level level)
{
    init(level, file, line);
    _stream<< func << "]";
    _output = std::bind(fileOutput, ofile, std::placeholders::_1);
}

Logger::Logger(FILE* ofile)
{
    _output = std::bind(fileOutput, ofile, std::placeholders::_1);
}

void Logger::init(Level level, std::string file, int line)
{
    if (level == L_DATA) return;
    _stream << "[" << Common::now2str("%H:%M:%S") << "][" << levelName[level]
    << "][" << file << ":" << line;
}

void Logger::finish()
{
    _stream << END;
}

Logger::~Logger()
{
    finish();
    _output(_stream.str());
}

void Logger::defaultOutput(const std::string& str)
{
    fwrite(str.c_str(), 1, str.length(), stdout);
}

void Logger::fileOutput(FILE* file, const std::string& str)
{
    size_t n = fwrite(str.c_str(), 1, str.length(), file);
    if (file == NULL || n != str.length())
    {
        throw Exception("logger output file error.");
    }
}