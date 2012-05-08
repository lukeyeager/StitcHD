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

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H 1

#include "Config.hpp"
#include "DisplayStitcHD.h"

#include <iostream>
using namespace std;

#include <QtGui/qmainwindow.h>
#include <QtGui/qwidget.h>
#include <QtGui/qlineedit.h>
#include <QtGui/qtextedit.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qlabel.h>
#include <QtGui/qcheckbox.h>
#include <QtGui/qradiobutton.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qspinbox.h>
#include <QtCore/qobject.h>
#include <QtGui/qmessagebox.h>
#include <QtGui/qbuttongroup.h>
#include <QtCore/qstring.h>
#include <QtGui/qgroupbox.h>
#include <QtGui/qslider.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qeventloop.h>
#include <QtCore/qthread.h>

class SettingsWindow : public QMainWindow
{
	Q_OBJECT

public:
	SettingsWindow(QWidget *parent, Config* config);
	Config* config;

public slots:
	//Settings button callbacks
	
	void showFpsChanged(int);

	void interpolationChanged(int);
	void alphaBlendChanged(int);
	void expBlendChanged(int);
	void frameTintChanged(int);
	void maxTintChanged(int);
	
	void showHmgMatchesChanged(int);
	void hmgOverlapChanged(int);
	void hmgAlphaChanged(int);
	void hessianChanged(int);
	void nOctavesChanged(int);
	void nOctaveLayersChanged(int);
	void extendedChanged(int);
	void uprightChanged(int);

	void flannOptChanged(int);
	void flannChecksChanged(int);
	void flannTreesChanged(int);
	void flannPrecisionChanged(int);
	void flannBuildChanged(int);
	void flannMemoryChanged(int);
	void flannFracChanged(int);

	void matchToleranceChanged(int);
	void ransacThresholdChanged(int);

	void setDefaults();
	void saveFrame();
	void toggleRecording();

protected:

	bool event(QEvent *e)
	{
		if (e->type() == QEvent::Close)
			running = false;

		if (running && e->type() == QEvent::User)
		{
			// Tell the stitcher to start up again
			QCoreApplication::postEvent(displayWindow, new QEvent(QEvent::User), Qt::LowEventPriority);
			return true;
		}
		return QWidget::event(e);
	}

private:

	bool running;
	bool recording;

	QButtonGroup *alphaBlendGroup, *flannOptGroup;
	QCheckBox *interpolationBox, *maxTintBox,
		*extendedBox, *uprightBox, *showHmgMatchesBox, *showFpsBox;
	QSlider *expBlendSlider, *tintSlider, *hmgOverlapSlider,
		*hmgAlphaSlider, *hessianSlider,
		*flannPrecisionSlider, *flannBuildSlider, *flannMemorySlider, *flannFracSlider,
		*toleranceSlider, *ransacSlider;
	QLabel *expBlendLabel, *tintLabel, *hmgOverlapLabel,
		*hmgAlphaLabel, *hessianLabel, *flannPrecisionLabel, *flannBuildLabel,
		*flannMemoryLabel, *flannFracLabel, *toleranceLabel, *ransacLabel;
	QSpinBox *hessianBox, *nOctaveBox, *nOctaveLayerBox, *flannChecksBox, *flannTreesBox;
	QPushButton *setDefaultsButton, *saveFrameButton, *recordButton;

	DisplayStitcHD *displayWindow;

	QGroupBox* buildStitcherSettings();
	QGroupBox* buildParamSettings();
	QGroupBox* buildSurfSettings();
	QGroupBox* buildMatchSettings();
	QGroupBox* buildHmgSettings();

};

#endif