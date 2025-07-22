#include "LogLevelHelper.h"

namespace CauldronZero
{
    const std::map<std::string, ll::io::LogLevel>& getLogLevelMap()
    {
        static const std::map<std::string, ll::io::LogLevel> stringToLogLevel = {
            {"Off",   ll::io::LogLevel::Off},
            {"Fatal", ll::io::LogLevel::Fatal},
            {"Error", ll::io::LogLevel::Error},
            {"Warn",  ll::io::LogLevel::Warn},
            {"Info",  ll::io::LogLevel::Info},
            {"Debug", ll::io::LogLevel::Debug},
            {"Trace", ll::io::LogLevel::Trace},
        };
        return stringToLogLevel;
    }
} // namespace CauldronZero
