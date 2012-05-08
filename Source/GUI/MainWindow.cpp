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

#include "MainWindow.h"
#include "SettingsWindow.h"
#include "DisplayCameras.h"

#include <iostream>
using namespace std;

QPixmap Mat2QPixmap(const Mat3b mat)
{
	// From http://stackoverflow.com/questions/5026965/how-to-convert-an-opencv-cvmat-to-qimage
	QImage qi(mat.cols, mat.rows, QImage::Format_ARGB32);
    for (int y = 0; y < mat.rows; ++y) {
            const cv::Vec3b *srcrow = mat[y];
            QRgb *destrow = (QRgb*)qi.scanLine(y);
            for (int x = 0; x < mat.cols; ++x) {
                    destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
            }
    }

	QPixmap qp;
	qp.convertFromImage(qi);

	return qp;
}

MainWindow::MainWindow(QWidget *parent)
	:config()
{
	config.readFromFile();

	QBoxLayout *mainLayout = new QVBoxLayout;
	QGridLayout *optionsLayout = new QGridLayout;

	QLabel *label;
	QBoxLayout *rowLayout;
	int index = 0;

	// Number of cameras
	label = new QLabel("Number of Cameras:");
	optionsLayout->addWidget(label, index, 0);

	rowLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	camCountGroup = new QButtonGroup;
	QRadioButton *camCountRadio1 = new QRadioButton("1", this);
	camCountRadio1->setChecked(config.camCount == 1);
	camCountGroup->addButton(camCountRadio1, 1);
	QRadioButton *camCountRadio2 = new QRadioButton("2", this);
	camCountRadio2->setChecked(config.camCount == 2);
	camCountGroup->addButton(camCountRadio2, 2);
	QRadioButton *camCountRadio3 = new QRadioButton("3", this);
	camCountRadio3->setChecked(config.camCount == 3);
	camCountGroup->addButton(camCountRadio3, 3);
	QRadioButton *camCountRadio4 = new QRadioButton("4", this);
	camCountRadio4->setChecked(config.camCount == 4);
	camCountGroup->addButton(camCountRadio4, 4);

	rowLayout->addWidget(camCountRadio1);
	rowLayout->addWidget(camCountRadio2);
	rowLayout->addWidget(camCountRadio3);
	rowLayout->addWidget(camCountRadio4);
	optionsLayout->addLayout(rowLayout, index, 1);
	
	connect(camCountGroup, SIGNAL(buttonClicked(int)), this, SLOT(camCountChanged(int)));
	index++;

	// Camera inversions
	label = new QLabel("Camera Inversions:");
	optionsLayout->addWidget(label, index, 0);

	rowLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	camInversionGroup = new QButtonGroup;
	camInversionGroup->setExclusive(false);
	QCheckBox *camInvertedBox1 = new QCheckBox("1", this);
	camInvertedBox1->setChecked(config.camInverted[0]);
	camInversionGroup->addButton(camInvertedBox1, 1);
	QCheckBox *camInvertedBox2 = new QCheckBox("2", this);
	camInvertedBox2->setChecked(config.camInverted[1]);
	camInversionGroup->addButton(camInvertedBox2, 2);
	QCheckBox *camInvertedBox3 = new QCheckBox("3", this);
	camInvertedBox3->setChecked(config.camInverted[2]);
	camInversionGroup->addButton(camInvertedBox3, 3);
	QCheckBox *camInvertedBox4 = new QCheckBox("4", this);
	camInvertedBox4->setChecked(config.camInverted[3]);
	camInversionGroup->addButton(camInvertedBox4, 4);

	rowLayout->addWidget(camInvertedBox1);
	rowLayout->addWidget(camInvertedBox2);
	rowLayout->addWidget(camInvertedBox3);
	rowLayout->addWidget(camInvertedBox4);
	optionsLayout->addLayout(rowLayout, index, 1);
	
	connect(camInversionGroup, SIGNAL(buttonClicked(int)), this, SLOT(camInversionChanged(int)));
	index++;
		
	// Frame resolution
	label = new QLabel("Frame Resolution:");
	optionsLayout->addWidget(label, index, 0);

	rowLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	QLabel *resolutionSeparatorLabel = new QLabel("x");
	widthLine = new QLineEdit;
	widthLine->setText(QString("%1").arg(config.camSizes[0][0]));
	heightLine = new QLineEdit;
	heightLine->setText(QString("%1").arg(config.camSizes[0][1]));

	rowLayout->addWidget(widthLine);
	rowLayout->addWidget(resolutionSeparatorLabel);
	rowLayout->addWidget(heightLine);
	optionsLayout->addLayout(rowLayout, index, 1);
	
	connect(widthLine, SIGNAL(textChanged(QString)), this, SLOT(resolutionChanged()));
	connect(heightLine, SIGNAL(textChanged(QString)), this, SLOT(resolutionChanged()));
	index++;

	mainLayout->addLayout(optionsLayout);

	// Submit buttons
	rowLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	showButton = new QPushButton("Show Cameras");
	runButton = new QPushButton("Run");
	quitButton = new QPushButton("Quit");

	rowLayout->addWidget(showButton);
	rowLayout->addWidget(runButton);
	rowLayout->addWidget(quitButton);
	mainLayout->addLayout(rowLayout);

	connect(showButton, SIGNAL(clicked()), this, SLOT(viewCameras()));
	connect(runButton, SIGNAL(clicked()), this, SLOT(runStitcHD()));
	connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

	index++;
	
	QGroupBox *group = new QGroupBox(this);
	group->setLayout(mainLayout);
	this->setCentralWidget(group);
	setWindowTitle("StitcHD Main Menu");
}

MainWindow::~MainWindow()
{
	config.writeToFile();
}

void MainWindow::camCountChanged(int value)
{
	config.camCount = value;

	switch(value)
	{
	case 1:
		config.hmgCount = 0;
		break;
	case 2:
		config.hmgCount = 1;
		break;
	case 3:
		config.hmgCount = 2;
		break;
	case 4:
		config.hmgCount = 4;
		break;
	default:
		break;
	}
}

void MainWindow::camInversionChanged(int value)
{
	config.camInverted[value-1] = camInversionGroup->button(value)->isChecked();
}

void MainWindow::resolutionChanged()
{
	for (int i=0; i<MAX_CAMERAS; i++)
	{
		config.camSizes[i][0] = widthLine->text().toInt();
		config.camSizes[i][1] = heightLine->text().toInt();
	}
}

void MainWindow::viewCameras()
{
	DisplayCameraController *display = new DisplayCameraController(this, config);
	display->setAttribute(Qt::WA_DeleteOnClose);
}

void MainWindow::runStitcHD()
{
	SettingsWindow* w = new SettingsWindow(this, &config);
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
}
