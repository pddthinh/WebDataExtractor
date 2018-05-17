#include "webengine.h"

#include <QWebFrame>
#include <QWebElement>
#include <QWebElementCollection>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QWebSettings>
#include <QDebug>

using namespace nsBaseWebEngine;

WebEngine::WebEngine(QWidget *parent)
	: QWebView(parent) {
	QWebSettings * lpSetting = QWebSettings::globalSettings();

	lpSetting->setAttribute(QWebSettings::JavaEnabled, true);
	lpSetting->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
	lpSetting->setAttribute(QWebSettings::JavascriptEnabled, true);

	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

	mstrJSClickEvent =
			"var evObj = document.createEvent('MouseEvents');"
			"evObj.initEvent('click', true, true );"
			"this.dispatchEvent(evObj);";
	mstrCmbSelectedCode = "this.selectedIndex = %1;";
}

QWebElement WebEngine::document() const {
	QWebFrame	*lpFrame = page()->mainFrame();
	return lpFrame->documentElement();
}

QWebElement WebEngine::getElementById(QString astrId) {
	QWebElementCollection lElementCol;
	QString lstrSelector = "*#" + astrId;
	lElementCol = document().findAll(lstrSelector);

	if(lElementCol.count() != 1)
		return QWebElement();

	return lElementCol.at(0);
}

void WebEngine::doClick(QWebElement *apWebElement) {
	do {
		if(!apWebElement) break;

		if(apWebElement->isNull()) break;

		apWebElement->setFocus();

		// currently ok
		apWebElement->evaluateJavaScript(mstrJSClickEvent);
	}while(false);
}

void WebEngine::doClick(const QString &astrSelector) {
	QWebElement lElement;

	lElement = document().findFirst(astrSelector);
	if(lElement.isNull()) {
		qWarning("WebEngine::doClick -- No element found");
		return;
	}

	//	qDebug() << lElement.attribute("href");
	lElement.removeAttribute("target");

	doClick(&lElement);
}

void WebEngine::doScroll(int posX, int posY) {
	QString js("window.scrollTo(%1, %2);");
	executeJS(js.arg(posX).arg(posY));
}

void WebEngine::executeJS(const QString astrJS) {
	QWebFrame	*lpFrame = page()->mainFrame();
	lpFrame->evaluateJavaScript(astrJS);
}

QWebElementCollection WebEngine::findAll(QString astrQuery) {
	return document().findAll(astrQuery);
}

QWebElement WebEngine::findFirst(QString astrQuery) {
	return document().findFirst(astrQuery);
}

bool WebEngine::changeCmbSelectedElement(QWebElement *apCmbElement, int aiSelectedItem) {
	bool	lblRet = false;
	QWebElementCollection	leCol;

	do {
		if(apCmbElement == NULL) break;

		if(apCmbElement->isNull()) break;

		if(aiSelectedItem < 0) break;

		leCol = apCmbElement->findAll("option");
		if(aiSelectedItem >= leCol.count()) break;

		apCmbElement->evaluateJavaScript(mstrCmbSelectedCode.arg(aiSelectedItem));
		lblRet = true;
	} while(false);

	return lblRet;
}

bool WebEngine::changeCmbSelectedElement(QWebElement *apCmbElement,
										 QString astrSelectedItem,
										 QString selected) {
	bool	lblRet = false;
	QWebElement				leTmp;
	QWebElementCollection	leCol;
	int						liIdx;

	do {
		if(apCmbElement == NULL) break;

		if(apCmbElement->isNull()) break;

		leCol = apCmbElement->findAll("option");
		foreach (leTmp, leCol)
			leTmp.removeAttribute("selected");

		for(liIdx = 0; liIdx < leCol.count(); liIdx++) {
			if(leCol[liIdx].toPlainText() == astrSelectedItem) {
				apCmbElement->setAttribute(QLatin1String("selectedIndex"), QString(liIdx));
				leCol[liIdx].setAttribute(QLatin1String("selected"), selected);
				lblRet = true;
				break;
			}
		}
	} while(false);

	return lblRet;
}

bool WebEngine::changeCmbSelectedElement(QString astrCmbElementSelector, int aiSelectedItem) {
	QWebElement		lelement;

	lelement = document().findFirst(astrCmbElementSelector);

	return changeCmbSelectedElement(&lelement, aiSelectedItem);
}

bool WebEngine::changeCmbSelectedElement(QString astrCmbElementSelector, QString astrSelectedItem) {
	QWebElement		lelement;

	lelement = document().findFirst(astrCmbElementSelector);

	return changeCmbSelectedElement(&lelement, astrSelectedItem);
}

QString WebEngine::getHrefURL(QWebElement *apAElement) {
	QString lstrHrefURL = QLatin1String("");

	do {
		if (apAElement == NULL) {
			qDebug() << QLatin1String("WebEngine::getHrefURL - Parameter is NULL");
			break;
		}

		if (apAElement->isNull()) {
			qDebug() << QLatin1String("WebEngine::getHrefURL - Link element is NULL");
			break;
		}

		QString lstrTmp = apAElement->attribute(QLatin1String("href")).trimmed();
		QUrl lUrl(lstrTmp);

		lstrHrefURL = page()->mainFrame()->baseUrl().resolved(lUrl).toString();
	} while(false);

	return lstrHrefURL;
}

QWebElementCollection WebEngine::findAllElement(QString tag,
												QString property, QString value,
												QWebElement *apParent) {
	QString selector = QString(CSS_FORMAT)
					   .arg(tag)
					   .arg(property)
					   .arg(value);
	if (apParent == NULL)
		return findAll(selector);

	return apParent->findAll(selector);
}

QWebElement WebEngine::findFirstElement(QString tag,
										QString property, QString value,
										QWebElement *apParent) {
	QString selector = QString(CSS_FORMAT)
					   .arg(tag)
					   .arg(property)
					   .arg(value);
	if (apParent == NULL)
		return findFirst(selector);

	return apParent->findFirst(selector);
}

QWebElementCollection WebEngine::findAllDiv(QString property, QString value, QWebElement *apParent) {
	QString selector = QString(CSS_FORMAT)
					   .arg("div")
					   .arg(property)
					   .arg(value);
	if (apParent == NULL)
		return findAll(selector);

	return apParent->findAll(selector);
}

QWebElement WebEngine::findFirstDiv(QString property, QString value, QWebElement *apParent) {
	QString selector = QString(CSS_FORMAT)
					   .arg("div")
					   .arg(property)
					   .arg(value);
	if (apParent == NULL)
		return findFirst(selector);

	return apParent->findFirst(selector);
}
