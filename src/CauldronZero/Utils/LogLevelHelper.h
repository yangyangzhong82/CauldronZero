#pragma once

#include "ll/api/io/LogLevel.h"
#include <map>
#include <string>

namespace CauldronZero
{
    /// @brief 获取一个将字符串映射到 ll::io::LogLevel 的常量映射表。
    /// @return 一个包含日志等级映射的 std::map 的常量引用。
    [[nodiscard]] const std::map<std::string, ll::io::LogLevel>& getLogLevelMap();
} // namespace CauldronZero
