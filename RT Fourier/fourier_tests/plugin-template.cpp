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
    "Voltage In",
    "Realtime input voltage level",
     DefaultGUIModel::INPUT,
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
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
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
  initParameters();
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

  // get the oldest piece of data we have

  // for every frequency we're checking:
    // subtract the old data from that frequency's sum
    // (according to its significance for the frequency band)

    // add the new data, again according to its significance

    // Possibly updating output? Look into how other modules do that.

  // replace old data with new
  // increment our data pointer
  // wrap pointer if needed (and notify frequency bands they should wrap)



}

void
PluginTemplate::initParameters(void)
{
  
}

void
PluginTemplate::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag) {
    case INIT:
      double bufferLength, from, to;
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      setState("Output Channel", output);
      setParameter("Buffer length", bufferLength);
      setParameter("from (hz)", from);
      setParameter("to (hz)", to);
      break;

    case MODIFY:
      
      break;

    case UNPAUSE:
      break;

    case PAUSE:
      break;

    case PERIOD:
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
