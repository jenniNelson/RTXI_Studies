/*
 * Copyright (C) 2011 Weill Medical College of Cornell University
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "clamp-protocol-window.h"
#include "clamp-protocol.h"

#include <qwt_text.h>
#include <qwt_legend.h>
#include <basicplot.h>

using namespace ClampProtocolModule;

ClampProtocolWindow::ClampProtocolWindow( QWidget *parent ) : QWidget( MainWindow::getInstance()->centralWidget() ) {
//	setWindowTitle("Protocol Viewer");

	overlaySweeps = false;
	plotAfter = false;
	colorScheme = 0;
	runCounter = 0;
	sweepsShown = 0;

	createGUI();
}

void ClampProtocolWindow::closeEvent( QCloseEvent *event ) {
	emit emitCloseSignal();
}

ClampProtocolWindow::~ClampProtocolWindow( void ) {
//	panel->removeClampProtocolWindow( this );
}

void ClampProtocolWindow::createGUI( void ) {
	
	subWindow = new QMdiSubWindow;
	subWindow->setWindowIcon(QIcon("/usr/local/lib/rtxi/RTXI-widget-icon.png"));
	subWindow->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint |
	                          Qt::WindowMinimizeButtonHint);
	subWindow->setAttribute(Qt::WA_DeleteOnClose);
	MainWindow::getInstance()->createMdi(subWindow);

	QVBoxLayout *plotWindowUILayout = new QVBoxLayout( this );
	frame = new QFrame;
	frameLayout = new QHBoxLayout;
	plotWindowUILayout->addLayout(frameLayout);

// Make the top of the GUI
	layout1 = new QGridLayout;
	currentScaleLabel = new QLabel("Current");
	currentScaleEdit = new QComboBox;
	currentScaleEdit->addItem( trUtf8( "\xce\xbc\x41" ) );
	currentScaleEdit->addItem( tr( "nA" ) );
	currentScaleEdit->addItem( tr( "pA" ) );
	currentScaleEdit->setCurrentIndex( 1 );
	currentY1Edit = new QSpinBox;
	currentY1Edit->setMaximum( 99999 );
	currentY1Edit->setMinimum( -99999 );
	currentY1Edit->setValue(-20);
	currentY2Edit = new QSpinBox;
	currentY2Edit->setMaximum( 99999 );
	currentY2Edit->setMinimum( -99999 );
	currentY2Edit->setValue(0);
	layout1->addWidget(currentScaleLabel, 1, 0, 1, 1);
	layout1->addWidget(currentY1Edit, 1, 1, 1, 1);
	layout1->addWidget(currentY2Edit, 1, 2, 1, 1);
	layout1->addWidget(currentScaleEdit, 1, 3, 1, 1);

	timeScaleLabel = new QLabel("Time");
	timeScaleEdit = new QComboBox;
	timeScaleEdit->addItem( tr( "s" ) );
	timeScaleEdit->addItem( tr( "ms" ) );
	timeScaleEdit->addItem( trUtf8( "\xce\xbc\x73" ) );
	timeScaleEdit->addItem( tr( "ns" ) );
	timeScaleEdit->setCurrentIndex( 1 );
	timeX1Edit = new QSpinBox;
	timeX1Edit->setMaximum( 99999 );
	timeX1Edit->setValue(0);
	timeX2Edit = new QSpinBox;
	timeX2Edit->setMaximum( 99999 );
	timeX2Edit->setValue(1000);
	layout1->addWidget(timeScaleLabel, 0, 0, 1, 1);
	layout1->addWidget(timeX1Edit, 0, 1, 1, 1);
	layout1->addWidget(timeX2Edit, 0, 2, 1, 1);
	layout1->addWidget(timeScaleEdit, 0, 3, 1, 1);

	frameLayout->addLayout(layout1);

	setAxesButton = new QPushButton("Set Axes");
	setAxesButton->setEnabled(true);
	frameLayout->addWidget(setAxesButton);

	layout2 = new QVBoxLayout;
	overlaySweepsCheckBox = new QCheckBox("Overlay Sweeps");
	layout2->addWidget(overlaySweepsCheckBox);
	plotAfterCheckBox = new QCheckBox("Plot after Protocol");
	layout2->addWidget(plotAfterCheckBox);
	frameLayout->addLayout(layout2);

	layout3 = new QVBoxLayout;
	textLabel1 = new QLabel("Color by:");
	colorByComboBox = new QComboBox;
	colorByComboBox->addItem( tr( "Run" ) );
	colorByComboBox->addItem( tr( "Trial" ) );
	colorByComboBox->addItem( tr( "Sweep" ) );
	layout3->addWidget(textLabel1);
	layout3->addWidget(colorByComboBox);
	frameLayout->addLayout(layout3);

	clearButton = new QPushButton("Clear");
	frameLayout->addWidget(clearButton);

// And now the plot on the bottom...
	plot = new BasicPlot( this );

	// Add scrollview for top part of widget to allow for smaller widths
	plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	plotWindowUILayout->addWidget( plot );

	resize( 625, 400 ); // Default size

	// Plot settings
	QwtText xAxisTitle, yAxisTitle;
	xAxisTitle.setText( "Time (ms)" );
	xAxisTitle.setFont( font );
	yAxisTitle.setText( "Current (nA)" );
	yAxisTitle.setFont( font );
	plot->setAxisTitle( QwtPlot::xBottom, xAxisTitle );
	plot->setAxisTitle( QwtPlot::yLeft, yAxisTitle );
	setAxes(); // Set axes to defaults (1,1000)(-20,20)

	QwtLegend *legend = new QwtLegend();
	plot->insertLegend( legend, QwtPlot::RightLegend );

	// Signal/Slot Connections
	QObject::connect( setAxesButton, SIGNAL(clicked(void)), this, SLOT(setAxes(void)) );
	QObject::connect( timeX1Edit, SIGNAL(valueChanged(int)), this, SLOT(setAxes(void)) );
	QObject::connect( timeX2Edit, SIGNAL(valueChanged(int)), this, SLOT(setAxes(void)) );
	QObject::connect( currentY1Edit, SIGNAL(valueChanged(int)), this, SLOT(setAxes(void)) );
	QObject::connect( currentY2Edit, SIGNAL(valueChanged(int)), this, SLOT(setAxes(void)) );
	QObject::connect( clearButton, SIGNAL(clicked(void)), this, SLOT(clearPlot(void)) );
	QObject::connect( overlaySweepsCheckBox, SIGNAL(clicked(void)), this, SLOT(toggleOverlay(void)) );
	QObject::connect( plotAfterCheckBox, SIGNAL(clicked(void)), this, SLOT(togglePlotAfter(void)) );
	QObject::connect( colorByComboBox, SIGNAL(activated(int)), this, SLOT(changeColorScheme(int)) );

	// Add tooltip to color scheme combo box
	QString tooltip =
		QString( "There are 10 colors which rotate in the same order\n" ) +
		QString( "Run: Change color after every protocol run\n" ) +
		QString( "Trial: For use when running multiple trials - A color will correspond to a specific trial number\n" ) +
		QString( "Sweep: A color will correspond to a specific sweep" );
	colorByComboBox->setToolTip(tooltip); //QToolTip::add( colorByComboBox, tooltip );

	subWindow->setWidget(this);
	show();
	subWindow->adjustSize();
}

void ClampProtocolWindow::addCurve( double *output, curve_token_t token ) { // Attach curve to plot
	double time[ token.points ];

	if( overlaySweeps ) 
		for( size_t i = 0; i < token.points; i++ )
			time[ i ] = token.period * ( token.stepStartSweep + i );   
	else 
		for( size_t i = 0; i < token.points; i++ ) 
			time[ i ] = token.period * ( token.stepStart + i );                          

	if( token.stepStart == -1 ) // stepStart is offset by -1 in order to connect curves, but since i is unsigned, must be careful of going negative  
		time[ 0 ] = 0;

	int idx;
	QString curveTitle;

	bool legendShow = token.lastStep; // Whether legend entry will be added

	switch( colorScheme ) {
		case 0: // Color by Run
			idx = runCounter % 10;
			curveTitle = "Run " + QString::number( runCounter + 1 );

			if( token.lastStep ) // Increase run counter if curve is last step in a run
			runCounter++;              
			break;

		case 1: // Color by Trial
			idx = token.trial;
			curveTitle = "Trial " + QString::number( idx + 1 );
			break;

		case 2: // Color by sweep
			idx = token.sweep;

			if( idx >= sweepsShown ) {
				legendShow = true;
				sweepsShown++;
			}
			else
				legendShow = false;

			curveTitle = "Sweep " + QString::number( idx + 1 );
			break;

		default:
			break;
	}	 

	curveContainer.push_back( QwtPlotCurvePtr( new QwtPlotCurve(curveTitle) ) );
	QwtPlotCurvePtr curve = curveContainer.back();
	curve->setSamples( time, output, token.points ); // Makes a hard copy of both time and output
	colorCurve( curve, idx );
	curve->setItemAttribute( QwtPlotItem::Legend, legendShow ); // Set whether curve will appear on legend
	curve->attach( plot );

	if( legendShow ) {
//		qobject_cast<QwtLegend*>(plot->legend())->legendWidgets().back()->setFont( font ); // Adjust font
	}

	if( plotAfter && !token.lastStep ) // Return before replot if plotAfter is on and its not last step of protocol
		return ;

	plot->replot(); // Attaching curve does not refresh plot, must replot
}

void ClampProtocolWindow::colorCurve( QwtPlotCurvePtr curve, int idx ) {
	QColor color;

	switch( idx ) {
		case 0: color = QColor( Qt::black ); break;
		case 1: color = QColor( Qt::red ); break;
		case 2: color = QColor( Qt::blue ); break;
		case 3: color = QColor( Qt::green ); break;
		case 4: color = QColor( Qt::cyan ); break;
		case 5: color = QColor( Qt::magenta ); break;
		case 6: color = QColor( Qt::yellow ); break;
		case 7: color = QColor( Qt::lightGray ); break;
		case 8: color = QColor( Qt::darkRed ); break;
		case 9: color = QColor( Qt::darkGreen ); break;
		default: color = QColor( Qt::black ); break;
	}

	QPen pen( color, 2 ); // Set color and width
	curve->setPen( pen );
}

void ClampProtocolWindow::setAxes( void ) {    
	double timeFactor, currentFactor;

	switch( timeScaleEdit->currentIndex() ) { // Determine time scaling factor, convert to ms
		case 0: timeFactor = 10; // (s)
			break;
		case 1: timeFactor = 1; // (ms) default
			break;
		case 2: timeFactor = 0.1; // (us)
			break;
		default: timeFactor = 1; // should never be called
			break;
	}

	switch( currentScaleEdit->currentIndex() ) { // Determine current scaling factor, convert to nA
		case 0: currentFactor = 10; // (uA)
			break;
		case 1: currentFactor = 1; // (nA) default
			break;
		case 2: currentFactor = 0.1; // (pA)
			break;
		default: currentFactor = 1; // shoudl never be called
			break;
	}

	// Retrieve desired scale
	double x1, x2, y1, y2;

	x1 = timeX1Edit->value() * timeFactor;
	x2 = timeX2Edit->value() * timeFactor;
	y1 = currentY1Edit->value() * currentFactor;
	y2 = currentY2Edit->value() * currentFactor;

	plot->setAxes( x1, x2, y1, y2 );
}

void ClampProtocolWindow::clearPlot( void ) {
	curveContainer.clear();
	plot->replot();
}

void ClampProtocolWindow::toggleOverlay( void ) {    
	if( overlaySweepsCheckBox->isChecked() ) { // Checked
	// Check if curves are plotted, if true check if user wants plot cleared in
	// order to overlay sweeps during next run
		overlaySweeps = true;
	}
	else { // Unchecked
		overlaySweeps = false;
	}
}

void ClampProtocolWindow::togglePlotAfter( void ) {
	if( plotAfterCheckBox->isChecked() ) // Checked
		plotAfter = true;    
	else  // Unchecked
		plotAfter = false;

	plot->replot(); // Replot since curve container is cleared    
}

void ClampProtocolWindow::changeColorScheme( int choice ) {
	if( choice == colorScheme ) // If choice is the same
		return ;    

	// Check if curves are plotted, if true check if user wants plot cleared in
	// order to change color scheme
	if ( !curveContainer.empty() && QMessageBox::warning(
					this,
					"Warning",
					"Switching the color scheme will clear the plot.\nDo you wish to continue?",
					QMessageBox::Yes | QMessageBox::Default, QMessageBox::No
					| QMessageBox::Escape) != QMessageBox::Yes) {

		colorByComboBox->setCurrentIndex( colorScheme ); // Revert to old choice if answer is no
		return ;
	}

	colorScheme = choice;
	curveContainer.clear();
	plot->replot(); // Replot since curve container is cleared
}

void  ClampProtocolWindow::doDeferred( const Settings::Object::State &s ) { }

void  ClampProtocolWindow::doLoad( const Settings::Object::State &s ) {
	if ( s.loadInteger("Maximized") )
		showMaximized();
	else if ( s.loadInteger("Minimized") )
		showMinimized();

// Window Position
	if ( s.loadInteger( "W" ) != NULL ) {
		resize( s.loadInteger("W"), s.loadInteger("H") );
		parentWidget()->move( s.loadInteger("X"), s.loadInteger("Y") );
	}

	// Load Parameters
	timeX1Edit->setValue( s.loadInteger("X1") );
	timeX2Edit->setValue( s.loadInteger("X2") );
	timeScaleEdit->setCurrentIndex( s.loadInteger("Time Scale") );
	currentY1Edit->setValue( s.loadInteger("Y1") );
	currentY2Edit->setValue( s.loadInteger("Y2") );
	currentScaleEdit->setCurrentIndex( s.loadInteger("Current Scale") );
	overlaySweepsCheckBox->setChecked( s.loadInteger("Overlay Sweeps") );
	plotAfterCheckBox->setChecked( s.loadInteger("Plot After") );
	colorByComboBox->setCurrentIndex( s.loadInteger("Color Scheme") );
	changeColorScheme( s.loadInteger("Color Scheme") );
	setAxes();
	toggleOverlay();
	togglePlotAfter();
}

void  ClampProtocolWindow::doSave( Settings::Object::State &s ) const {
	if ( isMaximized() )
		s.saveInteger( "Maximized", 1 );
	else if ( isMinimized() )
		s.saveInteger( "Minimized", 1 );

	// Window Position
	QPoint pos = parentWidget()->pos();
	s.saveInteger( "X", pos.x() );
	s.saveInteger( "Y", pos.y() );
	s.saveInteger( "W", width() );
	s.saveInteger( "H", height() );

	// Save parameters
	s.saveInteger( "X1", timeX1Edit->value() );
	s.saveInteger( "X2", timeX2Edit->value() );
	s.saveInteger( "Time Scale", timeScaleEdit->currentIndex() );
	s.saveInteger( "Y1", currentY1Edit->value() );
	s.saveInteger( "Y2", currentY2Edit->value() );
	s.saveInteger( "Current Scale", currentScaleEdit->currentIndex() );
	s.saveInteger( "Overlay Sweeps", overlaySweepsCheckBox->isChecked() );
	s.saveInteger( "Plot After", plotAfterCheckBox->isChecked() );
	s.saveInteger( "Color Scheme", colorByComboBox->currentIndex() );
}

