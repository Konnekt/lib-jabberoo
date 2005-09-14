// packet.hh
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
#ifndef PACKET_HH
#define PACKET_HH

#include <judo.hpp>
#include "jabberoo_conf.h"
namespace jabberoo {
     /**
      * A Jabber Packet with standard attributes.
      * This class has accessors and modifiers for several common Jabber attributes and elements.
      */
     class  Packet
	  {
	  public:
	       /**
		* Jabber Packet constructor with a base element of name.
		* Create a Jabber Packet with a base element of name.
		* @param name A string base element name.
		*/
	       JOO_API Packet(const std::string& name);

	       /**
		* Jabber Packet constructor based upon a judo::Element
		* Create a Jabber Packet based off of a judo::Element.
		* @param t A judo::Element
		*/
	       JOO_API Packet(const judo::Element& t);

	       // Common ops
	       /**
		* from attribute on the base element.
		* @return The string in the from attribute on the base element, NULL if the from attribute is nonexistent.
		*/
	       JOO_API const std::string getFrom()    const;

	       /**
		* to attribute on the base element.
		* @return The string in the to attribute on the base element, NULL if the to attribute is nonexistent.
		*/
	       JOO_API const std::string getTo()      const;

	       /**
		* id attribute on the base element.
		* @return The string in the id attribute on the base element, NULL if the id attribute is nonexistent.
		*/
	       JOO_API const std::string getID() const;

	       /**
		* error subelement of the base element.
		* @return The string in the error subelement of the base element, NULL if the error subelement is nonexistent.
		*/
	       JOO_API const std::string getError() const;

	       /**
		* code attribute on the error subelement.
		* @return The int in the code attribute on the error subelement, 0 if the code attribute or the error subelement is nonexistent.
		*/
	       JOO_API const int getErrorCode() const;

	       /**
		* XML string form of the Jabber Packet.
		* @return The XML forming the Jabber Packet as a std::string.
		*/
	       JOO_API const std::string toString()      const;

	       /**
		* XML judo::Element form of the Jabber Packet.
		* @return The XML forming the Jabber Packet as a judo::Element.
		* @see judo::Element
		*/
	       JOO_API const judo::Element&   getBaseElement() const;

	       /**
		* from attribute on the base element.
		* @param from The JabberID to go in the from attribute on the base element, normally set by the server.
		*/
	       JOO_API void setFrom(const std::string& from);

	       /**
		* to attribute on the base element.
		* @param to The JabberID to send this packet to.
		*/
	       JOO_API void setTo(const std::string& to);

	       /**
		* id attribute on the base element.
		* @param id The string id attribute on the base element.
		*/
	       JOO_API void setID(const std::string& id);


	       /**
		* Add x element to the packet.
		* Adds an x (extension) element to the Jabber Packet.
		* x elements should have an xmlns attribute.
		* @see judo::Element
		* @see judo::Element::putAttrib()
		* @return A pointer to the newly created judo::Element.
		*/
	       JOO_API judo::Element* addX();

	       /**
		* Add x element to the packet.
		* Adds an x (extension) element to the Jabber Packet with
		* the given namespace.
		* @see judo::Element
		* @see judo::Element::putAttrib()
		* @return A pointer to the newly created judo::Element.
		*/
	       JOO_API judo::Element* addX(const std::string& tnamespace);

	       /**
		* Get the x (extension) element which has an xmlns of tnamespace.
		* @see judo::Element
		* @param tnamespace The namespace of the x element to return.
		* @return A pointer to the x element.
		*/
	       JOO_API judo::Element* findX(const std::string& tnamespace) const;

	       /**
		* Erase an x element from the packet.
		* Erases an x (extension) element to the Jabber Packet with
		* the given namespace.
		* @see judo::Element
		* @param tnamespace The namespace of the x element to return.
		*/
	       JOO_API void eraseX(const std::string& tnamespace);

	       /**
		* Access to the base element
		* @see judo::Element
		* @return A reference to the base element of the Jabber Packet as a judo::Element
		*/
	       JOO_API judo::Element& getBaseElement();

	  protected:
	       judo::Element _base;
	  };
} // namespace jabberoo

#endif // PACKET_HH
