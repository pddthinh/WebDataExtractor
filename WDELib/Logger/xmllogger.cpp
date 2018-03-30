#include "xmllogger.h"

#define XML_NODE_ROOT			"db"
#define XML_NODE_ROOT_CLOSE		"</" XML_NODE_ROOT ">\n"

#define XML_NODE_ITEM			"item"
#define XML_NODE_SUB_ITEM		"val"

XMLLogger::XMLLogger(Logger *apWDELog, const QString& astrXMLPath) {
	mpLog = apWDELog;

	QString path = astrXMLPath;
	if (path.isEmpty())
		path = DEF_STR_DEFAULT_DATA_XLS_NAME;
	setFilePath(path);

	initXML();
}

/**
 * @brief XMLLogger::openXMLWriter
 *		Open an XML writer stream
 * This function is base on suggestion of:
 * http://stackoverflow.com/a/17172874
 *
 * @return
 */
XML_STREAM *XMLLogger::openXMLWriter() {
	XML_STREAM *pStream = NULL;

	do {
		pStream = new XML_STREAM();
		pStream->pFile = new QFile(mFilePath);
		quint64 size = pStream->pFile->size();

		do {
			if (size <= 0) {
				pStream->pFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
				break;
			}

			pStream->pFile->open(QFile::ReadWrite);
			quint64 end = QString(XML_NODE_ROOT_CLOSE).size();

			if (!pStream->pFile->seek(size - end)) {
				DEF_LOG(QLatin1String("Cannot initialize XML file for appending!"));
				pStream->free();

				delete pStream;
				pStream = NULL;
				goto _end;
			}
		} while (false);

		pStream->pWriter = new QXmlStreamWriter(pStream->pFile);
		pStream->pWriter->setAutoFormatting(true);
		pStream->pWriter->setAutoFormattingIndent(2);
	} while (0);

_end:
	return pStream;
}

/**
 * @brief XMLLogger::closeXMLWriter
 *		Close XML writer stream
 *
 * @param pStream
 */
void XMLLogger::closeXMLWriter(XML_STREAM *pStream) {
	do {
		if (!pStream)
			break;

		pStream->pFile->close();
		pStream->free();

		delete pStream;
		pStream = NULL;
	} while (0);
}

/**
 * @brief XMLLogger::initXML
 *		Initialise XML log file
 */
void XMLLogger::initXML() {
	QXmlStreamWriter *xmlWriter;
	XML_STREAM *stream;

	do {
		stream = openXMLWriter();
		if (!stream) {
			qDebug() << QLatin1String("Cannot open XML log file!");
			break;
		}

		if (stream->pFile->size() > 0) {
			closeXMLWriter(stream);
			break; // file existed, do nothing
		}

		xmlWriter = stream->pWriter;
		xmlWriter->writeStartDocument();

		// add the root node
		xmlWriter->writeStartElement(QLatin1String(XML_NODE_ROOT));
		// this comment is required for not being simplized root tag
		xmlWriter->writeComment(QLatin1String(" extracted DB "));
		xmlWriter->writeEndElement();

		xmlWriter->writeEndDocument();

		closeXMLWriter(stream);
	} while (0);
}

/**
 * @brief XMLLogger::logDataList
 *		Log list of data into XML file format
 *
 * @param pData
 * @return Number of saved items
 */
int XMLLogger::logDataList(DATA_LIST *pData) {
	int iSaved = 0;

	do {
		if (!pData)
			break;
		if (pData->isEmpty())
			break;

		XML_STREAM *stream = openXMLWriter();
		if (!stream) {
			qDebug() << QLatin1String("Cannot open XML for logging!");
			break;
		}

		QXmlStreamWriter *xmlWriter = stream->pWriter;
		foreach(DataEntry de, *(pData)) {
			if (de.isEmpty())
				continue;

			writeXMLData(xmlWriter, &de);

			iSaved++;
		}

		if (iSaved > 0)
			QTextStream(stream->pFile) << QLatin1String(XML_NODE_ROOT_CLOSE);

		closeXMLWriter(stream);

		pData->clear();
	} while (0);

	return iSaved;
}

void XMLLogger::writeXMLData(QXmlStreamWriter *pWriter, DataEntry *pData) {
	do {
		if (!pWriter || !pData)
			break;

		pWriter->writeStartElement(QLatin1String(XML_NODE_ITEM));

		QStringList lsValue;
		QString value;
		DATA_MAP *pDataMap = pData->getDataMap();
		foreach(QString key, pDataMap->keys()) {
			lsValue = pDataMap->value(key);

			// update the key for valid xml tag name
			key = key.replace(QLatin1Char(' '), QLatin1Char('_'));

			// single value node
			if (lsValue.size() == 1) {
				value = lsValue[0];
				if (value.isEmpty()) {
					pWriter->writeEmptyElement(key);
					continue;
				}

				pWriter->writeTextElement(key, value);
				continue;
			}

			// multi values node
			pWriter->writeStartElement(key);

			// write sub node value
			foreach(value, lsValue) {
				if (value.isEmpty())
					continue;

				pWriter->writeTextElement(XML_NODE_SUB_ITEM, value);
			}

			pWriter->writeEndElement();
		}

		pWriter->writeEndElement();
	} while (false);
}

void XMLLogger::writeComment(QString comment) {
	do {
		if (comment.isEmpty())
			break;

		XML_STREAM *stream = openXMLWriter();
		if (!stream) {
			qDebug() << QLatin1String("Cannot open XML for logging!");
			break;
		}

		QTextStream(stream->pFile) << QString("\n<!-- %1 -->\n%2")
									  .arg(comment)
									  .arg(XML_NODE_ROOT_CLOSE);

		closeXMLWriter(stream);
	} while (0);
}
