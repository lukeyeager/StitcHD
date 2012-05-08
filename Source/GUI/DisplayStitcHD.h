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

#ifndef DISPLAYSTITCHD_H
#define DISPLAYSTITCHD_H 1

#include "VideoStitcher.hpp"
#include "MainWindow.h"
#include "Utils.h"

#include <string>
#include <iostream>
using namespace std;

#include <QtGui/qmainwindow.h>
#include <QtGui/qlabel.h>
#include <Qt/qevent.h>
#include <Qt/qcoreapplication.h>
#include <QtGui/qscrollarea.h>

class DisplayStitcHD : public QMainWindow
{
	Q_OBJECT

public:

	VideoStitcher stitcher;

	DisplayStitcHD(QWidget *parent, Config& config)
		:QMainWindow(parent),
		config(config),
		stitcher(config),
		running(false),
		saveNextFrame(false)
	{
		frame = new QLabel;
		frame->setAlignment(Qt::AlignCenter);

		scroll = new QScrollArea;
		scroll->setBackgroundRole(QPalette::Shadow);
		scroll->setWidgetResizable(true);
		scroll->setAlignment(Qt::AlignCenter);
		scroll->setWidget(frame);

		QBoxLayout *vBox = new QVBoxLayout;
		QBoxLayout *hBox = new QHBoxLayout;

		stitchLatency = new QLabel("");
		hBox->addWidget(stitchLatency);

		hmgLatency = new QLabel("");
		hBox->addWidget(hmgLatency);

		vBox->addLayout(hBox);
		vBox->addWidget(scroll);
		
		QWidget *mainWidget = new QWidget;
		mainWidget->setLayout(vBox);

		setCentralWidget(mainWidget);
			
		for (int i=0; i<5; i++)
			latencies.push_back(-1);
		latencyIndex = 0;

		setWindowTitle("Display");
	}

	~DisplayStitcHD()
	{
		stitcher.stop();
		stitcher.timer.writeToFile();
	}

protected:

	bool event(QEvent* e)
	{
		if (e->type() == QEvent::Close)
			running = false;

		else if (e->type() == QEvent::User + 1)
		{
			if (0 == stitcher.start())
				running = true;

			QCoreApplication::postEvent(parentWidget(), new QEvent(QEvent::User), Qt::LowEventPriority);
			return true;
		}
		else if (e->type() == QEvent::User)
		{
			if (!running)
				cout << "ERROR: Can't stitch because the stitcher isn't running." << endl;
			else
			{
				if (0 == stitcher.getImage())
				{
					frame->resize(qMax(stitcher.displayFrame.cols, scroll->width()), qMax(stitcher.displayFrame.rows, scroll->height()));
					frame->setPixmap(Mat2QPixmap(stitcher.displayFrame));
					if (saveNextFrame)
					{
						imwrite(pictureOutputFileName(), stitcher.displayFrame);
						saveNextFrame = false;
					}
				}
				
				if (config.showFps)
				{
					hmgLatency->show();
					stitchLatency->show();

					hmgLatency->setText(QString("Homographier Latency: %1 ms").arg(stitcher.hmgLatency));
					
					latencies[latencyIndex] = clock();
					int latency = Timer::msTime(latencies[(latencyIndex + 1) % latencies.size()], latencies[latencyIndex]) / latencies.size();
					latencyIndex = (latencyIndex + 1) % latencies.size();

					stitchLatency->setText(QString("Stitch Latency: %1 ms").arg(latency));
				}
				else
				{
					hmgLatency->hide();
					stitchLatency->hide();
				}
			}

			QCoreApplication::postEvent(parentWidget(), new QEvent(QEvent::User), Qt::LowEventPriority);
			return true;
		}
		// Close match frames event
		else if (e->type() == QEvent::User + 2)
		{
			stitcher.closeMatchFrames();
			return true;
		}
		// Start recording event
		else if (e->type() == QEvent::User + 3)
		{
			stitcher.startRecording();
			return true;
		}
		// Stop recording event
		else if (e->type() == QEvent::User + 4)
		{
			stitcher.stopRecording();
			return true;
		}
		// Save frame event
		else if (e->type() == QEvent::User + 5)
		{
			saveNextFrame = true;
			return true;
		}
		else
			return QWidget::event(e);
	}

private:

	bool running;
	bool saveNextFrame;
	Config& config;
	QLabel *frame;
	QLabel *stitchLatency, *hmgLatency;
	QScrollArea *scroll;

	vector<clock_t> latencies;
	int latencyIndex;
};

#endif
