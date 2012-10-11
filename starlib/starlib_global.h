#pragma once
#if defined(STARLIB)
    #undef STARLIB_EXPORT
    #define STARLIB_EXPORT Q_DECL_EXPORT
#else
    #undef STARLIB_EXPORT
    #define STARLIB_EXPORT Q_DECL_IMPORT
#endif
