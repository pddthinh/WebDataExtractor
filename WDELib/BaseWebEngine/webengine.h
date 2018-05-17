#ifndef WEBENGINE_H
#define WEBENGINE_H

#include <QWebView>
#include "WDE.h"

class QWebElement;
class QWebElementCollection;

#define CSS_FORMAT				"%1[%2=\"%3\"]"

namespace nsBaseWebEngine
{

class WDE_LIB_EXPORT WebEngine : public QWebView
{
	Q_OBJECT
public:
	explicit WebEngine(QWidget *parent = 0);
	void doClick(const QString &astrSelector);
	void doClick(QWebElement *apWebElement);
	void doScroll(int posX, int posY);

public:
	QWebElement document() const;
	QWebElement getElementById(QString astrId);
	void executeJS(const QString astrJS);
	bool changeCmbSelectedElement(QString astrCmbElementSelector, int aiSelectedItem);
	bool changeCmbSelectedElement(QString astrCmbElementSelector, QString astrSelectedItem);
	bool changeCmbSelectedElement(QWebElement *apCmbElement, int aiSelectedItem);
	bool changeCmbSelectedElement(QWebElement *apCmbElement, QString astrSelectedItem, QString selected = "selected");
	QString getHrefURL(QWebElement *apAElement);
	QWebElementCollection findAll(QString astrQuery);
	QWebElement findFirst(QString astrQuery);

	// Utility functions
	QWebElementCollection findAllElement(QString tag, QString property, QString value, QWebElement *apParent = NULL);
	QWebElement findFirstElement(QString tag, QString property, QString value, QWebElement *apParent = NULL);
	QWebElementCollection findAllDiv(QString property, QString value, QWebElement *apParent = NULL);
	QWebElement findFirstDiv(QString property, QString value, QWebElement *apParent = NULL);

protected:
	QString		mstrJSClickEvent;
	QString		mstrCmbSelectedCode;
};

} // namespace nsBaseWebEngine

#endif // WEBENGINE_H
