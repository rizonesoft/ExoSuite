#pragma once
// ── ExoUI Export Macro ──────────────────────────────────────

#if defined(EXOUI_STATIC)
    #define EXOUI_API
#elif defined(EXOUI_BUILD)
    #define EXOUI_API __declspec(dllexport)
#else
    #define EXOUI_API __declspec(dllimport)
#endif
