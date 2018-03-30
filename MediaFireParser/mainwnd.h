#ifndef MAINWND_H
#define MAINWND_H

#include <QWidget>
#include "mediafire.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QGroupBox;
class QPushButton;
QT_END_NAMESPACE

class MainWnd : public QWidget
{
	Q_OBJECT
public:
	explicit MainWnd(QWidget *parent = 0);

public slots:
	void sltClickBrowseOpen();
	void sltClickBrowseSave();
	void sltClickDownload();
	void sltClickSave();
	void sltClickReset();

protected:
	void initControl();

protected:
	// URL Info
	QLineEdit		*mpTxtURL;
	QLineEdit		*mpTxtLoadPath;
	QPushButton		*mpBtnBrowseOpen;

	// Password info
	QLineEdit		*mpTxtPwdDownload;
	QLineEdit		*mpTxtPwdExtract;

	// Save info
	QLineEdit		*mpTxtSavePath;
	QPushButton		*mpBtnBrowseSave;

	// Button
	QPushButton		*mpBtnDownload;
	QPushButton		*mpBtnSaveInfo;
	QPushButton		*mpBtnCancel;
	QPushButton		*mpBtnReset;

	MediaFire		*mpMediaFire;
};

#endif // MAINWND_H
