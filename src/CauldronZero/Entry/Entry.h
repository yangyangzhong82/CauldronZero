#pragma once

#include "CauldronZero/Macros.h"
#include "ll/api/mod/NativeMod.h"

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

private:
    Entry();
    ~Entry();

    ll::mod::NativeMod& mSelf;
};

} // namespace CauldronZero
