#ifndef WDEINCLUDE_H
#define WDEINCLUDE_H

#include <QWebView>

#include "WDE.h"

#include "Logger/logger.h"
#include "Logger/loggertask.h"
#include "Logger/dblogger.h"
#include "Logger/xmllogger.h"
#include "BaseWebEngine/basewebengine.h"
#include "DataEntry/dataentry.h"
#include "Utilities/utilities.h"
#include "Database/sqlquerybuilder.h"
#include "Extension/extfileloading.h"
#include "Utilities/ContactSeeker.h"
#include "Utilities/xmlutil.h"

#ifdef USE_DATABASE
#include "Database/mysqlhandler.h"
#endif

#include "WDEDef.h"

#endif // WDEINCLUDE_H
