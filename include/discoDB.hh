// discoDB.hh
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

#ifndef DISCODB_HH
#define DISCODB_HH

#include <time.h>
#include <cstdio>
#include <cstring>
#include <map>
#include <list>
#include <utility>
#include <set>
#include <algorithm>
#include <functional>
#include <iostream>

#include <jutil.hh>
#include <judo.hpp>
#include <XPath.h>
#include <XCP.hh>
#include "packet.hh"
#include "message.hh"
#include "presence.hh"
#include <judo.hpp>
#include <sigc++/object.h>
#include <sigc++/signal.h>
#include <sigc++/slot.h>
#include <string>
#include <jabberoofwd.h>
#include "jabberoo_conf.h"

namespace jabberoo {

     /**
      * DiscoDB.
      * Don't blame me, temas did it.
      */
    class DiscoDB : public SigC::Object
    {
    public:
        class Item
        {
			friend DiscoDB;
        public:
			/// Item state flags (bitmap)
			enum State {
				stNone = 0, ///< Nothing queried
				stQuerying = 1, ///< Query sent, waiting for answer (not used!)
				stError = 2, ///< Last result was an error
				stItems = 0x10, ///< Got Items
				stInfo = 0x20, ///< Got Info
				stReady = 0x30, ///< Got Both...
			};

            /// A single identity on an Item
            class Identity
            {
            public:
                JOO_API Identity(const judo::Element& e);
                JOO_API Identity(const std::string& category, const std::string& type);
                JOO_API ~Identity();

                JOO_API const std::string& getType() const;
                JOO_API const std::string& getCategory() const;

            private:
                std::string _category;
                std::string _type;
            };

            typedef std::list<std::string> FeatureList;
            typedef std::list<Identity*> IdentityList;
            typedef std::list<DiscoDB::Item*>::iterator iterator;
            typedef std::list<DiscoDB::Item*>::const_iterator const_iterator;


            JOO_API Item(const std::string& jid);
            JOO_API ~Item();

            JOO_API const std::string& getJID() const;
            JOO_API void setNode(const std::string& val);
            JOO_API const std::string& getNode() const;
            JOO_API void setName(const std::string& val);
            JOO_API const std::string& getName() const;
            JOO_API const IdentityList& getIdentityList() const;
            JOO_API const FeatureList& getFeatureList() const;
            JOO_API bool hasChildren() const;
            JOO_API iterator begin();
            JOO_API const_iterator begin() const;
            JOO_API iterator end();
            JOO_API const_iterator end() const;
            JOO_API void appendChild(DiscoDB::Item* item);
            JOO_API void addFeature(const std::string& feature);
            JOO_API void addIdentity(const judo::Element& e);
            JOO_API void addIdentity(const std::string& category, 
                             const std::string& type);
			JOO_API DiscoDB::Item* find(const std::string& jid , const std::string& node);
			JOO_API bool hasChild(const Item * item) const;

            JOO_API State getState() const;
            JOO_API void setState(State state);

        private:
            std::string _jid;
            std::string _node;
            std::string _name;
            std::list<DiscoDB::Item*> _children;
            FeatureList _features;
            IdentityList _identities;
			State _state;
        };
/*RL: std:map na std:multimap*/
        typedef std::multimap<std::string, DiscoDB::Item*>::iterator iterator;
        typedef std::multimap<std::string, DiscoDB::Item*>::const_iterator const_iterator;
        typedef SigC::Slot1<void, const DiscoDB::Item*> DiscoCallbackFunc;

        class JOO_API  XCP_NotCached : public XCP{};


        JOO_API DiscoDB(jabberoo::Session& sess);
        JOO_API ~DiscoDB();

        /** 
        * Search the cache for a JID or throw XCP_NotCached if not found
        *
        * This will only return information for JIDs that have been directly
        * queried.  Some of these JIDs may have children attached to them, but
        * they will not be in the DB until they are authortatively answered for
        * by a direct query.
        */
        JOO_API DiscoDB::Item& operator[](const std::string& jid);

        JOO_API DiscoDB::Item* find(const std::string& jid , const std::string& node = "");

        /// Tell the cache to go out and cache a specific jid
        JOO_API void cache(const std::string& jid, DiscoCallbackFunc f, 
                   bool get_items = false);
        JOO_API void cache(const std::string& jid, const std::string& node,
                   DiscoCallbackFunc f, bool get_items = false);

        /// Cleanup the cache
        JOO_API void clear();

        JOO_API iterator begin()
        { return _items.begin(); }
        
        JOO_API const_iterator begin() const
        { return _items.begin(); }

        JOO_API iterator end()
        { return _items.end(); }

        JOO_API const_iterator end() const
        { return _items.end(); }

        JOO_API void erase(iterator it)
        { delete it->second; _items.erase(it); }

        /**
         * Signal is fired whenever a new item is added to the cache.
         * This is most commonly caught for filtering into special lists.
         */
        SigC::Signal1<void, const DiscoDB::Item&> signal_new_item;

        typedef std::multimap<std::string, DiscoDB::Item*> ItemMap;
        typedef std::multimap<std::string, DiscoCallbackFunc> CallbackMap;
		/** RL: Store  query_id=>node  in case if server doesn't give us node back in result... (happens!) */
        typedef std::map<std::string, std::string> PersistNodesMap;

	private:

        jabberoo::Session& _session;
        CallbackMap _callbacks;
        ItemMap _items;
		PersistNodesMap _persistNodes;

        void browseCB(const judo::Element& e);
        void discoInfoCB(const judo::Element& e);
        void discoItemsCB(const judo::Element& e);
        void runCallbacks(const std::string& jid, const std::string& node, const DiscoDB::Item* item);
    };
} // namespace jabberoo

#endif  // #ifndef DISCODB_HH
