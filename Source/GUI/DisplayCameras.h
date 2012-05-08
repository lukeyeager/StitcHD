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

#ifndef DISPLAYCAMERAS_H
#define DISPLAYCAMERAS_H 1

#include "VideoStitcher.hpp"
#include "MainWindow.h"

#include <string>
#include <vector>
#include <iostream>
using namespace std;

#include <DShow.h>
#include <Ks.h>               // Required by KsMedia.h   
#include <KsMedia.h>      // For KSPROPERTY_CAMERACONTROL_FLAGS_*
#include <Utility.h>

#include <Qt/qobject.h>
#include <Qt/qthread.h>
#include <Qt/qvector.h>
#include <QtGui/qmainwindow.h>
#include <QtGui/qlabel.h>
#include <Qt/qcoreapplication.h>
#include <QtGui/qdockwidget.h>

class DisplayCameraFrame : public QMainWindow
{
	Q_OBJECT

public:

	CameraCapture camCapture;
	int id;
	Config config;
	IAMCameraControl* cameraController;

	DisplayCameraFrame(QWidget *parent, int id, Config config, IAMCameraControl* camCon);

	~DisplayCameraFrame();

	void update();

public slots:
	//Directional button callbacks
	void panLeft();
	void panRight();
	void tiltUp();
	void tiltDown();
	void resetPos();

private:
	QLabel *frameWidget;
	QPushButton *leftButton, *rightButton, *upButton, *downButton, *resetButton;
};

class DisplayCameraController : public QWidget
{
	Q_OBJECT

public:

	bool running;
	Timer timer;
	QVector<DisplayCameraFrame*> camFrames;
	vector<IBaseFilter*> Devices;
	vector<IAMCameraControl*> cameraControllers;

	DisplayCameraController(QWidget *parent, Config& config);

	~DisplayCameraController();

protected:
	
	bool event(QEvent *e);

};

#endif