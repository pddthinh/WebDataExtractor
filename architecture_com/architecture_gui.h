#ifndef ARCHITECTURE_GUI_H
#define ARCHITECTURE_GUI_H

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include "architecture_base.h"
#include "chartered_practice.h"
#include "chartered_member.h"

class architecture_gui : public QWidget
{
	Q_OBJECT
public:
	explicit architecture_gui(QWidget *parent = 0);
	
	typedef enum {
		ENU_SITE_chartered_practice = 0,
		ENU_SITE_member_practice,
		ENU_SITE_MAX
	} ENU_SITE;

public slots:
	void sltItemDoubleClick(QListWidgetItem *apItem);
	void sltRun();

protected:
	void init();
	void initList();
	
private:
	QListWidget		*mpList;
};

#endif // ARCHITECTURE_GUI_H
