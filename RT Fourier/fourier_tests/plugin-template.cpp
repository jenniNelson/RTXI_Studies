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
  {
    "Voltage In",
    "Realtime input voltage level",
     DefaultGUIModel::STATE,
  },
  {
    "from (hz)", "Lower end of frequency band",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
  },
  {
    "to (hz)", "Higher end of frequency band",
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
}

/// The real-time loop. Output and needed calculations only, NOT FOR GUI jazz.
void
PluginTemplate::execute(void)
{
  return;
}

// Update the values in our fourier transform based on the newest data.
void PluginTemplate::update_fourier(double new_data)
{
  // get input data
  new_data = input(0);

  // get the oldest piece of data we have
  replaced = data_history[data_idx];

  // for every frequency we're sampling, update sum according to it:
  for (int i = 0; i < num_frequencies; i++) {
    // subtract the old data from that frequency's sum
    // (according to its significance for the frequency band)
    total_sum -= replaced * significance(frequencies[i], i, offset_or_not);

    // add the new data, again according to its significance
    total_sum += new_data * significance(frequencies[i], i, offset_or_not);

  }

  // Output the average power level over all the samples
  out_data = (total_sum/num_frequencies)/data_history_size;
  output(0) = out_data;

  // replace old data with new
  data_history[data_idx] = new_data;

  // increment data pointer
  data_idx++;
  // wrap pointer if needed (and notify frequency bands they should wrap)
  if(data_idx > data_history_size){
    data_idx = data_idx % data_history_size;
    offset_or_not = (offset_or_not + 1) % 2;
  }
}

double PluginTemplate::significance(double frequency, int spot_in_history, bool offset_or_not){


  return 1.0;
}

void
PluginTemplate::initParameters(double buffer_length, double from,
                               double to,            int samples)
{
  data_history_size = static_cast<int>(buffer_length / period);
  data_history = new double[data_history_size];

  num_frequencies = samples;
  double bandwidth = to - from;
  double gap = bandwidth / (samples + 1);

  // initialize array of all our frequency samples
  frequencies = new double[num_frequencies];
  for (int i = 0; i < samples; i++) {
    // Sample the middles of frequency range (i.e [_._._._] where . is a sample.)
    frequencies[i] = gap*i + gap/2 + from;
  }

}

void
PluginTemplate::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag) {
    case INIT:
      double buffer_length, from, to;
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      setState("Output Channel", out_data);
      setState("Voltage In", new_data);
      setParameter("Buffer length", buffer_length); // ms
      setParameter("from (hz)", from);
      setParameter("to (hz)", to);
      setParameter("# Samples in frequency band", num_frequencies);
      initParameters(buffer_length, from, to, num_frequencies);
      break;

    case MODIFY:

      break;

    case UNPAUSE:
      break;

    case PAUSE:
      break;

    case PERIOD:
      // We really don't know what to do here
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
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
