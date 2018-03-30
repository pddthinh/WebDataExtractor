#ifndef WDE_H
#define WDE_H

#include <QtCore/QtGlobal>

#ifdef WDE_LIB
	#define WDE_LIB_EXPORT Q_DECL_EXPORT
#else
	#define WDE_LIB_EXPORT Q_DECL_IMPORT
#endif

#endif // WDE_H
