//=======================================================================
// basepkg.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "basepkg.h"
#include "baseopt.h"
#include "file.h"
#include "rexp.h"
#include <fstream>
#include <algorithm>
#include <sstream>

using std::string;
using namespace Porg;

template<typename T> static T str2num(string const&);


BasePkg::BasePkg(string const& name_)
:
	m_files(),
	m_inodes(),
	m_name(name_),
	m_log(BaseOpt::logdir() + "/" + name_),
	m_base_name(get_base(name_)),
	m_version(get_version(name_)),
	m_date(time(0)),
	m_size(0),
	m_nfiles(0),
	m_nfiles_miss(0),
	m_icon_path(),
	m_url(),
	m_license(),
	m_summary(),
	m_description(),
	m_conf_opts(),
	m_author(),
	m_sorted_by_name(false)
{ }


void BasePkg::read_log()
{
	// read '#!porg' header or die
	
	FileStream<std::ifstream> f(m_log);
	string buf;
	if (!(getline(f, buf) && buf.find("#!porg") == 0))
		throw Error(m_log + ": '#!porg' header missing");

	int db_version = str2num<int>(buf.substr(7, 1));

	//
	// Read info header.
	// Each line in the header has the form '#<char>:<value>', where <char> is
	// a single character defining the info field, and <value> is its value.
	//

	while (getline(f, buf) && buf[0] == '#') {

		if (buf.size() < 3) {
			assert(buf.size() > 2);
			continue;
		}

		string val(buf.substr(3));

		switch (buf[1]) {

			case CODE_DATE: 		m_date = str2num<int>(val);		break;
			case CODE_CONF_OPTS:	m_conf_opts = val; 				break;
			case CODE_ICON_PATH:	m_icon_path = val;				break;
			case CODE_SUMMARY: 		m_summary = val; 				break;
			case CODE_URL: 			m_url = val; 					break;
			case CODE_LICENSE: 		m_license = val; 				break;
			case CODE_AUTHOR: 		m_author = val;					break;
			case CODE_DESCRIPTION:
				if (!m_description.empty())
					m_description += "\n";
				m_description += val;
				break;
		}
	}

	// Read list of logged files

	if (f.eof() || buf[0] != '/')
		return;

	if (db_version == 0) { 	// old format
		string::size_type p;
		do { 
			if ((p = buf.find("|")) != string::npos)
				buf.erase(p);
			add_file(buf);
		}
		while (getline(f, buf) && buf[0] == '/');
	}
	else {
		do { add_file(buf); }
		while (getline(f, buf) && buf[0] == '/');
	}

	sort_files();
}


BasePkg::~BasePkg()
{
	for (iter f(m_files.begin()); f != m_files.end(); delete *f++) ;
}


void BasePkg::unlog() const
{
	if (unlink(m_log.c_str()) != 0 && errno != ENOENT)
		throw Error("unlink(" + m_log + ")", errno);
}


string BasePkg::description_str(bool debug /* = false */) const
{
	string const head(debug ? "porg :: " : "");
	string desc("Description: ");
	
	if (m_description.find('\n') == string::npos)
		desc += m_description;
	else {
		std::istringstream is(m_description);
		for (string buf; getline(is, buf); )
			desc += '\n' + head + "   " + buf;
	}

	return desc;
}


string BasePkg::format_description() const
{
	string code(string("#") + CODE_DESCRIPTION + ':');

	if (m_description.empty())
		return code + '\n';

	string ret;
	std::istringstream is(m_description);

	for (string buf; getline(is, buf); )
		ret += code + buf + '\n';

	return ret;
}


void BasePkg::write_log() const
{
	// Create log file

	FileStream<std::ofstream> of(m_log);

	// write info header

	of	<< "#!porg-" PACKAGE_VERSION "\n"
		<< '#' << CODE_DATE 		<< ':' << m_date << '\n'
		<< '#' << CODE_AUTHOR		<< ':' << m_author << '\n'
		<< '#' << CODE_SUMMARY		<< ':' << Porg::strip_trailing(m_summary, '.') << '\n'
		<< '#' << CODE_URL			<< ':' << m_url << '\n'
		<< '#' << CODE_LICENSE		<< ':' << m_license << '\n'
		<< '#' << CODE_CONF_OPTS	<< ':' << m_conf_opts << '\n'
		<< '#' << CODE_ICON_PATH	<< ':' << m_icon_path << '\n'
		<< format_description();

	// write installed files
	
	for (const_iter f(m_files.begin()); f != m_files.end(); ++f)
		of << (*f)->name() << '\n';
}


void BasePkg::add_file(string const& path)
{
	File* file = new File(path);
	m_files.push_back(file);

	if (file->is_installed()) {
		m_nfiles++;
		// detect hardlinks to installed files, to count their size only once
		if (m_inodes.find(file->inode()) == m_inodes.end()) {
			m_inodes.insert(file->inode());
			m_size += file->size();
		}
	}
	else
		m_nfiles_miss++;
}


bool BasePkg::find_file(File* file)
{
	assert(file != 0);

	if (!m_sorted_by_name)
		sort_files();
	
	return std::binary_search(m_files.begin(), m_files.end(), file, Sorter());
}


bool BasePkg::find_file(string const& path)
{
	File file(path);
	return find_file(&file);
}


void BasePkg::sort_files(	sort_t type,	// = SORT_BY_NAME
							bool reverse)	// = false
{
	std::sort(m_files.begin(), m_files.end(), Sorter(type));
	if (reverse)
		std::reverse(m_files.begin(), m_files.end());
	
	m_sorted_by_name = (type == SORT_BY_NAME && !reverse);
}


string BasePkg::get_base(string const& name)
{
	for (string::size_type i = 1; i < name.size(); ++i) {
		if (isdigit(name.at(i)) && name.at(i - 1) == '-')
			return name.substr(0, i - 1);
	}
	return name;
}


string BasePkg::get_version(string const& name)
{
	for (string::size_type i = 1; i < name.size(); ++i) {
		if (isdigit(name.at(i)) && name.at(i - 1) == '-')
			return name.substr(i);
	}
	return "";
}


// convert string to numeric
template <typename T>	// T = {int,long,unsigned,...}
T str2num(std::string const& s)
{
	static std::istringstream is(s);
	is.clear();
	is.str(s);
	T t;
	is >> t;
	return t;
}


//-----------------//
// BasePkg::Sorter //
//-----------------//


BasePkg::Sorter::Sorter(sort_t type /* = SORT_BY_NAME */)
:
	m_sort_func(type == SORT_BY_NAME ? &Sorter::sort_by_name : &Sorter::sort_by_size)
{ }


inline bool BasePkg::Sorter::operator()(File* left, File* right) const
{
	return (this->*m_sort_func)(left, right);
}


inline bool BasePkg::Sorter::sort_by_name(File* left, File* right) const
{
	return left->name() < right->name();
}


inline bool BasePkg::Sorter::sort_by_size(File* left, File* right) const
{
	return left->size() > right->size();
}

