/*
Copyright (C) 2011 Georgia Institute of Technology

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
* This module simply outputs the specified current for as long as the module
* is active. It is designed to be programmatically paused and unpaused from
* another module (running an experimental module, for example) to implement
* a holding current in between trials.
*/


#include "current-hold.h"

extern "C" Plugin::Object *createRTXIPlugin(void) {
	return new Ihold();
}

/// vars[] are labels for the GUI, of type variable_t defined in DefaultGUIModel.
static DefaultGUIModel::variable_t vars[] = {
	// {"NameShownInLabel", "HoverExplanation", DefaultGUIModel::TYPE_FLAG}
	// TYPE_FLAG can be STATE, PARAMETER, INPUT, OUTPUT, or COMMENT
	{ "Command", "Static current output (pA)", DefaultGUIModel::OUTPUT, },
	// If it's a PARAMETER you can specify what type (double, int, whatever) via | DOUBLE
	{ "Holding Current (pA)", "Holding Current (pA)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

Ihold::Ihold(void) : DefaultGUIModel("Holding Current", ::vars, ::num_vars) {
	DefaultGUIModel::createGUI(vars, num_vars);

	current = -100e-12;
	on = false;

	update( INIT );
	refresh();
	resizeMe();
}

Ihold::~Ihold(void) {}

/// Execute is the stuff that's executed in real-time--the important bits!
void Ihold::execute(void) {
	output(0) = current;
}

void Ihold::update(DefaultGUIModel::update_flags_t flag) {
	switch (flag) {
		case INIT:
			setParameter("Holding Current (pA)", QString::number(current * 1e12)); // convert from A to pA
			break;

		case MODIFY:
			current = getParameter("Holding Current (pA)").toDouble() * 1e-12; // convert from pA to A
			break;

		case PAUSE:
			output(0) = 0;

		case PERIOD:
			default:
			break;
	}
}
