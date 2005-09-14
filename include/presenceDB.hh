// presenceDB.hh
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

#ifndef PRESENCEDB_HH
#define PRESENCEDB_HH

#include <sigc++/object.h>
#include <XCP.hh>

#include <string> 
#include <list>
#include <map>
#include <jabberoofwd.h>
#include <jutil.hh>
#include <presence.hh>
#include "jabberoo_conf.h"
namespace jabberoo
{
     /**
      * Presence database.
      * This class keeps track of and handles all Presence packets received.
      * This class plus the Roster class are crucial for clients which want rosters.
      * @see Presence
      * @see Roster
      */
     class  PresenceDB
	  : public SigC::Object
     {
     public:
	  typedef std::list<Presence>::iterator             iterator;
	  typedef std::list<Presence>::const_iterator       const_iterator;
	  typedef std::pair<const_iterator, const_iterator> range;
	  /**
	   * This is thrown if a JID is not in the database.
	   * @see equal_range
	   * @see findExact
	   * @see find
	   */
	  class JOO_API  XCP_InvalidJID : public XCP{};
	  	       
	  /**
	   * Create a PresenceDB for a given Session.
	   * This will begin keeping track of all Presence packets received in a given Session.
	   * @param s The Session.
	   * @see Session
	   */
	  JOO_API PresenceDB(Session& s);
     public:
	  /**
	   * Insert a Presence Packet into the database.
	   * @param p The Presence Packet to insert.
	   */
	  JOO_API void           insert(const Presence& p);
	  /**
	   * Remove a Presence Packet from the database.
	   * Currently this does *not* handle multiple entries with different resources.
	   * This function will remove all Presence Packets which match the user@host.
	   * @param jid The JabberID to remove.
	   */
	  JOO_API void           remove(const std::string& jid);
	  /**
	   * Send unavailable presence for all entries.
	   */
	  void		 send_unavailable(const std::string& jid);
	  /**
	   * Clear the database. Erases all entries.
	   */
	  JOO_API void		 clear();
	  /**
	   * This function will throw XCP_InvalidJID if the JID is not found.
	   * @see XCP_InvalidJID
	   * @return range
	   */
	  JOO_API range          equal_range(const std::string& jid) const;
	  /**
	   * Find Presence Packets from all JabberIDs which have the given user@host.
	   * This function will throw XCP_InvalidJID if the JID is not found.
	   * @return A const iterator of Presence Packets.
	   */
	  JOO_API const_iterator find(const std::string& jid) const;
	  /**
	   * Find the Presence Packet for an exact user@host/resource JabberID.
	   * This function will throw XCP_InvalidJID if the JID is not found.
	   * @return A Presence Packet.
	   */
      JOO_API     Presence       findExact(const std::string& jid) const;
	  /**
	   * Whether the PresenceDB contains a given user@host.
	   */
	  JOO_API bool           contains(const std::string& jid) const;
	  /**
	   * Whether the default Presence for user@host is available.
	   */
	  JOO_API bool           available(const std::string& jid) const;
     private:
	  typedef std::map<std::string, std::list<Presence>, jutil::CaseInsensitiveCmp > db;
	  db::const_iterator find_or_throw(const std::string& jid) const;
	  Session&   _Owner;
	  db         _DB;
     };
} // namespace jabberoo

#endif // PRESENCEDB_HH 1
