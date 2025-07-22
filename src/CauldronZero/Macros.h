#pragma once

#ifdef CauldronZero_EXPORTS
#define CZ_API [[maybe_unused]] __declspec(dllexport)
#else
#define CZ_API [[maybe_unused]] __declspec(dllimport)
#endif

