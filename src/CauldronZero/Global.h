#pragma once

#include "CauldronZero/Config/Config.h"
#include "CauldronZero/Macros.h"

namespace CauldronZero {

class CZ_API Global {
public:
    static Global& getInstance();

    Global(const Global&)            = delete;
    Global(Global&&)                 = delete;
    Global& operator=(const Global&) = delete;
    Global& operator=(Global&&)      = delete;

    bool load();

    Config& getConfig();

private:
    Global();
    ~Global();

    void setupConfig();

    Config mConfig;
};

} // namespace CauldronZero
