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
 * This is a template implementation file for a user module derived from
 * DefaultGUIModel with a custom GUI.
 */

#include "plugin-template.h"
#include <iostream>
#include <main_window.h>
#include <cmath>
#include "frequency.h"

extern "C" Plugin::Object*
createRTXIPlugin(void)
{
  return new PluginTemplate();
}

/// The GUI labels and pieces.
static DefaultGUIModel::variable_t vars[] = {
  // { ElementName, Hover description, TypeOfElement | (specified to be:) parameterType},
  {
    "Frequency band level",
    "Outputs levels of the specified frequency band",
     DefaultGUIModel::OUTPUT,
  },
  {
    "Frequency band level",
    "Outputs levels of the specified frequency band",
     DefaultGUIModel::STATE,
  },
  {
    "Voltage In",
    "Realtime input voltage level",
     DefaultGUIModel::INPUT,
  },
  /*{
    "Voltage In",
    "Realtime input voltage level",
     DefaultGUIModel::STATE,
  },*/
  {
    "from (Hz)", "Lower end of frequency band",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
  },
  {
    "to (Hz)", "Higher end of frequency band",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
  },
  {
    "# Samples in frequency band", "How many samples within the frequency band will be measure for power.",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER,
  },
  {
    "Buffer length", "How far back to measure frequency band level with (ms)",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
  },
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

PluginTemplate::PluginTemplate(void)
  : DefaultGUIModel("PluginTemplate with Custom GUI", ::vars, ::num_vars)
{
  setWhatsThis("<p><b>PluginTemplate:</b><br>QWhatsThis description.</p>");
  DefaultGUIModel::createGUI(vars,
                             num_vars); // this is required to create the GUI
  customizeGUI();
  // Update is how you insert code while running. INIT is a case for setting vars
  update(INIT); // this is optional, you may place initialization code directly
                // into the constructor
  refresh();    // this is required to update the GUI with parameter and state
                // values
  // Resize GUI in case it's customized
  QTimer::singleShot(0, this, SLOT(resizeMe()));
}

PluginTemplate::~PluginTemplate(void)
{
  //delete[] frequencies;
  //delete[] data_history;
}

/// The real-time loop. Output and needed calculations only, NOT FOR GUI jazz.
void
PluginTemplate::execute(void)
{
  update_fourier();
  return;
}

// Update the values in our fourier transform based on the newest data.
void PluginTemplate::update_fourier()
{
  // get input data
  new_data = input(0);

  // get the oldest piece of data we have
  replaced = data_history[data_idx];

  total_power = 0;
//
  frequency* freq;
  // for every frequency we're sampling, update sums according to it:
  for (int i = 0; i < num_frequencies; i++) {

    frequency* freq = frequencies[i];

    double oldest = data_history[ freq->oldest_idx ];

    freq->real_sum -= oldest * freq->real_significance();
    freq->imaginary_sum -= oldest * freq->imaginary_significance();
    //
    freq->real_sum += new_data * freq->real_significance();
    freq->imaginary_sum += new_data * freq->imaginary_significance();

    freq->increment_one_timestep();

    total_power += std::sqrt(std::pow(freq->real_sum, 2) + std::pow(freq->real_sum, 2));

  }

  // Output the average power level over all the samples
  out_data = total_power / 10000000;

  output(0) = out_data;

  // replace old data with new
  data_history[data_idx] = new_data;

  // increment data pointer and wrap pointer if needed
  data_idx = (data_idx + 1) % data_history_size;

}

void
PluginTemplate::initParameters(double buffer_length, double from,
                               double to,            int samples)
{
  data_history_size = static_cast<int>(buffer_length / period); //
  data_history = new double[data_history_size];

  // Clear out potential junk
  for(int i = 0; i < data_history_size; i++){
    data_history[i] = 0;
  }
  //Reset anything that could go wrong.
  data_idx = 0;
  replaced = 0;

  num_frequencies = samples;
  double bandwidth = to - from;
  double gap = bandwidth / (samples + 1);

  // initialize array of all our frequency samples
  frequencies = new frequency*[num_frequencies];

  for (int i = 0; i < num_frequencies; i++) {
    // Sample the middles of frequency range
    // (i.e [_._._._] where . is a sample.)
    frequencies[i] = new frequency(gap*i + gap/2 + from, period, data_history_size);
  }

}

void
PluginTemplate::update(DefaultGUIModel::update_flags_t flag)
{
  double buffer_length, from, to;
  switch (flag) {
    case INIT:
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      out_data = 0.0; new_data = 0.0;
      buffer_length = 100.0; from = 13.0; to = 30.0;
      num_frequencies = 4;
      setState("Output Channel", out_data);
      setState("Voltage In", new_data);
      setParameter("Buffer length", buffer_length); // ms
      setParameter("from (Hz)", from);
      setParameter("to (Hz)", to);
      setParameter("# Samples in frequency band", num_frequencies);

      // If buffer length too small, increase it:
      if(buffer_length < 1000/from ){
        buffer_length = 1000/from + 1;
      }

      initParameters(buffer_length, from, to, num_frequencies);
      break;

    case MODIFY:


      buffer_length = getParameter("Buffer length").toDouble(); // ms
      from = getParameter("from (Hz)").toDouble();
      to = getParameter("to (Hz)").toDouble();
      num_frequencies = getParameter("# Samples in frequency band").toInt();
      // Deallocate memory allocated by INIT to reallocate
      //delete frequencies;
      //delete[] data_history;

      // If buffer length too small, increase it:
      if(buffer_length < 1/(from/1000)){
        buffer_length = 1/(from/1000) + 1;
      }

      initParameters(buffer_length, from, to, num_frequencies);

      break;

    case UNPAUSE:
      break;

    case PAUSE:
      break;

    case PERIOD:
      // We really don't know what to do here
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      break;

    case EXIT:
      delete[] frequencies; // Deallocate memory
      delete[] data_history;
      break;

    default:
      break;
  }
}

void
PluginTemplate::customizeGUI(void)
{
  /*
  QGridLayout* customlayout = DefaultGUIModel::getLayout();

  QGroupBox* button_group = new QGroupBox;

  QPushButton* abutton = new QPushButton("Button A");
  QPushButton* bbutton = new QPushButton("Button B");
  QHBoxLayout* button_layout = new QHBoxLayout;
  button_group->setLayout(button_layout);
  button_layout->addWidget(abutton);
  button_layout->addWidget(bbutton);
  QObject::connect(abutton, SIGNAL(clicked()), this, SLOT(aBttn_event()));
  QObject::connect(bbutton, SIGNAL(clicked()), this, SLOT(bBttn_event()));

  customlayout->addWidget(button_group, 0, 0);
  setLayout(customlayout);
  */
}
