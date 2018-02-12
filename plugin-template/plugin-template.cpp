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
    "GUI label", "Tooltip description",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
  },
  {
    "A State", "Tooltip description", DefaultGUIModel::STATE,
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

void
PluginTemplate::initParameters(void)
{
  some_parameter = 0;
  some_state = 0;
}

void
PluginTemplate::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag) {
    case INIT:
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      setParameter("GUI label", some_parameter);
      setState("A State", some_state);
      break;

    case MODIFY:
      some_parameter = getParameter("GUI label").toDouble();
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
}

// functions designated as Qt slots are implemented as regular C++ functions
void
PluginTemplate::aBttn_event(void)
{
}

void
PluginTemplate::bBttn_event(void)
{
}
