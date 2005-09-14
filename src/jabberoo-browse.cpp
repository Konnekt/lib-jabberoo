/*
 * jabberoo-browse.cpp
 * Jabber Client Library Browse Support
 *
 * Original Code Copyright (C) 1999-2001 Dave Smith (dave@jabber.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contributor(s): Julian Missig (IBM)
 *
 */

#include "stdafx.h"

namespace jabberoo {

BrowseDB::Item::Item(const Element& e)
{
    _type = e.getAttrib("type");
    _category = e.getAttrib("category");
    _jid = e.getAttrib("jid");
    _name = e.getAttrib("name");
    _version = e.getAttrib("version");

    for (Element::const_iterator it = e.begin(); it != e.end(); ++it)
    {
        if ((*it)->getType() == Node::ntElement)
        {
            Element* elem = static_cast<Element*>((*it));
            std::string name = elem->getName();
            if (name == "ns")
            {
                _namespaces.push_back(elem->getCDATA());
            }
            else if (!elem->getAttrib("jid").empty())
            {
                _children.push_back(new BrowseDB::Item(*elem));
            }
        }
    }
}

BrowseDB::Item::~Item()
{
    while(!_children.empty())
    {
        BrowseDB::Item* child = _children.front();
        delete child;
        _children.pop_front();
    }
}

BrowseDB::BrowseDB(Session& sess) : _session(sess), _query(NULL)
{
}

BrowseDB::~BrowseDB()
{
    clear();
}

BrowseDB::Item& BrowseDB::operator[](const std::string& jid)
{
    BrowseDB::iterator it = _children.find(jid);
    if (it == _children.end())
    {
        throw XCP_NotCached();
    }

    return *(it->second);
}

void BrowseDB::cache(const std::string& jid, BrowseCallbackFunc f)
{
    if (!_query)
    {
        _query = _session.registerXPath(
            "/iq[@type='result']/*[@xmlns='jabber:iq:browse']",
            SigC::slot(*this, &BrowseDB::browseXPathCB) );
    }
    // Hook up the callback
    _callbacks[jid] = f;
    
    judo::Element iq("iq");
    iq.putAttrib("type", "get");
    iq.putAttrib("id", _session.getNextID());
    iq.putAttrib("to", jid);
    judo::Element* query = iq.addElement("query");
    query->putAttrib("xmlns", "jabber:iq:browse");

    // Send it out
    _session << iq.toString().c_str();

    // Dupe checking and what not is handled by the xpath that picks up results
}

void BrowseDB::clear()
{
    for (BrowseDB::iterator it = _children.begin(); it != _children.end(); ++it)
    {
        delete it->second;
    }

    _children.clear();
}

void BrowseDB::browseXPathCB(const Element& e)
{
    std::cout << "Received Browse CB: " << e.toString() << std::endl;
    Element* child = NULL;
    std::string jid;

    // Find the first child element and ensure it has a jid
    for (Element::const_iterator it = e.begin(); it != e.end(); ++it)
    {
        if ( (*it)->getType() == Node::ntElement )
        {
           child = static_cast<Element*>(*it);
           jid = child->getAttrib("jid");
           if (!jid.empty())
               break;
        }
    }

    if (child == NULL)
    {
        // XXX MAJOR ERROR HERE
        return;
    }

    // We need to make sure we don't already have a copy of this, if we do
    // clean it up and let the new one take precedence
    BrowseDB::iterator it = _children.find(jid);
    if (it != _children.end())
    {
        _children.erase(it);
    }

    // Cache it up
    BrowseDB::Item* item = new BrowseDB::Item(*child);
    _children[jid] = item;

    // See if we have a callback for this lookup
    std::map<std::string, BrowseCallbackFunc>::iterator i = _callbacks.find(jid);
    if (i == _callbacks.end())
        return;

    i->second(*item);

    _callbacks.erase(i);
}

} // namespace jabberoo
