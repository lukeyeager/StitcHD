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

#include "DisplayCameras.h"

DisplayCameraFrame::DisplayCameraFrame(QWidget *parent, int id, Config config, IAMCameraControl* camCon)
	:QMainWindow(parent),
	id(id),
	config(config),
	cameraController(camCon),
	camCapture(id,
		config.camSizes[id][0], 
		config.camSizes[id][1],
		config.camInverted[id],
		NULL,
		NULL)
{
	QGroupBox *group = new QGroupBox();
	QGridLayout *gridLayout = new QGridLayout();

	QBoxLayout *boxLayout = new QHBoxLayout();

	leftButton = new QPushButton("Pan Left");
	boxLayout->addWidget(leftButton);
	rightButton = new QPushButton("Pan Right");
	boxLayout->addWidget(rightButton);
	upButton = new QPushButton("Tilt Up");
	boxLayout->addWidget(upButton);
	downButton = new QPushButton("Tilt Down");
	boxLayout->addWidget(downButton);
	resetButton = new QPushButton("Re-center");
	boxLayout->addWidget(resetButton);
	gridLayout->addLayout(boxLayout, 0, 0, Qt::AlignCenter);

	connect(leftButton, SIGNAL(clicked()), this, SLOT(panLeft()));
	connect(rightButton, SIGNAL(clicked()), this, SLOT(panRight()));
	connect(upButton, SIGNAL(clicked()), this, SLOT(tiltUp()));
	connect(downButton, SIGNAL(clicked()), this, SLOT(tiltDown()));
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetPos()));

	frameWidget = new QLabel();
	gridLayout->addWidget(frameWidget, 1, 0, Qt::AlignCenter);

	group->setLayout(gridLayout);
	setCentralWidget(group);
	setWindowTitle(QString("Camera %1").arg(id + 1));
}

DisplayCameraFrame::~DisplayCameraFrame()
{
	cout << "Frame Resolution: " << camCapture.frame.cols << 'x' << camCapture.frame.rows << endl;
}

void DisplayCameraFrame::update()
{
	if (camCapture.initialize())
		return;

	camCapture.getFrame();

	if (camCapture.frame.cols == 0 && camCapture.frame.rows == 0)
		return;

	frameWidget->setPixmap(Mat2QPixmap(camCapture.frame));
	return ;
}

void DisplayCameraFrame::panLeft()
{
	if(config.camInverted[id] == false)
	{
		set_mechanical_pan_relative(cameraController, -1);
	}
	else
	{
		set_mechanical_pan_relative(cameraController, 1);
	}
}

void DisplayCameraFrame::panRight()
{
	if(config.camInverted[id] == false)
	{
		set_mechanical_pan_relative(cameraController, 1);
	}
	else
	{
		set_mechanical_pan_relative(cameraController, -1);
	}
}

void DisplayCameraFrame::tiltUp()
{
	if(config.camInverted[id] == false)
	{
		set_mechanical_tilt_relative(cameraController, -1);
	}
	else
	{
		set_mechanical_tilt_relative(cameraController, 1);
	}
}

void DisplayCameraFrame::tiltDown()
{
	if(config.camInverted[id] == false)
	{
		set_mechanical_tilt_relative(cameraController, 1);
	}
	else
	{
		set_mechanical_tilt_relative(cameraController, -1);
	}
}

void DisplayCameraFrame::resetPos()
{
	reset_mechanical_pan_tilt(cameraController);
}

DisplayCameraController::DisplayCameraController(QWidget *parent, Config& config)
	:QWidget(parent),
	timer(config),
	running(true)
{
	timer.start();

	//initialize DirectShow camera control objects
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	enum_devices(Devices);

	int devices_size = Devices.size();
	HRESULT hr;
	cameraControllers.resize(devices_size, 0);
	for (int i = 0; i < devices_size; i++)
	{
		hr = Devices[i]->QueryInterface(IID_IAMCameraControl, (void **)&cameraControllers[i]);
		if(hr != S_OK)   
		{   
			fprintf(stderr, "ERROR: Unable to access IAMCameraControl interface.\n");
		}
	}

	for (int i=0; i<config.camCount; i++)
	{
		DisplayCameraFrame *camFrame = new DisplayCameraFrame(this, i, config, cameraControllers[i]);
		camFrame->setAttribute(Qt::WA_DeleteOnClose);
		camFrame->show();

		connect(camFrame, SIGNAL(destroyed()), this, SLOT(close()));

		camFrames.push_back(camFrame);
	}

	QCoreApplication::postEvent(this, new QEvent(QEvent::User), Qt::LowEventPriority);
}

DisplayCameraController::~DisplayCameraController()
{
	for(int i = 0; i < Devices.size(); i++)
	{
		Devices[i]->Release();
	}
	//What about IAMCameraControl objects? these never get deleted in ptzProto2 main
	CoUninitialize();

	timer.stop();
	timer.writeToFile();
}

bool DisplayCameraController::event(QEvent *e)
{
	if (e->type() == QEvent::Close)
		running = false;

	if (running && e->type() >= QEvent::User && e->type() < QEvent::User + camFrames.size())
	{
		int index = e->type() - QEvent::User;

		camFrames[index]->update();

		if (index == camFrames.size() - 1)
			QCoreApplication::postEvent(this, new QEvent(QEvent::User), Qt::LowEventPriority);
		else
			QCoreApplication::postEvent(this, new QEvent(QEvent::Type(QEvent::User + index + 1)), Qt::LowEventPriority);

		return true;
	}
		
	return QWidget::event(e);
}
