#pragma once

#define VERSION "Konnekt W32 mod"

#ifdef JABBEROO_EXPORTS
  #define JOO_API __declspec(dllexport)
#else
  #define JOO_API __declspec(dllimport)
#endif