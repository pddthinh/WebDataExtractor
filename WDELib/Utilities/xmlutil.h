//#ifndef XMLUTIL_H
//#define XMLUTIL_H

//#include <QFile>
//#include <QDebug>
//#include <QXmlStreamReader>
//#include <QXmlStreamWriter>


//#include <QtXml/QXmlDefaultHandler>

//#include "DataEntry/dataentry.h"

//using namespace nsWDEDef;

////#define XML_ITEM_PREFIX			QLatin1String("_")
//#define XML_ITEM_SPACE			QLatin1String("__")

//#define XML_NODE_DATA			QLatin1String("data")
//#define XML_NODE_ITEM			QLatin1String("item")

//class XMLUtil
//{
//public:
//	XMLUtil(QString path);
//	inline void setFilePath(QString path) { mFilePath = path; }

//	bool init();
//	void close();
//	bool saveData(DATA_LIST *pList);
//	bool loadData(DATA_LIST *pList, QString path = QString(""));

//private:
//	QFile				*mpFile;
//	QString				mFilePath;
//	QXmlStreamWriter	*mpXMLWriter;
//};

//class DataEntryHandler: public QXmlDefaultHandler
//{
//public:
//	DataEntryHandler(DATA_LIST *pList);

//	// overide functions
//	bool characters(const QString &ch);
//	bool endElement(const QString&,
//					const QString&,
//					const QString &name);
//	bool startElement(const QString&,
//					  const QString&,
//					  const QString &name,
//					  const QXmlAttributes &);
//	bool error(const QXmlParseException &exception);
//	bool fatalError(const QXmlParseException &exception);

//	// util functions
//	inline void setDataList(DATA_LIST *pList) { mpList = pList; }

//private:
//	DATA_LIST		*mpList;
//	DataEntry		mData;
//	QString			mstrKey;
//	QString			mstrValue;
//	bool			mblInItem;
//}; // DataEntryHandler

//#endif // XMLUTIL_H
