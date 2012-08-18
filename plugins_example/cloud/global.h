#pragma once
#include <QtCore/qglobal.h>

#undef EXPORT
#if defined(EXPORTFLAG)
#  define EXPORT Q_DECL_EXPORT
#else
#  define EXPORT Q_DECL_IMPORT
#endif
