//#include "xmlutil.h"

//XMLUtil::XMLUtil(QString path)
//{
//	mFilePath = path;
//}

///**
// * Initialize the XMLUtil class
// *
// * @brief XMLUtil::init
// * @return
// */
//bool XMLUtil::init()
//{
//	bool lblRet = false;

//	do
//	{
//		mpFile = new QFile(mFilePath);
//		if (mpFile->open(QFile::Truncate
//						 | QFile::WriteOnly
//						 | QFile::Text) == false)
//		{
//			qDebug() << QLatin1String("Can not open file: ") << mFilePath;
//			delete mpFile;
//			mpFile = NULL;
//			break;
//		}

//		mpXMLWriter = new QXmlStreamWriter(mpFile);
//		mpXMLWriter->setAutoFormatting(true);

//		mpXMLWriter->writeStartDocument();
//		mpXMLWriter->writeStartElement(XML_NODE_DATA);
//		lblRet = true;
//	} while (false);

//	return lblRet;
//}

///**
// * Close the xml file
// *
// * @brief XMLUtil::close
// */
//void XMLUtil::close()
//{
//	do
//	{
//		if (mpFile == NULL || mpXMLWriter == NULL)
//			break;

//		mpXMLWriter->writeEndElement(); // [data]

//		mpXMLWriter->writeEndDocument();
//		delete mpXMLWriter;
//		mpXMLWriter = NULL;

//		mpFile->close();
//		delete mpFile;
//		mpFile = NULL;
//	} while (false);
//}

///**
// * Save the data list to xml file
// *
// * @brief XMLUtil::saveData
// * @param pList
// * @return
// */
//bool XMLUtil::saveData(DATA_LIST *pList)
//{
//	bool lblRet = false;

//	do
//	{
//		if (mpFile == NULL)
//		{
//			qDebug() << QLatin1String("Output file is not created!");
//			break;
//		}

//		if (pList->isEmpty() == true)
//		{
//			qDebug() << QLatin1String("Data list is empty!");
//			break;
//		}

//		DataEntry data;
//		DATA_MAP *lpDataMap = NULL;
//		int i;
//		int size = pList->size();
//		for (i = 0; i < size; i++)
//		{
//			data = pList->at(i);
//			lpDataMap = data.getDataMap();
//			if (lpDataMap == NULL || lpDataMap->isEmpty() == true)
//			{
//				qWarning() << QLatin1String("Data is empty!");
//				continue;
//			}

//			mpXMLWriter->writeStartElement(XML_NODE_ITEM);
//			QString value;
//			foreach (QString key, lpDataMap->keys())
//			{
//				value = lpDataMap->value(key);
//				key = key.replace(" ", XML_ITEM_SPACE);

//				if (value == NULL || value.isEmpty())
//				{
//					mpXMLWriter->writeEmptyElement(key);
//					continue;
//				}

//				mpXMLWriter->writeStartElement(key);
//				mpXMLWriter->writeCDATA(value);
//				mpXMLWriter->writeEndElement(); // key
//			} // keys

//			mpXMLWriter->writeEndElement(); // element [item]
//		} // node

//		lblRet = true;
//	} while (false);

//	return lblRet;
//}

///**
// * Load data from XML file to a list of DataEntry
// *
// * @brief XMLUtil::loadData
// * @param pList
// * @param path
// * @return
// */
//bool XMLUtil::loadData(DATA_LIST *pList, QString path)
//{
//	bool lblRet = false;

//	do
//	{
//		if (pList == NULL)
//		{
//			qDebug() << QLatin1String("Result list is null!");
//			break;
//		}

//		pList->clear();

//		QString filePath = path;
//		if (filePath .length() == 0)
//			filePath = mFilePath;
//		QFile file(filePath);
//		QXmlSimpleReader xmlReader;
//		QXmlInputSource *lpSource = new QXmlInputSource(&file);
//		DataEntryHandler *lpHandler = new DataEntryHandler(pList);

//		xmlReader.setContentHandler(lpHandler);
//		xmlReader.setErrorHandler(lpHandler);

//		lblRet = xmlReader.parse(lpSource);

//		file.close();
//	} while(false);

//	return lblRet;
//}

///**********************************************************/
//// DataEntryHandler

//DataEntryHandler::DataEntryHandler(DATA_LIST *pList)
//{
//	mblInItem = false;
//	mpList = pList;
//}

///**
// * Callback when loading text string value
// *
// * @brief DataEntryHandler::characters
// * @param ch
// * @return
// */
//bool DataEntryHandler::characters(const QString &ch)
//{
//	// TODO: have to detect this is CDATA or normal text
//	// this way is work arround only

//	QString tmp = ch;
//	tmp = tmp.replace("\n", "").trimmed();

//	if (tmp.length() > 0)
//		mstrValue = ch;

//	return true;
//}

///**
// * Callback when loading a start element
// *
// * @brief DataEntryHandler::startElement
// * @param name
// * @return
// */
//bool DataEntryHandler::startElement(const QString &,
//									const QString &,
//									const QString &name,
//									const QXmlAttributes &)
//{
//	do
//	{
//		if (name == XML_NODE_DATA)
//			continue;

//		if (name == XML_NODE_ITEM)
//		{
//			mblInItem = true;
//			mstrKey.clear();
//			mstrValue.clear();
//			mData.clearData();
//			mData.clearTable();
//			continue;
//		}

//		if (mblInItem == true)
//		{
//			mstrKey = name;
//			mstrKey = mstrKey.replace(XML_ITEM_SPACE, " ");
//		}
//	} while (false);

//	return true;
//}

///**
// * Callback when loading an end elment
// *
// * @brief DataEntryHandler::endElement
// * @param name
// * @return
// */
//bool DataEntryHandler::endElement(const QString &,
//								  const QString &,
//								  const QString &name)
//{
//	do
//	{
//		if (name == XML_NODE_DATA)
//			continue;

//		if (name == XML_NODE_ITEM)
//		{
//			// save to list
//			if (mData.getDataMap()->isEmpty() == false)
//				mpList->append(mData);
//			mData.clearData();
//			mData.clearTable();
//			mblInItem = false;
//			continue;
//		}

//		// add property
//		mData.addProperty(mstrKey, mstrValue);
//		mstrKey.clear();
//		mstrValue.clear();
//	} while (false);

//	return true;
//}

//bool DataEntryHandler::error(const QXmlParseException &exception)
//{
//	qDebug() << QLatin1String("Parsing got recoverable error: ")
//			 << exception.message();

//	return true;
//}

//bool DataEntryHandler::fatalError(const QXmlParseException &exception)
//{
//	qDebug() << QLatin1String("Parse fail: ")
//			 << exception.message();
//	qDebug() << QLatin1String("Handler error message: ")
//			 << errorString();

//	return true;
//}
