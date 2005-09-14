// presence.hh
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

#ifndef PRESENCE_HH
#define PRESENCE_HH

#include <packet.hh>
#include "jabberoo_conf.h"
namespace jabberoo
{
     /**
      * A Jabber Presence Packet.
      * This class implements most of the methods needed to deal with a Jabber Presence Packet.
      * @see jabberoo::Packet
      */
     class  Presence
	  : public Packet
	  {
	  public:
	       /**
		* Presence type of the Packet.
		*/
	       enum Type {
		    ptSubRequest,   /**< A subscription request. */
		    ptUnsubRequest, /**< An unsubscription request. */
		    ptSubscribed,   /**< Subscription confirmed/allowed/approved. */
		    ptUnsubscribed, /**< Subscription revoked/denied. */
		    ptAvailable,    /**< Available presence (standard online presence). */
		    ptUnavailable,  /**< Unavailable presence (offline). */
		    ptError,        /**< Error (offline). */
		    ptInvisible     /**< Invisible presence, others see you as offline. */
	       };

	       /**
		* How the Presence should be displayed.
		* In addition to simply being available/unavailable, show hints at how a 
		* user's presence should be displayed in clients.
		*/
	       enum Show {
		    stInvalid, /**< Not a valid show type. */
		    stOffline, /**< Show user as offline. */
		    stOnline,  /**< Show user as online. */
		    stChat,    /**< Show user as wanting to chat. */
		    stAway,    /**< Show user as away. */
		    stXA,      /**< Show user as extended away. */
		    stDND      /**< Show user as Do Not Disturb. */
	       };

	       /**
		* Construct a Presence Packet based upon the given judo::Element.
		* @see judo::Element
		* @see jabberoo::Packet
		* @param t A judo::Element which should have a presence base element
		*/
	       JOO_API Presence(const judo::Element& t);

	       /**
		* Construct a Presence Packet based upon given values
		* @see jabberoo::Packet
		* @param jid The JabberID to send this presence to. An empty string sends to everyone who has the proper subscription.
		* @param ptype The Presence::Type of presence to send.
		* @param stype The Presence::Show for the presence, Presence::stInvalid leaves it blank.
		* @param status The status message for the presence. Can be an empty string.
		* @param priority The priority of this presence. Should be a string of an int which is 0 or greater. Empty string sets it to 0.
		*/
	       JOO_API Presence(const std::string& jid, Type ptype, Show stype = stInvalid, const std::string& status = "", const std::string& priority = "0");

	       // Modifiers
	       /**
		* Set the presence type.
		* @see Presence::Type
		* @param ptype The Presence::Type of presence to send.
		*/
	       JOO_API void setType(Presence::Type ptype);
	       /**
		* Set the presence status message.
		* @param status The status message for the presence. Can be an empty string.
		*/
	       JOO_API void setStatus(const std::string& status);
	       /**
		* Set the presence show.
		* @see Presence::Show
		* @param stype The Presence::Show for the presence.
		*/
	       JOO_API void setShow(Presence::Show stype);
	       /**
		* Set the presence priority.
		* @param priority The priority of this presence. Should be a string of an int.
		*/
	       JOO_API void setPriority(const std::string& priority);

	       // Accessors
	       /**
		* Get the presence type.
		* @see Presence::Type
		* @return The Presence::Type of the Presence Packet.
		*/
	       JOO_API Type          getType()     const;
	       /**
		* Get the presence show.
		* @see Presence::Show
		* @return The Presence::Show of the Presence Packet.
		*/
	       JOO_API Show          getShow()     const;
	       /**
		* Get the presence status message.
		* In subscription request Presence Packets, the status message is the request reason.
		* @return The status message in the Presence Packet.
		*/
	       JOO_API const std::string  getStatus()   const;
	       /**
		* Get the Presence::Show as a string.
		* @see Presence::Show
		* @return The string version of the Presence::Show of the Presence Packet.
		*/
	       JOO_API const std::string  getShow_str() const;
	       /**
		* Get the priority of the presence.
		* The priority determines which resource messages should default to sending to
		* if the sender has logged in multiple times. If two resources have the same
		* priority, the most recently logged in resource is the default.
		* @return Priority as an int, 0 or greater. 0 if nonexistent.
		*/
	       JOO_API int           getPriority() const;

	  protected:
	       static std::string translateType(Type ptype);
	       static Type   translateType(const std::string& ptype);
	       static std::string translateShow(Show stype);
	       static Show   translateShow(Type ptype, const std::string& stype);

	  private:
	       Show   _show;
	       Type   _type;
	       int    _priority;
	  };
} // namespace jabberoo

#endif // PRESENCE_HH
