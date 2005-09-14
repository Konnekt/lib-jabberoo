// JID.hh
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

#ifndef JID_HH
#define JID_HH

#include "jabberoo_conf.h"

namespace jabberoo
{
     /**
      * JabberIDs consist of a username, a server (host), and a resource.
      * This class provides functionality for obtaining the strings of the pieces.
      * Currently the format is user@server/resource.
      */
     class  JID 
	  {
	  public:
        /// Setup XPath Ops and other startup functions.
        static void init();

	       /**
		* Get the resource part of a JabberID.
		* @return the resource.
		*/
	       JOO_API static std::string getResource(const std::string& jid);
	       /**
		* Get the username and the server (host) from a JabberID.
		* @return The username and server in the form user@server
		*/
	       JOO_API static std::string getUserHost(const std::string& jid);	    
	       /**
		* Get the server from a JabberID.
		* @return The server (host).
		*/
	       JOO_API static std::string getHost(const std::string& jid);
	       /**
		* Get the username from a JabberID.
		* @return The username.
		*/
	       JOO_API static std::string getUser(const std::string& jid);
	       /**
		* Determine whether the username is valid.
		* @return true if the username is valid.
		*/
	       JOO_API static bool isValidUser(const std::string& user);
	       /**
		* Determine whether the hostname is valid.
		* @return true if the hostname is valid.
		*/
	       JOO_API static bool isValidHost(const std::string& host);
	       /**
		* Compare two JabberIDs.
		* Since the username and server (host) are not case sensitive, but the
		* resource is, this function is available to compare JabberIDs.
		* @return The result of a compare, similar to strcompare
		*/
	       JOO_API static int compare(const std::string& ljid, const std::string& rjid);

	       /**
		* Another way to use the JID::compare function.
		* @see JID::compare
		*/
	       JOO_API struct  Compare {
		    /**
		     * Another way to use the JID::compare function.
		     * @return Whether the JabberIDs are equivalent.
		     */
		    bool operator()(const std::string& lhs, const std::string& rhs) const;
	       };
	  };
}  // namespace jabberoo
#endif // #ifndef JID_HH
