/*
                                   )
                                  (.)
                                  .|.
                                  | |
                              _.--| |--._
                           .-';  ;`-'& ; `&.
                          \   &  ;    &   &_/
                           |"""---...---"""|
                           \ | | | | | | | /
                            `---.|.|.|.---'

 * This file is generated by bake.lang.c for your convenience. Headers of
 * dependencies will automatically show up in this file. Include bake_config.h
 * in your main project file. Do not edit! */

#ifndef OUSTER_CLIENT2_BAKE_CONFIG_H
#define OUSTER_CLIENT2_BAKE_CONFIG_H

/* Headers of public dependencies */
/* No dependencies */

/* Convenience macro for exporting symbols */
#ifndef ouster_client2_STATIC
#if defined(ouster_client2_EXPORTS) && (defined(_MSC_VER) || defined(__MINGW32__))
  #define OUSTER_CLIENT2_API __declspec(dllexport)
#elif defined(ouster_client2_EXPORTS)
  #define OUSTER_CLIENT2_API __attribute__((__visibility__("default")))
#elif defined(_MSC_VER)
  #define OUSTER_CLIENT2_API __declspec(dllimport)
#else
  #define OUSTER_CLIENT2_API
#endif
#else
  #define OUSTER_CLIENT2_API
#endif

#endif

