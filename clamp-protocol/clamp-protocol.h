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

#pragma once

#include <fifo.h>
#include <default_gui_model.h>
#include "clamp-protocol-editor.h"
#include "clamp-protocol-window.h"
#include "protocol.h"

namespace ClampProtocolModule {

	class ClampProtocol : public DefaultGUIModel {

		Q_OBJECT

		public:
			ClampProtocol(void);
			~ClampProtocol(void);

			void initParameters(void);
			void customizeGUI(void);
			void execute(void);
			void refresh(void);
			
			void foreignToggleProtocol(bool);

			void receiveEvent( const ::Event::Object *);
			void receiveEventRT( const ::Event::Object *);

		protected:
			virtual void update(DefaultGUIModel::update_flags_t);

		private:
			void doLoad(const Settings::Object::State &);
			void doSave(Settings::Object::State &) const;

			std::list< ClampProtocolWindow * > plotWindowList;

	//		QString fileName;
			double period;
			double voltage, junctionPotential;
			double trial, time, sweep, segmentNumber, intervalTime;
			int numTrials;

			Protocol protocol;
			enum executeMode_t { IDLE, PROTOCOL } executeMode;
			enum protocolMode_t { SEGMENT, STEP, EXECUTE, END, WAIT } protocolMode;
			Step step;
			ProtocolStep::stepType_t stepType;
			int segmentIdx;
			int sweepIdx;
			int stepIdx;
			int trialIdx;
			int numSweeps;
			int numSteps;
			int stepTime, stepEndTime;
			double protocolEndTime;
			double stepOutput;
			double outputFactor, inputFactor;
			double rampIncrement;
			double pulseWidth;
			int pulseRate;
			Fifo fifo;
			std::vector<double> data;
			
			double prevSegmentEnd; // Time segment ends after its first sweep
			int stepStart; //Time when step starts divided by period
			curve_token_t token;

			bool recordData;
			bool protocolOn;
			bool recording;
			bool plotting;
			QTimer *plotTimer;

			QCheckBox *recordCheckBox;
			QLineEdit *loadFilePath;
			QPushButton *loadButton, *editorButton, *viewerButton, *runProtocolButton;
			ClampProtocolWindow *plotWindow;
			ClampProtocolEditor *protocolEditor;

			friend class ToggleProtocolEvent;
			class ToggleProtocolEvent : public RT::Event {
				public: 
					ToggleProtocolEvent(ClampProtocol*, bool, bool);
					~ToggleProtocolEvent(void);
					int callback(void);
				private:
					ClampProtocol *parent;
					bool protocolOn;
					bool recordData;
			};
		
		signals:
			void plotCurve( double *, curve_token_t );

		public slots:
			void loadProtocolFile(void);
			void openProtocolEditor(void);
			void openProtocolWindow(void);
			void updateProtocolWindow(void);
			void closeProtocolWindow(void);
			void closeProtocolEditor(void);
			void toggleProtocol(void);
	};

}
