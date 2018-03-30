#include "textfilterwidget.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

TextFilterWidget::TextFilterWidget(QWidget *apParent)
	: QWidget(apParent)
{
	initControl();
}

void TextFilterWidget::initControl()
{
	QGridLayout *lpLayout = new QGridLayout(this);
	QVBoxLayout *lpBtnLayout = new QVBoxLayout(this);
	QPushButton *lpBtnClose, *lpBtnFilter;
	QPushButton *lpBtnBrowseInput, *lpBtnBrowseOutput;

	lpBtnFilter = new QPushButton(QLatin1String("Filter"), this);
	lpBtnClose = new QPushButton(QLatin1String("Close"), this);
	lpBtnBrowseInput = new QPushButton(QLatin1String("..."), this);
	lpBtnBrowseOutput = new QPushButton(QLatin1String("..."), this);

	lpLayout->addWidget(new QLabel(QLatin1String("Input file")),
						0, 0);
	mpInputPath = new QLineEdit(this);
	lpLayout->addWidget(mpInputPath, 0, 1);
	lpLayout->addWidget(lpBtnBrowseInput, 0, 2);

	lpLayout->addWidget(new QLabel(QLatin1String("Output file")),
						1, 0);
	mpOutputPath = new QLineEdit(this);
	lpLayout->addWidget(mpOutputPath, 1, 1);
	lpLayout->addWidget(lpBtnBrowseOutput, 1, 2);

	// btn layout
	lpLayout->addLayout(lpBtnLayout, 0, 3, 2, 1, Qt::AlignTop);

	lpBtnLayout->addWidget(lpBtnFilter);
	lpBtnLayout->addWidget(lpBtnClose);

	connect(this, SIGNAL(hide()), this, SLOT(close()));
	connect(lpBtnClose, SIGNAL(clicked()), this, SIGNAL(hide()));

	setLayout(lpLayout);
}

void TextFilterWidget::sltBrowseInputPath()
{

}

void TextFilterWidget::sltBrowseOutputPath()
{

}

void TextFilterWidget::sltBtnRemoveDuplicate()
{

}

void TextFilterWidget::sltHide()
{
	this->hide();
}
