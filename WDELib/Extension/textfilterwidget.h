#ifndef TEXTFILTERWIDGET_H
#define TEXTFILTERWIDGET_H

#include <QWidget>
#include <QLineEdit>

class TextFilterWidget : public QWidget
{
	Q_OBJECT

public:
	TextFilterWidget(QWidget *apParent = 0);

Q_SIGNALS:
	void hide();

public Q_SLOTS:
	void sltHide();

protected Q_SLOTS:
	void sltBrowseInputPath();
	void sltBrowseOutputPath();
	void sltBtnRemoveDuplicate();

private:
	void initControl();

private:
	QLineEdit		*mpInputPath;
	QLineEdit		*mpOutputPath;
};

#endif // TEXTFILTERWIDGET_H
