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

#ifndef CP_PROTOCOL_EDITOR_H
#define CP_PROTOCOL_EDITOR_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#else
	#include <QtGui>
#endif

#include "protocol.h"


namespace ClampProtocolModule {

	class ClampProtocolEditor : public QWidget {
		Q_OBJECT

		public:
			ClampProtocolEditor( QWidget * );
			~ClampProtocolEditor( void ) { };
			Protocol protocol; // Clamp protocol
			void createGUI(void);
		
		private:
			QPushButton *saveProtocolButton, *loadProtocolButton, 
			            *exportProtocolButton, *previewProtocolButton, 
			            *clearProtocolButton;
			QGroupBox *protocolDescriptionBox;
			QLabel *segmentStepLabel;
			QTableWidget *protocolTable;
			QPushButton *addStepButton, *insertStepButton, *deleteStepButton;
			QGroupBox *segmentSummaryGroup, *segmentSweepGroup;
			QLabel *segmentSweepLabel;
			QSpinBox *segmentSweepSpinBox;
			QListWidget *segmentListWidget;
			QPushButton *addSegmentButton, *deleteSegmentButton;

			QMdiSubWindow *subWindow;

			int currentSegmentNumber;
			QStringList ampModeList, stepTypeList;
			void createStep( int );
			int loadFileToProtocol( QString );
			bool protocolEmpty( void );
			void closeEvent( QCloseEvent * );

		protected:
			QHBoxLayout *layout1, *layout4, *segmentSweepGroupLayout;
			QVBoxLayout *windowLayout, *layout3, *protocolDescriptionBoxLayout, 
			            *layout5, *segmentSummaryGroupLayout, *layout6;
			QGridLayout *layout2;
				
		signals:
			void protocolTableScroll( void );
			void emitCloseSignal( void );
		
		public slots:
			QString loadProtocol( void );
			void loadProtocol( QString );
			void clearProtocol( void );
			void exportProtocol( void );
			void previewProtocol( void );
			void comboBoxChanged( QString );
			virtual void protocolTable_currentChanged(int, int);
			virtual void protocolTable_verticalSliderReleased();
		
		private slots:
			void addSegment( void );
			void deleteSegment( void );
			void addStep( void );
			void insertStep( void );
			void deleteStep( void );
			void updateSegment( QListWidgetItem* );
			void updateSegmentSweeps( int );
			void updateTableLabel( void );
			void updateTable( void );
			void updateStepAttribute( int, int );
			void updateStepType( int, ProtocolStep::stepType_t );
			void saveProtocol( void );
	};

}
#endif // CP_protocol_editor.h
