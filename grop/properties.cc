//=======================================================================
// properties.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "properties.h"
#include "filestreeview.h"
#include "infotextview.h"
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>


Grop::Properties::Properties(Grop::Pkg const& pkg, Gtk::Window& parent)
:
	Gtk::Dialog("grop :: properties", parent),
	m_notebook()
{
	set_border_width(4);
	set_default_size(500, 500);

	Gtk::Label* label = Gtk::manage(new Gtk::Label());
	Gtk::Box* label_box = Gtk::manage(new Gtk::Box());
	label->set_markup("Package <b>" + pkg.name() + "</b>");
	label_box->pack_start(*label, Gtk::PACK_SHRINK);

	Gtk::Box* box = get_content_area();
	box->set_spacing(4);
	box->pack_start(*label_box, Gtk::PACK_SHRINK);
	box->pack_start(m_notebook, Gtk::PACK_EXPAND_WIDGET);

	Gtk::ScrolledWindow* scrolled_window_files = Gtk::manage(new Gtk::ScrolledWindow());
	scrolled_window_files->add(*(Gtk::manage(new Grop::FilesTreeView(pkg))));
	m_notebook.append_page(*scrolled_window_files, "Files");

	Gtk::ScrolledWindow* scrolled_window_info = Gtk::manage(new Gtk::ScrolledWindow());
	scrolled_window_info->add(*(Gtk::manage(new Grop::InfoTextView(pkg))));
	m_notebook.append_page(*scrolled_window_info, "Info");

	add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);

	show_all();
	run();
}


void Grop::Properties::instance(Pkg const& pkg, Gtk::Window& parent)
{
	Grop::Properties properties(pkg, parent);
}

