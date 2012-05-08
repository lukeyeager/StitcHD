/*
This file is part of StitcHD.

StitcHD is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

StitcHD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with StitcHD.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H 1

#include "Config.hpp"

#include <QtGui/qmainwindow.h>
#include <QtGui/qwidget.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qtextedit.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qlabel.h>
#include <QtGui/qcheckbox.h>
#include <QtGui/qradiobutton.h>
#include <QtGui/qpushbutton.h>
#include <QtCore/qobject.h>
#include <QtGui/qmessagebox.h>
#include <QtGui/qerrormessage.h>
#include <QtGui/qbuttongroup.h>
#include <QtCore/qstring.h>
#include <QtGui/qgroupbox.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/types_c.h>
using namespace cv;

// This utility used in several places
QPixmap Mat2QPixmap(const Mat3b mat);

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	Config config;

	MainWindow(QWidget *parent = 0);

	~MainWindow();

public slots:
	// Options widgets callbacks
	void camCountChanged(int);
	void camInversionChanged(int);
	void resolutionChanged();

	// Commit button callbacks
	void viewCameras();
	void runStitcHD();

private:
	QLineEdit *widthLine, *heightLine;
	QButtonGroup *camCountGroup, *camInversionGroup;
	QPushButton *showButton, *runButton, *quitButton;

};

#endif // GUI_H
