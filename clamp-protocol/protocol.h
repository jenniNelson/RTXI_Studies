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

#ifndef CP_PROTOCOL_H
#define CP_PROTOCOL_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <qdom.h>

namespace ClampProtocolModule {

	class Protocol;
	class ProtocolStep;
	class ProtocolSegment;

	typedef boost::shared_ptr<ProtocolStep> Step;               // Step pointer
	typedef std::vector<Step> SegmentContainer;                 // Vector of steps: segment
	typedef std::vector<Step>::iterator SegmentContainerIt;     // Iterator for segment container
	typedef boost::shared_ptr<ProtocolSegment> Segment;         // Segment pointer
	typedef std::vector<Segment> ProtocolContainer;             // Vector of segments: protocol
	typedef std::vector<Segment>::iterator ProtocolContainerIt; // Iterator for protocol containter

	class ProtocolStep { // Individual step within a protocol
		public:
			ProtocolStep();
			~ProtocolStep() { };
			double retrieve( int );
		
			enum ampMode_t{VOLTAGE,CURRENT} ampMode;
			enum stepType_t{STEP,RAMP,TRAIN,CURVE} stepType;
			double stepDuration;
			double deltaStepDuration;
			double delayBefore;
			double delayAfter;
			double holdingLevel1;
			double deltaHoldingLevel1;
			double holdingLevel2;
			double deltaHoldingLevel2;
			double pulseWidth;
			int pulseRate;
	}; // class ProtocolStep

	class ProtocolSegment { // A segment within a protocol, made up of ProtocolSteps
		friend class Protocol;
		
		public:
			ProtocolSegment();
			~ProtocolSegment() { };
		
			SegmentContainer segmentContainer;
		
		private:
			int numSweeps;
	};

	class Protocol {
		friend class ClampProtocolEditor;
		
		public:
			Protocol();
			~Protocol() { };

			// These should be private
			Segment getSegment( int );                      // Return a segment
			int numSegments( void );                        // Number of segments in a protocol
			int numSweeps( int );                           // Number of sweeps in a segment
			int segmentLength( int, double, bool );         // Points in a segment (w/ or w/o sweeps), length / period
			void setSweeps( int, int );                     // Set sweeps for a segment
			Step getStep( int, int );                       // Return step in a segment
			int numSteps( int );                            // Return number of steps in segment
			void toDoc( void );                             // Convert protocol to QDomDocument
			void fromDoc( QDomDocument );                   // Load protocol from a QDomDocument
			void clear( void );                             // Clears container
			std::vector<std::vector<double>> run( double ); // Runs the protocol, returns a time and output vector
		
			ProtocolContainer protocolContainer;
			QDomDocument protocolDoc;
		
		private:
			int addSegment( int );      // Add a segment to container
			int deleteSegment( int );   // Delete a segment from container
			int addStep( int, int );    // Add a step to a segment in container
			int deleteStep( int, int ); // Delete a step from segment in container

			QDomElement segmentToNode( QDomDocument &, int );
			QDomElement stepToNode( QDomDocument &, int, int );
	}; // class Protocol

}
#endif // CP_protocol.h
