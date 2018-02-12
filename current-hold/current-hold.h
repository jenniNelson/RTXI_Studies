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
* This module outputs a constant current. It is intended to be
* used to specify a holding current while other (experimental) modules
* are inactive and can be called programmatically or triggered by
* a signal. A value of 1 on the input signal means the holding current
* is on.
*
*/

#include <default_gui_model.h>
#include <string>

class Ihold : public DefaultGUIModel {
	public:
		Ihold(void);
		virtual ~Ihold(void);
		virtual void execute(void);
	
	protected:
		virtual void update(DefaultGUIModel::update_flags_t);
	
	private:
		double current;
		bool on;
};
