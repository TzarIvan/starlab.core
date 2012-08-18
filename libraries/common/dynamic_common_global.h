/// @todo rename this file to global.h?
#pragma once
#if defined(DYNAMIC_COMMON)
#  define DYNAMIC_COMMON_EXPORT Q_DECL_EXPORT
#else
#  define DYNAMIC_COMMON_EXPORT Q_DECL_IMPORT
#endif

#ifdef DEBUG_STARAPP
    #define DEB 
#else
    #define DEB if(false)
#endif
