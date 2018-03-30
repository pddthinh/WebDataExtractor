#ifndef XMLLOGGER_H
#define XMLLOGGER_H

#include "WDEInclude.h"
using namespace nsLogger;

#include <QFile>
#include <QXmlStreamWriter>

typedef struct {
	QFile				*pFile = NULL;
	QXmlStreamWriter	*pWriter = NULL;

	void free() {
		delete pWriter;
		pWriter = NULL;

		delete pFile;
		pFile = NULL;
	}
} XML_STREAM;

class WDE_LIB_EXPORT XMLLogger
{
public:
	XMLLogger(Logger *apWDELog = NULL, const QString& astrXMLPath = QLatin1String(""));
	inline void setFilePath(const QString& path) { mFilePath = path; }
	int logDataList(DATA_LIST *pData);
	void writeComment(QString comment);

protected:
	void initXML();
	XML_STREAM *openXMLWriter();
	void closeXMLWriter(XML_STREAM *pStream);

private:
	void writeXMLData(QXmlStreamWriter *pWriter, DataEntry *pData);

private:
	Logger				*mpLog;			// WDE Logger
	QString				mFilePath;
};

#endif // XMLLOGGER_H
