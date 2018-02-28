/*
 * Copyright (C) 2011 Georgia Institute of Technology, University of Utah,
 * Weill Cornell Medical College
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This is a template header file for a user modules derived from
 * DefaultGUIModel with a custom GUI.
 */

#include <default_gui_model.h>

class PluginTemplate : public DefaultGUIModel
{

  Q_OBJECT

public:
  PluginTemplate(void);
  virtual ~PluginTemplate(void);

  void execute(void);
  void createGUI(DefaultGUIModel::variable_t*, int);
  void customizeGUI(void);

protected:
  virtual void update(DefaultGUIModel::update_flags_t);

private:
  double out_data;
  double period;

  // History of received recordings.
  double* data_history;
  // Size determined by user and RT period.
  int data_history_size;
  // Points to oldest data in data_history
  int data_idx;
  // The newest chunk of data (Used in RT method)
  double new_data;
  // Out-of-date data (Used in RT method)
  double replaced;

  // For frequency bands not partitioning data_history neatly
  int offset_or_not; // 0 = don't, 1 = offset by data_size & FB.size

  struct frequency{
    double frequency_value;     // hz - base of frequency
    //double band;          // band size (hz)
    double sum;
    frequency(double _frequency=0.0):frequency_value(_frequency),sum(0.0)
    {
    }
    double significance(int index, int wrap_or_not){
      //int offset = data_size % frequency;
      return 1.0; //return sin(magic);
    }
  };

  frequency* frequencies;
  int num_frequencies;
  double total_sum;

  void initParameters(double buffer_length, double from,
                      double to,            int samples);

  void update_fourier(double new_data);

private slots:

};
