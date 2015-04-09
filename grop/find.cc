//=======================================================================
// find.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "find.h"
#include "db.h"
#include "mainwindow.h"
#include <gtkmm/grid.h>
#include <gtkmm/button.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/filechooserdialog.h>

using namespace Grop;

Find* Find::s_find = 0;


Find::Find(MainWindow& parent)
:
	Gtk::Dialog("grop :: find file", parent, true),
	m_entry(),
	m_treeview(parent)
{
	set_border_width(8);
	set_default_size(300, 200);
	
	Gtk::Button* button_browse = Gtk::manage(new Gtk::Button("_Browse", true));
	button_browse->signal_clicked().connect(sigc::mem_fun(*this, &Find::browse));

	Gtk::Grid* grid = Gtk::manage(new Gtk::Grid());
	grid->set_column_spacing(get_border_width());
	grid->attach(m_entry, 0, 0, 1, 1);
	grid->attach(*button_browse, 1, 0, 1, 1);

	Gtk::ScrolledWindow* scrolled_window = Gtk::manage(new Gtk::ScrolledWindow());
	scrolled_window->add(m_treeview);

	Gtk::Box* box = get_content_area();
	box->set_spacing(get_border_width());
	box->pack_start(*grid, Gtk::PACK_SHRINK);
	box->pack_start(*scrolled_window, Gtk::PACK_EXPAND_WIDGET);

	add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
	add_button(Gtk::Stock::FIND, Gtk::RESPONSE_APPLY);

	set_default_response(Gtk::RESPONSE_APPLY);
	m_entry.set_activates_default();
	m_entry.set_hexpand();

	get_action_area()->set_layout(Gtk::BUTTONBOX_EDGE);

	show_all();
}


void Find::instance(MainWindow& parent)
{
	if (!s_find)
		s_find = new Find(parent);

	s_find->m_entry.set_text("");
	s_find->reset_treeview();
	s_find->run();
}


void Find::on_response(int id)
{
	if (id == Gtk::RESPONSE_APPLY)
		find();
	else
		hide();
}


Gtk::TreeModel::iterator Find::reset_treeview()
{
	m_treeview.m_model->clear();
	return m_treeview.m_model->append();
}


void Find::find()
{
	Gtk::TreeModel::iterator i = reset_treeview();
	Glib::ustring path(m_entry.get_text());

	if (path.empty())
		return;
	
	(*i)[m_treeview.m_columns.m_name] = "(file not found)";
	(*i)[m_treeview.m_columns.m_pkg] = 0;

	if (path[0] != '/')
		return;
	
	int cnt = 0;

	for (DB::const_iter p = DB::pkgs().begin(); p != DB::pkgs().end(); ++p) {
		if ((*p)->find_file(path)) {
			if (cnt++)
				i = m_treeview.m_model->append();
			(*i)[m_treeview.m_columns.m_name] = (*p)->name();
			(*i)[m_treeview.m_columns.m_pkg] = *p;
		}
	}
}


void Find::browse()
{
	Gtk::FileChooserDialog dialog(*this, "Select file");

	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	dialog.set_show_hidden();
	dialog.set_filename(m_entry.get_text());
	
	if (dialog.run() == Gtk::RESPONSE_OK) {
		m_entry.set_text(dialog.get_filename());
		m_entry.set_position(-1);
	}
}


//--------------------//
// Find::PkgsTreeView //
//--------------------//


Find::PkgsTreeView::PkgsTreeView(MainWindow& parent)
:
	m_columns(),
	m_model(Gtk::ListStore::create(m_columns)),
	m_selected_pkg(0),
	m_mainwindow(parent)
{
	set_model(m_model);
	set_headers_visible(false);
	append_column("", m_columns.m_name);

	get_selection()->signal_changed().connect(
		sigc::mem_fun(this, &Find::PkgsTreeView::on_selection_changed));
}


void Find::PkgsTreeView::on_selection_changed()
{
	Gtk::TreeModel::iterator i = get_selection()->get_selected();
	m_selected_pkg = i ? (*i)[m_columns.m_pkg] : static_cast<Pkg*>(0);
}


//
// Double clicking on a row makes the main treeview in the main window
// scroll to the selected package.
//
bool Find::PkgsTreeView::on_button_press_event(GdkEventButton* event)
{
	bool handled = Gtk::TreeView::on_button_press_event(event);

	if (m_selected_pkg && event->window == get_bin_window()->gobj()
	&& event->button == 1 && event->type == GDK_2BUTTON_PRESS)
		m_mainwindow.scroll_to_pkg(m_selected_pkg);
	
	return handled;
}


//
// Pressing <Return> makes the main treeview in the main window
// scroll to the selected package.
//
bool Find::PkgsTreeView::on_key_press_event(GdkEventKey* event)
{
	bool handled = Gtk::TreeView::on_key_press_event(event);
	
	if (m_selected_pkg && event->keyval == GDK_KEY_Return)
		m_mainwindow.scroll_to_pkg(m_selected_pkg);
	
	return handled;
}

