//=======================================================================
// main.cc
//-----------------------------------------------------------------------
// This file is part of the package grop
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "opt.h"
#include "db.h"
#include "mainwindow.h"
#include "util.h"
#include <gtkmm/main.h>
#include <fstream>

static void show_error(std::string msg, bool gtk_started);
static void create_pid_file();


int main(int argc, char* argv[])
{
	bool gtk_started = false;

	try
	{
		Grop::Opt::init();
		Gtk::Main kit(argc, argv, Grop::Opt::context());

		gtk_started = true;
		
		if (!geteuid())
			create_pid_file();
		
		Grop::DB::init();
		Grop::MainWindow window;
		
		kit.run(window);
	}

	catch (std::exception const& x)
	{
		show_error(x.what(), gtk_started);
	}
	
	catch (Glib::Exception const& x)
	{
		show_error(x.what(), gtk_started);
	}

	remove(PIDFILEDIR "/grop.pid");
}


void show_error(std::string msg, bool gtk_started)
{
	if (gtk_started)
		Grop::run_error_dialog(msg, 0);
	else
		std::cerr << "grop: " << msg << '\n';
}


//
// create temp file carrying PID for later retrieval
//
void create_pid_file()
{
/*
	std::string pidfile(PIDFILEDIR "/grop.pid");

	if (!access(pidfile.c_str(), W_OK))
		throw Porg::Error("Grop is already running");

	Porg::FileStream<std::ofstream> os(pidfile);
	
	if (!(os << getpid())) {
		throw Porg::Error(std::string("Error writing to PID file '") 
			+ pidfile + "'", errno);
	}
*/
}


