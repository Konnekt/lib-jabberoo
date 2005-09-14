/*
 * jabberoo-component-session.cpp
 * Jabber Component Session management
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
#include "jabberoo-component.hh"

using namespace jabberoo;

ComponentSession::ComponentSession() : 
    judo::ElementStream(this), _connState(csNotConnected)
{ }

ComponentSession::~ComponentSession()
{
    if (_connState != csNotConnected)
    {
        disconnect();
    }

    while (!_XPaths.empty())
    {
        judo::XPath::Query* query = _XPaths.front();
        delete query;
        _XPaths.pop_front();
    }
}

void ComponentSession::connect(const std::string& server, const std::string& componentID,
        const std::string& password)
{
    _componentID = componentID;
    _server = server;
    _password = password;

    assert(_connState == csNotConnected);

    reset();

    *this << "<stream::stream to='" << _componentID.c_str() << "' xmlns='jabber:component:accept' xmlns:stream='http://etherx.jabber.org/streams'>";

    _connState = csAwaitingAuth;
}

void ComponentSession::disconnect()
{
    assert(_connState != csNotConnected);

    *this << "</stream:stream>";
    
    _connState = csNotConnected;
}

void ComponentSession::push(const char* data, int datasz)
{
    try 
    {
        judo::ElementStream::push(data, datasz);
    } 
    catch (const judo::ElementStream::exception::ParserError& error) 
    {
        disconnect();
    }
}

judo::XPath::Query* ComponentSession::registerXPath(const std::string& query, 
        ElementCallbackFunc f)
{
    judo::XPath::Query* xpq = new judo::XPath::Query(query);
    _XPaths.push_front(xpq);
    _XPCallbacks.insert(std::make_pair(xpq, f));

    return xpq;
}

void ComponentSession::unregisterXPath(judo::XPath::Query* id)
{
	std::remove(_XPaths.begin(), _XPaths.end(), id);
    _XPCallbacks.erase(id);
    delete id;
}

void ComponentSession::onDocumentStart(judo::Element* t)
{
    // Retrieve the SID from the stream header
    _SessionID = t->getAttrib("id");
    
    std::string full = _SessionID + _password;
    *this << "<handshake>" << shahash(full.c_str()) << "</handshake>";
}

void ComponentSession::onElement(judo::Element* t) 
{
    if (_connState != csConnected && t->getName() == "handshake")
    {
        _connState = csConnected;
        delete t;
        evtConnected();
        return;
    }

    judo::Element& tref = *t;
    // See if a judo::xpath handles this
    typedef std::list<judo::XPath::Query*>::iterator IT;
    for (IT it = _XPaths.begin(); it != _XPaths.end(); it++)
    {
        if ((*it)->check(tref))
        {
            _XPCallbacks[(*it)](tref);
        }
    }

    delete t;
}

void ComponentSession::onCDATA(judo::CDATA* c)
{
     // If we ever want to deal with CDATA we receive
     // possibly message sizes and things like that
     // then we do it here.
     delete c;
}

void ComponentSession::onDocumentEnd() 
{
    disconnect();
}

