//=======================================================================
// util.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "util.h"
#include <gtkmm/messagedialog.h>
#include <gtkmm/main.h>

using std::string;
using namespace Grop;


void Grop::main_iter()
{
	while (Gtk::Main::events_pending())
		Gtk::Main::iteration();
}


bool Grop::run_question_dialog(string const& msg, Gtk::Window* parent)
{
	Gtk::MessageDialog dialog(msg, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true);
	if (parent)
		dialog.set_transient_for(*parent);
	dialog.set_title("grop :: question");
	return dialog.run() == Gtk::RESPONSE_YES;
}


void Grop::run_error_dialog(string const& msg, Gtk::Window* parent)
{
	Gtk::MessageDialog dialog(msg, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
	if (parent)
		dialog.set_transient_for(*parent);
	dialog.set_title("grop :: error");
	dialog.run();
}

