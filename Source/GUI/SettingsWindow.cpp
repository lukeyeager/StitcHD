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

#define LABEL_WIDTH 30

#include "SettingsWindow.h"

#include <QtGui/qsizepolicy.h>

QGroupBox* SettingsWindow::buildStitcherSettings()
{
	QGroupBox *group = new QGroupBox("Stitcher Settings", this);

	QGridLayout *grid = new QGridLayout;
	QBoxLayout *row;
	QLabel *label;
	QRadioButton *radioButton;
	int index = 0;
	QString tip;

	// Interpolation
	tip = "<p>When warping the images, the stitcher can take the pixel value from the nearest image pixel to the transformed pixel (Disabled).</p> \
		  <p>Or, it can take the distance-weighted, bilinear average of the four points surrounding the calculated point (Enabled).<\p>";
	label = new QLabel("Interpolate:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	interpolationBox = new QCheckBox("", this);
	interpolationBox->setChecked(config->interpolate == 1);
	interpolationBox->setToolTip(tip);
	grid->addWidget(interpolationBox, index, 1);
	
	connect(interpolationBox, SIGNAL(stateChanged(int)), this, SLOT(interpolationChanged(int)));
	index++;

	// Alpha blend radio buttons
	label = new QLabel("Blending style:");
	label->setToolTip("<p>Choose which type of blending you want to use between the images.</p>");
	grid->addWidget(label, index, 0);

	alphaBlendGroup = new QButtonGroup;
	row = new QBoxLayout(QBoxLayout::LeftToRight);

	radioButton = new QRadioButton("Overlay", this);
	radioButton->setToolTip("<p>No Blending (Overlay)</p> \
		  <p>In overlap regions, the stitcher simply takes the value of the lower-indexed camera.</p>");
	radioButton->setChecked(config->alphaBlend == 0);
	alphaBlendGroup->addButton(radioButton, 0);
	row->addWidget(radioButton);

	radioButton = new QRadioButton("Average", this);
	radioButton->setToolTip("<p>Average Blending</p> \
		  <p>In overlap regions, the stitcher uses the average value of all overlapping images.</p>");
	radioButton->setChecked(config->alphaBlend == 1);
	alphaBlendGroup->addButton(radioButton, 1);
	row->addWidget(radioButton);

	radioButton = new QRadioButton("Linear", this);
	radioButton->setToolTip("<p>Linear Blending</p> \
		  <p>In overlap regions, the stitcher weights each images' pixel linearly with respect to the distance from the center of the image.</p>");
	radioButton->setChecked(config->alphaBlend == 2);
	alphaBlendGroup->addButton(radioButton, 2);
	row->addWidget(radioButton);

	radioButton = new QRadioButton("Exponential", this);
	radioButton->setToolTip("<p>Exponential Blending</p> \
		  <p>In overlap regions, the stitcher weights each images' pixel linearly with respect to the distance from the center of the image.</p>");
	radioButton->setChecked(config->alphaBlend == 3);
	alphaBlendGroup->addButton(radioButton, 3);
	row->addWidget(radioButton);

	grid->addLayout(row, index, 1);
	
	connect(alphaBlendGroup, SIGNAL(buttonClicked(int)), this, SLOT(alphaBlendChanged(int)));
	index++;

	// Exponential blend slider
	tip = "<p>Exponential Blend Level</p> \
		  <p>A higher value corresponds to a smaller visible overlap region.</p>";
	label = new QLabel("   Exp. Blend Level:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	row = new QBoxLayout(QBoxLayout::LeftToRight);
	expBlendSlider = new QSlider(Qt::Horizontal, this);
	expBlendSlider->setToolTip(tip);
	expBlendSlider->setRange(0, 100);
	expBlendSlider->setValue(config->expBlendValue);
	expBlendSlider->setEnabled(config->alphaBlend == 3);
	row->addWidget(expBlendSlider);

	expBlendLabel = new QLabel(QString("%1%").arg(config->expBlendValue));
	expBlendLabel->setToolTip(tip);
	expBlendLabel->setMinimumWidth(LABEL_WIDTH);
	row->addWidget(expBlendLabel);

	grid->addLayout(row, index, 1);

	connect(expBlendSlider, SIGNAL(sliderMoved(int)), this, SLOT(expBlendChanged(int)));
	index++;

	// Tint slider
	tip = "<p>Frame Tint</p> \
		  <p>Helps to delineate the frames from one another.</p>";
	label = new QLabel("Frame Tint Level:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	row = new QBoxLayout(QBoxLayout::LeftToRight);
	tintSlider = new QSlider(Qt::Horizontal, this);
	tintSlider->setToolTip(tip);
	tintSlider->setRange(0, 255);
	tintSlider->setValue(config->frameTint);
	row->addWidget(tintSlider);

	tintLabel = new QLabel(QString("%1").arg(config->frameTint));
	tintLabel->setToolTip(tip);
	tintLabel->setMinimumWidth(LABEL_WIDTH);
	row->addWidget(tintLabel);

	grid->addLayout(row, index, 1);

	index++;
	connect(tintSlider, SIGNAL(sliderMoved(int)), this, SLOT(frameTintChanged(int)));

	// Max tint box
	label = new QLabel("Set Maximum Tint:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	maxTintBox = new QCheckBox("", this);
	maxTintBox->setToolTip(tip);
	maxTintBox->setChecked(config->maxTint == 1);

	grid->addWidget(maxTintBox, index, 1);

	connect(maxTintBox, SIGNAL(stateChanged(int)), this, SLOT(maxTintChanged(int)));
	index++;

	group->setLayout(grid);
	group->setFlat(true);
	return group;
}

QGroupBox* SettingsWindow::buildParamSettings()

{
	// MISC
	//frameOverlap: slider
	//hmgTransitionAlpha: slider

	// SURF
	//hessianThreshold: text box
	//nOctaves: text box
	//nOctaveLayers: text box
	//extended: checkbox
	//upright: checkbox

	// MATCH
	//flannMatchOpt: radiobuttons
	//flannChecks: text box
	//flannTrees: text box
	//flannTargetPrecision: slider
	//flannBuildWeight: slider
	//flannMemoryWeight: slider?
	//flannSampleFraction: slider

	// HOMOGRAPHY
	//matchTolerance: slider
	//ransacReprojThresh: text box

	QGroupBox *group = new QGroupBox("Parameter Calculation Settings", this);

	QBoxLayout *settingsLayout = new QVBoxLayout;

	QGridLayout *grid = new QGridLayout;
	QBoxLayout *row;
	QLabel *label;
	QString tip;
	int index = 0;
	
	// Show hmg matches
	tip = "<p>Displays the matching keypoints used to calculate homographies between frames.<p> \
			<p>This can be helpful when changing any value in Parameter Calculation Settings.</p>";
	label = new QLabel("Show Matching Keypoints:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	showHmgMatchesBox = new QCheckBox;
	showHmgMatchesBox->setToolTip(tip);
	showHmgMatchesBox->setChecked(config->showMatches);
	grid->addWidget(showHmgMatchesBox, index, 1);
	
	connect(showHmgMatchesBox, SIGNAL(stateChanged(int)), this, SLOT(showHmgMatchesChanged(int)));
	index++;

	// Homography overlap
	tip = "<p>Specifies what percentage of the frame is shared between both cameras.</p> \
					  <p>Adjusting this setting stops keypoints from being calculated in non-shared regions, \
					  saving time and preventing false matches.</p>";
	label = new QLabel("Frame Overlap:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);
	
	row = new QBoxLayout(QBoxLayout::LeftToRight);
	hmgOverlapSlider = new QSlider(Qt::Horizontal, this);
	hmgOverlapSlider->setRange(0, 99);
	hmgOverlapSlider->setValue(config->frameOverlap);
	hmgOverlapSlider->setToolTip(tip);
	row->addWidget(hmgOverlapSlider);

	hmgOverlapLabel = new QLabel(QString("%1%").arg(config->frameOverlap));
	hmgOverlapLabel->setToolTip(tip);
	hmgOverlapLabel->setMinimumWidth(LABEL_WIDTH);
	row->addWidget(hmgOverlapLabel);
	grid->addLayout(row, index, 1);
	
	connect(hmgOverlapSlider, SIGNAL(sliderMoved(int)), this, SLOT(hmgOverlapChanged(int)));
	index++;
	
	// Homography alpha
	tip = "<p>Sets the amount of weight a new homography has when averaged with the previous homography.</p> \
					  <p>Lower values result in a smoother image but slower updating, while higher values update more quickly \
					  at the cost of more drastic image shifting.</p>";
	label = new QLabel("Time Average Weight:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	row = new QBoxLayout(QBoxLayout::LeftToRight);
	hmgAlphaSlider = new QSlider(Qt::Horizontal, this);
	hmgAlphaSlider->setRange(0, 100);
	hmgAlphaSlider->setValue(config->hmgTransitionAlpha);
	hmgAlphaSlider->setToolTip(tip);
	row->addWidget(hmgAlphaSlider);

	hmgAlphaLabel = new QLabel(QString("%1%").arg(config->hmgTransitionAlpha));
	hmgAlphaLabel->setToolTip(tip);
	hmgAlphaLabel->setMinimumWidth(LABEL_WIDTH);
	row->addWidget(hmgAlphaLabel);

	grid->addLayout(row, index, 1);
	
	connect(hmgAlphaSlider, SIGNAL(sliderMoved(int)), this, SLOT(hmgAlphaChanged(int)));
	index++;

	settingsLayout->addLayout(grid);
	settingsLayout->addWidget(buildSurfSettings());
	settingsLayout->addWidget(buildMatchSettings());
	settingsLayout->addWidget(buildHmgSettings());

	group->setLayout(settingsLayout);
	group->setFlat(true);
	return group;
}

QGroupBox* SettingsWindow::buildSurfSettings()
{
	QGroupBox *group = new QGroupBox(QString("SURF Settings"));

	QGridLayout *grid = new QGridLayout;
	QBoxLayout *row;
	QLabel* label;
	QString tip;
	int index = 0;

	// Hessian threshold
	tip = "<p>The Hessian threshold determines how many keypoints will be calculated.</p> \
					  <p>A lower threshold allows more keypoints, while a higher threshold reduces the \
					  keypoints calculated.</p>";
	label = new QLabel("Hessian Threshold:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	/*row = new QBoxLayout(QBoxLayout::LeftToRight);
	hessianSlider = new QSlider(Qt::Horizontal, this);
	hessianSlider->setRange(0, 2000);
	hessianSlider->setValue(config->hessianThreshold);
	hessianSlider->setToolTip(tip);
	row->addWidget(hessianSlider);
	hessianLabel = new QLabel(QString("%1").arg(config->hessianThreshold));
	hessianLabel->setToolTip(tip);
	row->addWidget(hessianLabel);
	
	grid->addLayout(row, index, 1);*/

	hessianBox = new QSpinBox(this);
	hessianBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
	hessianBox->setRange(0, 2000);
	hessianBox->setValue(config->hessianThreshold);
	hessianBox->setToolTip(tip);
	
	grid->addWidget(hessianBox, index, 1);
	
	//connect(hessianSlider, SIGNAL(sliderMoved(int)), this, SLOT(hessianChanged(int)));
	connect(hessianBox, SIGNAL(valueChanged(int)), this, SLOT(hessianChanged(int)));
	index++;
	
	// Number of octaves
	tip = "<p>The number of a gaussian pyramid octaves that the detector uses.</p> \
					  <p>If you want to get very large features, use the larger value. \
					  If you want just small features, decrease it.</p>";
	label = new QLabel("nOctaves:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	nOctaveBox = new QSpinBox(this);
	nOctaveBox->setRange(1, 100);
	nOctaveBox->setValue(config->nOctaves);
	nOctaveBox->setToolTip(tip);

	grid->addWidget(nOctaveBox, index, 1);
	
	connect(nOctaveBox, SIGNAL(valueChanged(int)), this, SLOT(nOctavesChanged(int)));
	index++;

	// Octave layers
	tip = "<p>The number of images within each octave of a gaussian pyramid.</p> \
					  <p>We believe increasing this number tends to increase accuracy.</n>";
	label = new QLabel("nOctaveLayers:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	nOctaveLayerBox = new QSpinBox(this);
	nOctaveLayerBox->setRange(1, 100);
	nOctaveLayerBox->setValue(config->nOctaveLayers);
	nOctaveLayerBox->setToolTip(tip);

	grid->addWidget(nOctaveLayerBox, index, 1);
	
	connect(nOctaveLayerBox, SIGNAL(valueChanged(int)), this, SLOT(nOctaveLayersChanged(int)));
	index++;
	
	// Extended keypoints
	tip = "<p>Determines type of keypoints to calculate.</p> \
					  <p>False means that basic descriptors of 64 elements will be calculated, \
					  and true means that extended descriptors of 128 elements will be calculated.</p>";
	label = new QLabel("Use Extended Keypoints:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	extendedBox = new QCheckBox("", this);
	extendedBox->setChecked(config->extended);
	extendedBox->setToolTip(tip);

	grid->addWidget(extendedBox, index, 1);
	
	connect(extendedBox, SIGNAL(stateChanged(int)), this, SLOT(extendedChanged(int)));
	index++;
	
	// Upright keypoints
	tip = "<p>Determines whether or not to calculate orientation data for keypoints.</p> \
					  <p>If false, orientation data is calculated. If true, orientation calculation is skipped, \
					  speeding up the process if the images are at similar rotation angles (as in cameras \
					  mounted on the same plane).</p>";
	label = new QLabel("Use Upright Keypoints:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	uprightBox = new QCheckBox("", this);
	uprightBox->setChecked(config->upright);
	uprightBox->setToolTip(tip);

	grid->addWidget(uprightBox, index, 1);

	connect(uprightBox, SIGNAL(stateChanged(int)), this, SLOT(uprightChanged(int)));
	index++;

	label = new QLabel("Reference:");
	grid->addWidget(label, index, 0);
	label = new QLabel(
		QString("<a href='http://opencv.itseez.com/2.3/modules/features2d/doc/feature_detection_and_description.html#surf'>OpenCV SURF Documentation</a>"));
	label->setOpenExternalLinks(true);
	grid->addWidget(label, index, 1);
	index++;

	group->setLayout(grid);
	return group;
}

QGroupBox* SettingsWindow::buildMatchSettings()
{
	QGroupBox *group = new QGroupBox("Match Settings");

	QGridLayout *grid = new QGridLayout;
	QBoxLayout *row;
	QRadioButton *radioButton;
	QLabel* label;
	QString tip;
	int index = 0;

	// Flann options
	tip = "<p>Choose which type of search index to construct for \
					  FLANN nearest neighbor matching.</p>";
	row = new QBoxLayout(QBoxLayout::LeftToRight);
	label = new QLabel("FLANN Matching Type:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	flannOptGroup = new QButtonGroup;

	radioButton = new QRadioButton("Brute-Force", this);
	radioButton->setChecked(config->flannMatchOpt == 0);
	radioButton->setToolTip("<p>Matching will be performed with a brute-force keypoint search.</p>");
	flannOptGroup->addButton(radioButton, 0);
	row->addWidget(radioButton);

	radioButton = new QRadioButton("KD-Tree", this);
	radioButton->setChecked(config->flannMatchOpt == 1);
	radioButton->setToolTip("<p>Matching will be performed by searching a set of kd-trees in parallel.</p>");
	flannOptGroup->addButton(radioButton, 1);
	row->addWidget(radioButton);

	radioButton = new QRadioButton("Autotuned", this);
	radioButton->setChecked(config->flannMatchOpt == 2);
	radioButton->setToolTip("<p>Based on the autotuned parameters, the algorithm will build a custom index for optimum \
							performance. This option tends to be accurate, but slow.</p>");
	flannOptGroup->addButton(radioButton, 2);
	row->addWidget(radioButton);
	
	grid->addLayout(row, index, 1);
	
	connect(flannOptGroup, SIGNAL(buttonClicked(int)), this, SLOT(flannOptChanged(int)));
	index++;

	// Flann checks
	tip = "<p>The number of times the trees in the index will be traversed during each search.</p> \
					  <p> A higher value is more accurate, but loses speed.</p>";
	label = new QLabel("Checks:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	flannChecksBox = new QSpinBox(this);
	flannChecksBox->setRange(1, 100);
	flannChecksBox->setValue(config->flannChecks);
	flannChecksBox->setToolTip(tip);

	grid->addWidget(flannChecksBox, index, 1);
	
	connect(flannChecksBox, SIGNAL(valueChanged(int)), this, SLOT(flannChecksChanged(int)));
	index++;

	// Flann trees
	tip = "<p>Sets the number of kd-trees to use in the KD-Tree option.</p>";
	label = new QLabel("KD-Trees:");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	flannTreesBox = new QSpinBox(this);
	flannTreesBox->setRange(1, 16);
	flannTreesBox->setValue(config->flannTrees);
	flannTreesBox->setEnabled(config->flannMatchOpt == 1);
	flannTreesBox->setToolTip(tip);

	grid->addWidget(flannTreesBox, index, 1);
	
	connect(flannTreesBox, SIGNAL(valueChanged(int)), this, SLOT(flannTreesChanged(int)));
	index++;

	// Flann options
	label = new QLabel("Autotuned Params:");
	label->setToolTip("<p>Set autotuned values, used only with Autotuned option.</p>");
	grid->addWidget(label, index, 0);
	
	row = new QBoxLayout(QBoxLayout::LeftToRight);

	// Flann precision
	tip = "<p>Sets precision by what percentage of searches return the exact nearest neighbor.</p> \
				<p>A higher value gives more accurate results, but searches are slower.</p>";
	label = new QLabel("Target\nPrecis.");
	label->setToolTip(tip);
	row->addWidget(label);

	flannPrecisionSlider = new QSlider(Qt::Vertical, this);
	flannPrecisionSlider->setRange(0, 100);
	flannPrecisionSlider->setValue(config->flannTargetPrecision);
	flannPrecisionSlider->setEnabled(config->flannMatchOpt == 2);
	flannPrecisionSlider->setToolTip(tip);
	row->addWidget(flannPrecisionSlider);

	flannPrecisionLabel = new QLabel(QString("%1%").arg(config->flannTargetPrecision));
	flannPrecisionLabel->setToolTip(tip);
	flannPrecisionLabel->setMinimumWidth(LABEL_WIDTH + 10);
	row->addWidget(flannPrecisionLabel);

	connect(flannPrecisionSlider, SIGNAL(sliderMoved(int)), this, SLOT(flannPrecisionChanged(int)));

	// Flann build / search time ratio
	tip = "<p>Sets the importance of index build time compared to individual search times.</p> \
			<p>Lower values mean that the index takes longer to build, but searches go faster.</p>";
	label = new QLabel("Build\nvs.\nSearch\nRatio");
	label->setToolTip(tip);
	row->addWidget(label);

	flannBuildSlider = new QSlider(Qt::Vertical, this);
	flannBuildSlider->setRange(0, 100);
	flannBuildSlider->setValue(config->flannBuildWeight);
	flannBuildSlider->setEnabled(config->flannMatchOpt == 2);
	flannBuildSlider->setToolTip(tip);
	row->addWidget(flannBuildSlider);
	
	flannBuildLabel = new QLabel(QString("%1%").arg(config->flannBuildWeight));
	flannBuildLabel->setToolTip(tip);
	flannBuildLabel->setMinimumWidth(LABEL_WIDTH + 10);
	row->addWidget(flannBuildLabel);

	connect(flannBuildSlider, SIGNAL(sliderMoved(int)), this, SLOT(flannBuildChanged(int)));

	// Flann memory / time ratio
	tip = "<p>Sets the tradeoff between computation time and memory use in the index.</p> \
			<p>Values less than 1 give more importance to memory, and values greater than \
			1 give importance to memory usage.</p>";
	label = new QLabel("Time\nvs.\nMemory");
	label->setToolTip(tip);
	row->addWidget(label);

	flannMemorySlider = new QSlider(Qt::Vertical, this);
	flannMemorySlider->setRange(-200, 400);
	flannMemorySlider->setValue(config->flannMemoryWeight);
	flannMemorySlider->setEnabled(config->flannMatchOpt == 2);
	flannMemorySlider->setToolTip(tip);
	row->addWidget(flannMemorySlider);
	
	flannMemoryLabel = new QLabel(QString("%1").arg(float(config->flannMemoryWeight) / 100.0));
	flannMemoryLabel->setToolTip(tip);
	flannMemoryLabel->setMinimumWidth(LABEL_WIDTH + 10);
	row->addWidget(flannMemoryLabel);
	
	connect(flannMemorySlider, SIGNAL(sliderMoved(int)), this, SLOT(flannMemoryChanged(int)));

	// Flann dataset fraction
	tip = "<p>Sets what fraction of the dataset to use for parameter configuration.</p> \
			<p>100% is most accurate, but can take too long if the dataset is very large. \
			A smaller subset of data can give good approximations at faster speeds.</p>";
	label = new QLabel("Dataset \nFraction \nUsed");
	label->setToolTip(tip);
	row->addWidget(label);

	flannFracSlider = new QSlider(Qt::Vertical, this);
	flannFracSlider->setRange(0, 100);
	flannFracSlider->setValue(config->flannSampleFraction);
	flannFracSlider->setEnabled(config->flannMatchOpt == 2);
	flannFracSlider->setToolTip(tip);
	row->addWidget(flannFracSlider);

	flannFracLabel = new QLabel(QString("%1%").arg(config->flannSampleFraction));
	flannFracLabel->setToolTip(tip);
	flannFracLabel->setMinimumWidth(LABEL_WIDTH + 10);
	row->addWidget(flannFracLabel);

	grid->addLayout(row, index, 1);

	connect(flannFracSlider, SIGNAL(sliderMoved(int)), this, SLOT(flannFracChanged(int)));
	index++;

	label = new QLabel("Reference:");
	grid->addWidget(label, index, 0);
	label = new QLabel(
		QString("<a href='http://opencv.itseez.com/2.3/modules/flann/doc/flann_fast_approximate_nearest_neighbor_search.html#flann-index-t-index'>OpenCV FLANN Documentation</a>"));
	label->setOpenExternalLinks(true);
	grid->addWidget(label, index, 1);
	index++;

	group->setLayout(grid);
	return group;
}

QGroupBox* SettingsWindow::buildHmgSettings()
{
	QGroupBox *group = new QGroupBox("Homography Settings");

	QGridLayout *grid = new QGridLayout;
	QBoxLayout *row;
	QRadioButton *radioButton;
	QLabel* label;
	QString tip;
	int index = 0;
	
	// Keypoint distance tolerance
	tip = "<p>Sets what percentage of keypoint matches to reject based on deviance from \
			the mean distance. (Correct keypoint matches should have the same distance from \
			keypoint to keypoint across images.)</p> \
			<p>Higher values accept more keypoint matches, and lower values accept less matches.</p>";
	label = new QLabel(tr("Match Dist. Tolerance"));
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);

	row = new QBoxLayout(QBoxLayout::LeftToRight);
	toleranceSlider = new QSlider(Qt::Horizontal, this);
	toleranceSlider->setRange(0, 100);
	toleranceSlider->setValue(config->matchTolerance);
	toleranceSlider->setToolTip(tip);
	row->addWidget(toleranceSlider);

	toleranceLabel = new QLabel(QString("%1%").arg(config->matchTolerance));
	toleranceLabel->setToolTip(tip);
	toleranceLabel->setMinimumWidth(LABEL_WIDTH);
	row->addWidget(toleranceLabel);

	grid->addLayout(row, index, 1);
	
	connect(toleranceSlider, SIGNAL(sliderMoved(int)), this, SLOT(matchToleranceChanged(int)));
	index++;
	
	// Ransac threshold
	tip = "<p>Maximum allowed reprojection error to accept a match in the RANSAC method.</p> \
			<p>Lower values cause more matches to be treated as outliers, excluding them from homography \
			calculation. NOTE: Matches rejected by RANSAC will still appear on the homography matches display.</p>";
	label = new QLabel("RANSAC Threshold");
	label->setToolTip(tip);
	grid->addWidget(label, index, 0);
	
	row = new QBoxLayout(QBoxLayout::LeftToRight);
	ransacSlider = new QSlider(Qt::Horizontal, this);
	ransacSlider->setRange(10, 150);
	ransacSlider->setValue(config->ransacReprojThresh);
	ransacSlider->setToolTip(tip);
	row->addWidget(ransacSlider);

	ransacLabel = new QLabel(QString("%1").arg((float)config->ransacReprojThresh / 10.0));
	ransacLabel->setToolTip(tip);
	ransacLabel->setMinimumWidth(LABEL_WIDTH);
	row->addWidget(ransacLabel);

	grid->addLayout(row, index, 1);

	connect(ransacSlider, SIGNAL(sliderMoved(int)), this, SLOT(ransacThresholdChanged(int)));
	index++;

	label = new QLabel("Reference:");
	grid->addWidget(label, index, 0);
	label = new QLabel(
		QString("<a href='http://opencv.itseez.com/2.3/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html#findhomography'>OpenCV Homography Documentation</a>"));
	label->setOpenExternalLinks(true);
	grid->addWidget(label, index, 1);
	index++;
	
	group->setLayout(grid);
	return group;
}

SettingsWindow::SettingsWindow(QWidget *parent, Config* config)
	:QMainWindow(parent), config(config)
{
	QBoxLayout *mainLayout = new QVBoxLayout;
	QBoxLayout *hBox;
	
	// Show fps
	QString tip = "<p>Displays the framerate for each major module.</p>";
	hBox = new QHBoxLayout;
	hBox->setAlignment(Qt::AlignLeft);
	QLabel* label = new QLabel("   Show Framerate:");
	label->setToolTip(tip);
	hBox->addWidget(label);

	showFpsBox = new QCheckBox;
	showFpsBox->setChecked(config->showFps);
	showFpsBox->setToolTip(tip);
	hBox->addWidget(showFpsBox);

	connect(showFpsBox, SIGNAL(stateChanged(int)), this, SLOT(showFpsChanged(int)));
	
	// Add widgets
	mainLayout->addLayout(hBox);
	mainLayout->addWidget(buildStitcherSettings());
	mainLayout->addWidget(buildParamSettings());

	// Add buttons
	hBox = new QHBoxLayout;
	setDefaultsButton = new QPushButton("Set Defaults");
	connect(setDefaultsButton, SIGNAL(clicked()), this, SLOT(setDefaults()));
	hBox->addWidget(setDefaultsButton);

	saveFrameButton = new QPushButton("Save Frame");
	connect(saveFrameButton, SIGNAL(clicked()), this, SLOT(saveFrame()));
	hBox->addWidget(saveFrameButton);

	recordButton = new QPushButton("Record");
	connect(recordButton, SIGNAL(clicked()), this, SLOT(toggleRecording()));
	hBox->addWidget(recordButton);

	mainLayout->addLayout(hBox);

	QWidget *centralWidget = new QWidget;
	centralWidget->setLayout(mainLayout);
	this->setCentralWidget(centralWidget);
	setWindowTitle("StitcHD Settings");

	displayWindow = new DisplayStitcHD(this, *config);
	displayWindow->setAttribute(Qt::WA_DeleteOnClose);
	displayWindow->setMinimumSize(600, 400);
	displayWindow->show();

	connect(displayWindow, SIGNAL(destroyed()), this, SLOT(close()));

	// Ask the stitcher to initialize
	QCoreApplication::postEvent(displayWindow, new QEvent(QEvent::Type(QEvent::User + 1)), Qt::LowEventPriority);
	running = true;
	recording = false;
}

void SettingsWindow::interpolationChanged(int value)
{
	config->interpolate = (value == Qt::Checked);
}

void SettingsWindow::alphaBlendChanged(int value)
{
	config->alphaBlend = value;

	expBlendSlider->setEnabled(value == 3);
}

void SettingsWindow::expBlendChanged(int value)
{
	config->expBlendValue = value;
	expBlendLabel->setText(QString("%1%").arg(value));
}

void SettingsWindow::frameTintChanged(int value)
{
	config->frameTint = value;
	tintLabel->setText(QString("%1").arg(value));
}

void SettingsWindow::maxTintChanged(int value)
{
	config->maxTint = (value == Qt::Checked);
}

void SettingsWindow::hmgOverlapChanged(int value)
{
	config->frameOverlap = value;
	hmgOverlapLabel->setText(QString("%1%").arg(value));
}

void SettingsWindow::hmgAlphaChanged(int value)
{
	config->hmgTransitionAlpha = value;
	hmgAlphaLabel->setText(QString("%1%").arg(value));
}

void SettingsWindow::hessianChanged(int value)
{
	config->hessianThreshold = value;
	//hessianLabel->setText(QString("%1").arg(value));
}

void SettingsWindow::nOctavesChanged(int value)
{
	config->nOctaves = value;
}

void SettingsWindow::nOctaveLayersChanged(int value)
{
	config->nOctaveLayers = value;
}

void SettingsWindow::extendedChanged(int value)
{
	config->extended = (value == Qt::Checked);
}

void SettingsWindow::uprightChanged(int value)
{
	config->upright = (value == Qt::Checked);
}

void SettingsWindow::flannOptChanged(int value)
{
	config->flannMatchOpt = value;

	flannTreesBox->setEnabled(value == 1);
	flannPrecisionSlider->setEnabled(value == 2);
	flannBuildSlider->setEnabled(value == 2);
	flannMemorySlider->setEnabled(value == 2);
	flannFracSlider->setEnabled(value == 2);
}

void SettingsWindow::flannChecksChanged(int value)
{
	config->flannChecks = value;
}

void SettingsWindow::flannTreesChanged(int value)
{
	config->flannTrees = value;
}

void SettingsWindow::flannPrecisionChanged(int value)
{
	config->flannTargetPrecision = value;
	flannPrecisionLabel->setText(QString("%1%").arg(value));
}

void SettingsWindow::flannBuildChanged(int value)
{
	config->flannBuildWeight = value;
	flannBuildLabel->setText(QString("%1%").arg(value));
}

void SettingsWindow::flannMemoryChanged(int value)
{
	config->flannMemoryWeight = value;
	flannMemoryLabel->setText(QString("%1").arg((float) value / 100.0));
}

void SettingsWindow::flannFracChanged(int value)
{
	config->flannSampleFraction = value;
	flannFracLabel->setText(QString("%1%").arg(value));
}

void SettingsWindow::matchToleranceChanged(int value)
{
	config->matchTolerance = value;
	toleranceLabel->setText(QString("%1%").arg(value));
}

void SettingsWindow::ransacThresholdChanged(int value)
{
	config->ransacReprojThresh = value;
	ransacLabel->setText(QString("%1").arg((float)config->ransacReprojThresh / 10.0));
}

void SettingsWindow::showHmgMatchesChanged(int value)
{
	config->showMatches = (value == Qt::Checked);
	if (value != Qt::Checked)
		QCoreApplication::postEvent(displayWindow, new QEvent(QEvent::Type(QEvent::User + 2)), Qt::LowEventPriority);
}

void SettingsWindow::showFpsChanged(int value)
{
	config->showFps = (value == Qt::Checked);
}

void SettingsWindow::setDefaults()
{
	Config def;

	showFpsBox->setChecked(def.showFps);

	interpolationBox->setChecked(def.interpolate);
	alphaBlendGroup->button(def.alphaBlend)->setChecked(true);
	alphaBlendChanged(def.alphaBlend);

	expBlendSlider->setValue(def.expBlendValue);
	expBlendChanged(def.expBlendValue);
	tintSlider->setValue(def.frameTint);
	frameTintChanged(def.frameTint);
	maxTintBox->setChecked(def.maxTint);
	
	showHmgMatchesBox->setChecked(def.showMatches);
	hmgOverlapSlider->setValue(def.frameOverlap);
	hmgOverlapChanged(def.frameOverlap);
	hmgAlphaSlider->setValue(def.hmgTransitionAlpha);
	hmgAlphaChanged(def.hmgTransitionAlpha);
	//hessianSlider->setValue(def.hessianThreshold);
	//hessianChanged(def.hessianThreshold);
	hessianBox->setValue(def.hessianThreshold);
	nOctaveBox->setValue(def.nOctaves);
	nOctaveLayerBox->setValue(def.nOctaveLayers);
	extendedBox->setChecked(def.extended);
	uprightBox->setChecked(def.upright);

	flannOptGroup->button(def.flannMatchOpt)->setChecked(true);
	flannOptChanged(def.flannMatchOpt);
	flannChecksBox->setValue(def.flannChecks);
	flannTreesBox->setValue(def.flannTrees);
	flannPrecisionSlider->setValue(def.flannTargetPrecision);
	flannPrecisionChanged(def.flannTargetPrecision);
	flannBuildSlider->setValue(def.flannBuildWeight);
	flannBuildChanged(def.flannBuildWeight);
	flannMemorySlider->setValue(def.flannMemoryWeight);
	flannMemoryChanged(def.flannMemoryWeight);
	flannFracSlider->setValue(def.flannSampleFraction);
	flannFracChanged(def.flannSampleFraction);

	toleranceSlider->setValue(def.matchTolerance);
	matchToleranceChanged(def.matchTolerance);
	ransacSlider->setValue(def.ransacReprojThresh);
	ransacThresholdChanged(def.ransacReprojThresh);
}

void SettingsWindow::saveFrame()
{
	// Tell the displayWindow to save a frame
	QCoreApplication::postEvent(displayWindow, new QEvent(QEvent::Type(QEvent::User + 5)), Qt::LowEventPriority);
}

void SettingsWindow::toggleRecording()
{
	if (recording)
	{
		// Tell the displayWindow to stop recording
		QCoreApplication::postEvent(displayWindow, new QEvent(QEvent::Type(QEvent::User + 4)), Qt::LowEventPriority);

		recordButton->setText("Record");
	}
	else
	{
		// Tell the displayWindow to start recording
		QCoreApplication::postEvent(displayWindow, new QEvent(QEvent::Type(QEvent::User + 3)), Qt::LowEventPriority);

		recordButton->setText("Stop Recording");
	}

	recording = !recording;
}