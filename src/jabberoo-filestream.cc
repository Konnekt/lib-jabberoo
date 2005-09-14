// jabberoo-filestream.cc
// Jabber client library
//
// Original Code Copyright (C) 1999-2001 Dave Smith (dave@jabber.org)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Contributor(s): Julian Missig
//
// This Original Code has been modified by IBM Corporation. Modifications
// made by IBM described herein are Copyright (c) International Business
// Machines Corporation, 2002.
//
// Date             Modified by     Description of modification
// 01/20/2002       IBM Corp.       Updated to libjudo 1.1.1
// 2002-03-05       IBM Corp.       Updated to libjudo 1.1.5
// 2002-07-09       IBM Corp.       Added Roster::getSession()
//
// =====================================================================================

#include <filestream.hh>
#include <judo.hpp>

#include <fstream>

using namespace jabberoo;
using namespace std;

FileStream::FileStream(const char *fn):
    _stream(this)
{
    _root = NULL;
    _filename = fn;
}

FileStream::~FileStream()
{
    Cleanup();
}

void FileStream::Cleanup()
{
    delete _root;
    _root = NULL;
}

bool FileStream::ParseFile(const std::string& root_name)
{
    ifstream fs;
	fs.open(_filename);
	
	char chunk[1024];
	
    try
    {
        if (!root_name.empty())
            _stream.push("<" + root_name + ">");
        while(!fs.eof()) 
        {
            fs.read(chunk, 1024);
            int rcount = fs.gcount();
            if(rcount <= 0)
                break;
            _stream.push(chunk, rcount);
        }
        if (!root_name.empty())
            _stream.push("</" + root_name + ">");
    } 
    catch (judo::ElementStream::exception::ParserError& e)
    {
        _error = e.getMessage();
        fs.close();
        Cleanup();
        return false;
    }

    fs.close();

    return true;
}

void FileStream::onDocumentStart(judo::Element* elem)
{
    _root = elem;
}

void FileStream::onElement(judo::Element* elem)
{
    _root->appendChild(elem);
}

void FileStream::onDocumentEnd(void)
{
}

/// Parent becomes responsible for destroying root
judo::Element* FileStream::getRoot()
{
    judo::Element* ret = _root;
    _root = NULL;
    return ret;
}
