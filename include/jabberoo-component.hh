/* jabberoo-component.hh
 * Jabber client library
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
 * Contributor(s): Julian Missig
 *
 * This Original Code has been modified by IBM Corporation. Modifications 
 * made by IBM described herein are Copyright (c) International Business 
 * Machines Corporation, 2002.
 *
 * Date             Modified by     Description of modification
 * 01/20/2002       IBM Corp.       Updated to libjudo 1.1.1
 * 2002-03-05       IBM Corp.       Updated to libjudo 1.1.5
 * 2002-07-09       IBM Corp.       Added Roster::getSession()
 */

#ifndef INCL_JABBEROO_COMPONENT_H
#define INCL_JABBEROO_COMPONENT_H

#include <jabberoofwd.h>

#include <judo.hpp>
#include <sigc++/object.h>
#include <sigc++/signal.h>

#include <string>
#include <list>
#include <map>
#include <XPath.h>
#include <packet.hh>
#include "jabberoo_conf.h"

namespace jabberoo {
    
class ComponentSession:
    public judo::ElementStreamEventListener, public judo::ElementStream, public SigC::Object
{
public:
    enum ConnectionState
    {
        csNotConnected,
        csAwaitingAuth,
        csConnected
    };

    JOO_API ComponentSession();
    JOO_API virtual ~ComponentSession();

    JOO_API const std::string& getComponentID() const
    { return _componentID; }

    JOO_API ConnectionState getState() const
    { return _connState; }

    JOO_API void connect(const std::string& server, const std::string& componentID, 
            const std::string& password);

    JOO_API void disconnect();

    JOO_API judo::XPath::Query* registerXPath(const std::string& query, ElementCallbackFunc f);

    JOO_API void unregisterXPath(judo::XPath::Query* id);

    JOO_API ComponentSession& operator>>(const char* buffer) { push(buffer, strlen(buffer)); return *this; } 
    JOO_API ComponentSession& operator<<(const Packet& p) { evtTransmitXML(p.toString().c_str()); return *this;}
    JOO_API ComponentSession& operator<<(const char* buffer) { evtTransmitXML(buffer); return *this;}
    JOO_API virtual void push(const char* data, int datasz);


    SigC::Signal1<void, const char*, SigC::Marshal<void> >        evtTransmitXML;
    SigC::Signal0<void, SigC::Marshal<void> >         evtConnected;
protected:
    virtual void onDocumentStart(judo::Element* t);
    virtual void onElement(judo::Element* t);
    virtual void onCDATA(judo::CDATA* c);
    virtual void onDocumentEnd();

private:
    std::string _server;
    std::string _componentID;
    std::string _password;
    std::string _SessionID;
    ConnectionState _connState;
    std::list<judo::XPath::Query*> _XPaths;
    std::map<judo::XPath::Query*, ElementCallbackFunc> _XPCallbacks;
};

} // namespace jabberoo
#endif //INCL_JABBEROO_COMPONENT_H
