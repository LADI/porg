//=======================================================================
// db.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef PORG_DB_H
#define PORG_DB_H

#include "config.h"
#include "porg/common.h"	// sort_t
#include <vector>


namespace Porg {

class Pkg;

class DB : public std::vector<Pkg*>
{
	public:

	DB();
	~DB();

	void get_pkgs(std::vector<std::string> const& args);
	void get_all_pkgs();

	void list();
	void list_files();
	void print_conf_opts() const;
	void query();
	void remove();
	void print_info() const;

	protected:

	void get_pkg_list_widths(int&, int&);
	int get_file_size_width();
	bool add_pkg(std::string const& name);
	void del_pkg(std::string const& name);

	class Sorter
	{
		public:

		Sorter(sort_t const& = SORT_BY_NAME);
		bool operator()(Pkg* left, Pkg* right) const;

		private:

		bool (Sorter::*m_sort_func)(Pkg*, Pkg*) const;
		bool sort_by_name(Pkg* left, Pkg* right) const;
		bool sort_by_size(Pkg* left, Pkg* right) const;
		bool sort_by_nfiles(Pkg* left, Pkg* right) const;
		bool sort_by_date(Pkg* left, Pkg* right) const;

	};	// class DB::Sorter

	float m_total_size;
	ulong m_total_files;

};		// class DB

}		// namespace Porg

#endif  // PORG_DB_H
