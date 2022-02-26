#pragma once

#include <QObject>

class testui : public QObject
{
	Q_OBJECT

public:
	testui(QWidget *parent);
	~testui();
};
