/* jabberoo.cc
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
 */

#include "stdafx.h"
#include <JID.hh>
#include "XPath.h"

namespace jabberoo {

struct JIDEqualsFunction : public judo::XPath::Function
{
    bool run(judo::XPath::Value* ctxt, judo::XPath::Op::OpList& args)
    {
        judo::XPath::Value::ElemList& elems = ctxt->getList();
        judo::XPath::Value::ElemList::iterator it = elems.begin();
                                                                            
        while (it != elems.end())
        {
            std::string value;
            judo::XPath::Value::ElemList::iterator next = it;
            ++next;
                                                                            
            judo::Element* elem = *it;
            std::string val1 = args[0]->calcStr(elem);
            std::string val2 = args[1]->calcStr(elem);
                                                                            
            if (JID::compare(val1, val2) != 0)
            {
                elems.erase(it);
            }

            it = next;
        }
                                                                            
        if (elems.empty())
        {
            return false;
        }
                                                                            
        return true;
                                                                            
    }
};

void JID::init()
{
    // Add our XPath ops
    judo::XPath::add_function("jid-equals", new jabberoo::JIDEqualsFunction());
}

std::string JID::getResource(const std::string& jid)
{     // Search for resource divider
     std::string::size_type i = jid.find("/");
     // If it was found, extract the resource and return it..
     if (i != std::string::npos)
	return jid.substr(i+1, jid.length()-i);
     else
     	return "";
}

std::string JID::getUserHost(const std::string& jid)
{
     // Search for resource divider..
     std::string::size_type i = jid.find("/");
     // If it was found, extract he userhost and return..
     if (i != std::string::npos)
	  return jid.substr(0,i);
     else
	  return jid;
}

std::string JID::getHost(const std::string& jid)
{
     std::string::size_type d1 = jid.find("@"); // Search for @ divider
     std::string::size_type d2 = jid.find("/"); // Search for / divider
     if ((d1 != std::string::npos) && (d2 != std::string::npos))
	  return jid.substr(d1+1,d2-d1-1);
     else if ((d1 != std::string::npos) && (d2 == std::string::npos))
	  return jid.substr(d1+1, jid.length());
     else if ((d1 == std::string::npos) && (d2 != std::string::npos))
	  return jid.substr(0,d2);
//     else if ((d1 == std::string::npos) && (d2 == std::string::npos))
     else
	  return jid;
}

std::string JID::getUser(const std::string& jid)
{
	std::string::size_type d1 = jid.find("@");
	if (d1 != std::string::npos)
		return jid.substr(0, d1);
	else
		return "";
}

bool JID::isValidUser(const std::string& user)
{
     if ( (user.empty()) ||
	  (user.find(' ') != std::string::npos) ||
	  (user.find('@') != std::string::npos) ||
	  (user.find('/') != std::string::npos) ||
	  (user.find('\'') != std::string::npos) ||
	  (user.find('\"') != std::string::npos) ||
	  (user.find(':') != std::string::npos)
	  // Insert other invalid chars here
	  )
     {
	  return false;
     }
     return true;
}

bool JID::isValidHost(const std::string& host)
{
     if ( (host.empty()) ||
	  (host.find(' ') != std::string::npos) ||
	  (host.find('@') != std::string::npos) ||
	  (host.find('/') != std::string::npos) ||
	  (host.find('\'') != std::string::npos) ||
	  (host.find('\"') != std::string::npos)
	  // Insert other invalid chars here
	  )
     {
	  return false;
     }
     return true;
}

int JID::compare(const std::string& ljid, const std::string& rjid)
{
     // User and Host are case insensitive, Resource is case sensitive
     int userhost = _stricmp(JID::getUserHost(ljid).c_str(), JID::getUserHost(rjid).c_str());
     int resource = JID::getResource(ljid).compare(JID::getResource(rjid));

     // If the user and host of both are equal, return whether the resource is
     if (userhost == 0)
	  return resource;
     return userhost;
}

bool JID::Compare::operator()(const std::string& lhs, const std::string& rhs) const
{
     return (JID::compare(lhs, rhs) < 0);
}

} // namespace jabberoo
