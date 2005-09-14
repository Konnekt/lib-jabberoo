// roster.hh
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

#ifndef ROSTER_HH
#define ROSTER_HH

#include <string>
#include <set>
#include <map>
#include <sigc++/object.h>
#include <sigc++/signal.h>
#include <XCP.hh>
#include <jabberoofwd.h>
#include <presence.hh>
#include <jutil.hh>
#include "jabberoo_conf.h"
namespace jabberoo
{
     /**
      * A Jabber Roster.
      * A Roster is a list of users, or Roster Items.
      */
     class Roster
	  : public SigC::Object
	  {
	  public:
	       /**
		* The Subscription you have in relation to this Roster Item.
		* The Subscription determines who can see whose Presence.
		*/
               enum Subscription {
                rsNone,  /**< Neither of you can see the other's presence. */
                rsTo,    /**< You can see their presence, but they cannot see yours. */
                rsFrom,  /**< You cannot see their presence, but they can see yours. */
                rsBoth,  /**< Both of you can see each other's presence. */
                rsRemove /**< This Roster Item is being removed. */
               };

	       // Exceptions
	       class JOO_API  XCP_InvalidJID : public XCP{};

	       /**
		* An individual Roster Item.
		*/
               class Item {
               public:
                    // Initializers
		    /**
		     * Create a Roster Item from a judo::Element.
		     * @param t A judo::Element.
		     * @see judo::Element
		     */
		    JOO_API Item(const judo::Element& t);
		    /**
		     * Create a Roster Item for a particular Roster, given a judo::Element.
		     * @param r A Roster.
		     * @param t A judo::Element.
		     * @see Roster
		     * @see judo::Element
		     */
             JOO_API        Item(Roster& r, const judo::Element& t);
		    /**
		     * Create a Roster Item based on a JabberID and a nickname.
		     * @param jid The JabberID.
		     * @param nickname The nickname.
		     */
		    JOO_API Item(const std::string& jid, const std::string& nickname);
		    /**
		     * Roster Item destructor.
		     */
		    JOO_API ~Item();
               public:
		    // Group modifiers
		    /**
		     * Add this Roster Item to a group.
		     * Roster Items can be in multiple groups.
		     * @param group The name of the group.
		     */
            JOO_API         void addToGroup(const std::string& group);
		    /**
		     * Delete this Roster Item from a group.
		     * Roster Items can be in multiple groups.
		     * @param group The name of the group.
		     */
             JOO_API        void delFromGroup(const std::string& group);
		    /**
		     * Remove this Roster Item from all groups it is in.
		     */
             JOO_API        void clearGroups();
		    /**
		     * Set the nickname for this Roster Item.
		     * @param nickname The nickname.
		     */
		    JOO_API void setNickname(const std::string& nickname);
		    /**
		     * Set the JabberID for this Roster Item.
		     * This will NOT modify an existing Roster Item.
		     * By changing the JabberID, you are essentially
		     * creating a new Roster Item.
		     * @param jid The JabberID.
		     */
		    JOO_API void setJID(const std::string& jid);
        
                    // Info ops
		    /**
		     * Whether or not this Roster Item is online.
		     */
             JOO_API        bool             isAvailable() const;
		    /**
		     * Get the nickname for this Roster Item.
		     */
             JOO_API        std::string           getNickname() const;
		    /**
		     * Get the JabberID for this Roster Item.
		     */
             JOO_API        std::string           getJID() const;
		    /**
		     * Get the Subscription to this Roster Item.
		     * @see Subscription
		     */
             JOO_API        Subscription     getSubsType() const;
		    /**
		     * Whether or not this Roster Item is pending a subscription approval.
		     */
		    JOO_API bool             isPending() const;

                    // Group pseudo-container ops/iterators
		    typedef std::set<std::string>::const_iterator iterator;
		    /**
		     * Get the first group this item belongs to.
		     * @return An iterator to the first group.
		     */
		    JOO_API iterator begin() const { return _groups.begin(); }
		    /**
		     * Get the last group this item belongs to.
		     * @return An iterator to the end group.
		     */
		    JOO_API iterator end()   const { return _groups.end(); }
		    /**
		     * Whether this item has any groups.
		     * @return False if this item belongs to any groups.
		     */
		    JOO_API bool     empty() const { return _groups.empty(); }

	       protected:
                    // Update handlers
                    void update(Roster& r, const std::string& jid, const Presence& p, Presence::Type prev_type);
		    bool update(const judo::Element& t);
                    bool update(Roster& r, const judo::Element& t);
               private:
		    friend class Roster;
		    int          _rescnt;
            std::set<std::string>  _groups;
                    Subscription _type;
		    bool         _pending;
                    std::string       _nickname;
                    std::string       _jid;
               };
	       
	       typedef std::map<std::string, Item, jutil::CaseInsensitiveCmp> ItemMap;

	       // Non-const iterators
	       typedef jutil::ValueIterator<ItemMap::iterator, Item > iterator;
	       /**
		* Get the first Roster::Item
		* @return An iterator to the first Roster Item.
		*/
	       JOO_API iterator begin() { return _items.begin(); }
	       /**
		* Get the last Roster::Item
		* @return An iterator to the end Roster Item.
		*/
	       JOO_API iterator end() { return _items.end(); }

		   /*RL*/
	       JOO_API void clear() { _items.clear(); }

	       // Const iterators
	       typedef jutil::ValueIterator<ItemMap::const_iterator, const Item> const_iterator;
	       /**
		* Get the first Roster::Item
		* @return A const iterator to the first Roster Item.
		*/
	       JOO_API const_iterator begin() const { return _items.begin(); }
	       /**
		* Get the last Roster::Item
		* @return A const iterator to the end Roster Item.
		*/
	       JOO_API const_iterator end() const { return _items.end(); }

               // Intializers
	       /**
		* Create a Roster for a given Session.
		* @param s The Session.
		* @see Session
		*/
          JOO_API      Roster(Session& s);

	       // Accessors
	       /**
		* Get the Session this Roster is using
		* @return The Session.
		*/
	      JOO_API  Session& getSession() { return _owner; }

	       // Operators
	       /**
		* Get a Roster Item in the Roster given a JabberID.
		*/
	      JOO_API  const Roster::Item& operator[](const std::string& jid) const;
	      JOO_API  Roster::Item& operator[](const std::string& jid);
	       /**
		* Add a Roster Item to the Roster.
		*/
	      JOO_API  Roster& operator<<(const Item& i) { update(i); return *this; }
          public:
          JOO_API      void reset();

               // Information
	       /**
		* Whether or not this Roster contains a certain JabberID.
		* @param jid The JabberID.
		*/
          JOO_API      bool containsJID(const std::string& jid) const;

               // Update ops
	       /**
		* Add a Roster Item to the Roster given a judo::Element.
		* This is how you push Items as they're received.
		* @param t The judo::Element
		*/
         JOO_API       void update(const judo::Element& t);	   // Roster push
	       /**
		* Update the Presence for the appropriate Roster Items.
		* @param p The Presence.
		* @param prev_type The previous Presence::Type for the Roster Item the Presence refers to.
		*/
              JOO_API  void update(const Presence& p, Presence::Type prev_type);     // Roster presence
	       /**
		* Add a Roster Item.
		* This is generally used when an application wants to
		* add or modify Roster Items. Note that duplicate Roster Items
		* for a particular JabberID cannot exist, the previous one for 
		* the JabberID will be overwritten.
		* @param i The Roster::Item to add/modify.
		*/
	      JOO_API  void update(const Item& i);	   // Roster item add/modify

               // Control ops
	       /**
		* Remove a Roster Item based on JabberID.
		* @param jid The JabberID.
		*/
              JOO_API  void deleteUser(const std::string& jid); /* Remove the user w/ JID */
	       /**
		* Fetch the Roster from the server.
		*/
              JOO_API  void fetch() const;                 /* Retrieve roster from server */             
               /**
               * Get the number of items in the roster
               */
              JOO_API  int size() const
               { return _items.size(); }

               // Translation
              JOO_API  static std::string translateS10N(Subscription stype);
              JOO_API  static Subscription translateS10N(const std::string& stype);
              JOO_API  static std::string filterJID(const std::string& jid);

	       // Item/Group access -- HACK
              JOO_API  const std::map<std::string, std::set<std::string> >& getGroups() const { return _groups;} 
          public:
	       /**
		* This signal is emitted whenever the Roster display should be refreshed.
		*/
               SigC::Signal0<void, SigC::Marshal<void> >                      evtRefresh;

               /**
                * This signal is emitted just before the referenced item is removed from the roster.
                */
                SigC::Signal1<void, Item&> evtRemovingItem;
                /** 
                 * This signal is emitted just before the reference item is changed in the roster.
                 */
                SigC::Signal1<void, Item&> evtUpdating;
                /**
                 * This signal is emitted after an item is udpated.
                 */
                SigC::Signal1<void, Item&> evtUpdateDone;
                /** [RL]
                 * This signal is emitted after an item is added.
                 */
                SigC::Signal1<void, Item&> evtItemAdded;
	       /**
		* This signal is emitted whenever the Presence for a user changes.
		* @param jid The JabberID for the Presence.
		* @param available Whether or not this user is online.
		* @param prev_type The previous Presence::Type of this user.
		*/
           SigC::Signal3<void, const std::string&, bool, Presence::Type, SigC::Marshal<void> > evtPresence;
          private:
	       friend class Item;
	       void mergeItemGroups(const std::string& itemjid, const std::set<std::string>& oldgrp, const std::set<std::string>& newgrp);
               void removeItemFromGroup(const std::string& group, const std::string& jid);
	       void removeItemFromAllGroups(const Item& item);
	       void deleteAgent(const judo::Element& iq);
               ItemMap                   _items;
               std::map<std::string, std::set<std::string> > _groups;
               Session&                  _owner;
     };

}  // namespace jabberoo
#endif // #ifndef ROSTER_HH
