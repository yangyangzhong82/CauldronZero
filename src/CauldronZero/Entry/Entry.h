#pragma once

#include "CauldronZero/Macros.h"
#include "ll/api/mod/NativeMod.h"
#include "CauldronZero/Config.h"

namespace CauldronZero {

class CZ_API Entry {

public:
    static Entry& getInstance();

    Entry(const Entry&)            = delete;
    Entry(Entry&&)                 = delete;
    Entry& operator=(const Entry&) = delete;
    Entry& operator=(Entry&&)      = delete;

    [[nodiscard]] ll::mod::NativeMod& getSelf() const;

    bool load();
    bool enable();
    bool disable();
    Config& getConfig();
private:
    Entry();
    ~Entry();

    ll::mod::NativeMod& mSelf;
};
inline std::optional<Config>  config;
} // namespace CauldronZero

// 声明 loadCauldronZeroConfig 函数
void loadCauldronZeroConfig();
