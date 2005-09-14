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
#include "discoDB.hh"
#include "session.hh"

#include <sigc++/object_slot.h>

namespace jabberoo {

    // CLASS Identity
DiscoDB::Item::Identity::Identity(const judo::Element& e)
{
    _type = e.getAttrib("type");
    _category = e.getAttrib("category");
}

DiscoDB::Item::Identity::Identity(const std::string& category, const std::string& type):
    _category(category), _type(type)
{ }

DiscoDB::Item::Identity::~Identity()
{ }

const std::string& DiscoDB::Item::Identity::getType() const
{ return _type; }

const std::string& DiscoDB::Item::Identity::getCategory() const
{ return _category; }


    // CLASS Item
DiscoDB::Item::~Item()
{
    while(!_identities.empty())
    {
        DiscoDB::Item::Identity* ident = _identities.front();
        delete ident;
        _identities.pop_front();
    }
}

DiscoDB::Item::Item(const std::string& jid) : _jid(jid)
{ _state = stNone; }

const std::string& DiscoDB::Item::getJID() const
{ return _jid; }

void DiscoDB::Item::setNode(const std::string& val)
{ _node = val; }

const std::string& DiscoDB::Item::getNode() const
{ return _node; }

void DiscoDB::Item::setName(const std::string& val)
{ _name = val; }

const std::string& DiscoDB::Item::getName() const
{ return _name; }

const DiscoDB::Item::IdentityList& DiscoDB::Item::getIdentityList() const
{ return _identities; }

const DiscoDB::Item::FeatureList& DiscoDB::Item::getFeatureList() const
{ return _features; }

bool DiscoDB::Item::hasChildren() const
{ return (_children.size() > 0); }

DiscoDB::Item::iterator DiscoDB::Item::begin()
{ return _children.begin(); }

DiscoDB::Item::const_iterator DiscoDB::Item::begin() const
{ return _children.begin(); }

DiscoDB::Item::iterator DiscoDB::Item::end()
{ return _children.end(); }

DiscoDB::Item::const_iterator DiscoDB::Item::end() const
{ return _children.end(); }

void DiscoDB::Item::appendChild(DiscoDB::Item* item)
{ _children.push_back(item); }

void DiscoDB::Item::addFeature(const std::string& feature)
{ _features.push_back(feature); }

void DiscoDB::Item::addIdentity(const judo::Element& e)
{ _identities.push_back(new Identity(e)); }

void DiscoDB::Item::addIdentity(const std::string& category, 
         const std::string& type)
{ _identities.push_back(new Identity(category, type)); }

DiscoDB::Item::State DiscoDB::Item::getState() const {
	return _state;
}

void DiscoDB::Item::setState(DiscoDB::Item::State state) {
	_state = state;
}

bool DiscoDB::Item::hasChild(const Item * item) const {
	return std::find(_children.begin() , _children.end() , item) != _children.end();
}



DiscoDB::DiscoDB(Session& sess) : 
    _session(sess) 
{
}

DiscoDB::~DiscoDB()
{
    clear();
}

DiscoDB::Item& DiscoDB::operator[](const std::string& jid)
{
    DiscoDB::iterator it = _items.find(jid);
    if (it == _items.end())
    {
        throw XCP_NotCached();
    }

    return *(it->second);
}

DiscoDB::Item* DiscoDB::find(const std::string& jid , const std::string& node) {
    // Make sure we already got the items stuff in there
    std::pair<ItemMap::iterator, ItemMap::iterator> items = _items.equal_range(jid);
    if (items.first == items.second)
    {
        return NULL;
    }
    else
    {
        // Find the one with the same node value
        for (ItemMap::iterator i = items.first; i != items.second; ++i)
        {
            if (i->second->getNode() == node)
            {
                return i->second;
            }
        }
    }
	return NULL;
}

DiscoDB::Item* DiscoDB::Item::find(const std::string& jid , const std::string& node) {
    // Make sure we already got the items stuff in there
	for (std::list<DiscoDB::Item*>::iterator i = _children.begin(); i != _children.end(); ++i) {
        if ((*i)->getJID() == jid && (*i)->getNode() == node)
        {
            return *i;
        }
    }
	return NULL;
}


void DiscoDB::cache(const std::string& jid, DiscoCallbackFunc f, bool get_items)
{
	cache(jid , "" , f , get_items);
}

void DiscoDB::cache(const std::string& jid, const std::string& node,
    DiscoCallbackFunc f, bool get_items)
{
    // Hook up the callback
	_callbacks.insert(CallbackMap::value_type(node.empty() ? jid : (jid + "::" + node), f));
    
    // Build our node
    judo::Element iq("iq");
    iq.putAttrib("type", "get");
    std::string id = _session.getNextID();
    iq.putAttrib("id", id);
    iq.putAttrib("to", jid);
    judo::Element* query = iq.addElement("query");
    if (get_items)
    {
        query->putAttrib("xmlns", "http://jabber.org/protocol/disco#items");
        _session.registerIQ(id, SigC::slot(*this, &DiscoDB::discoItemsCB));
    }
    else
    {
        query->putAttrib("xmlns", "http://jabber.org/protocol/disco#info");
        _session.registerIQ(id, SigC::slot(*this, &DiscoDB::discoInfoCB));
    }
	// Create or set new item
	if (!node.empty()) {
		query->putAttrib("node", node);
		_persistNodes[id] = node; //RL
	}
	Item * item = find(jid , node);
	if (!item) {
		item = new Item(jid);
		item->setNode(node);
	    _items.insert(ItemMap::value_type(jid, item));
	}
	//item->setState((Item::State) ((item->getState() & Item::stReady) | Item::stQuerying));
	item->setState(Item::stQuerying);

    // Send it out
    _session << iq.toString().c_str();
}

void DiscoDB::clear()
{
    for (DiscoDB::iterator it = _items.begin(); it != _items.end(); ++it)
    {
        delete it->second;
    }

    _items.clear();
}

void DiscoDB::discoInfoCB(const judo::Element& e)
{
    // Catch errors and see if we can browse instead
    if (e.cmpAttrib("type", "error"))
    {
        judo::Element iq("iq");
        iq.putAttrib("type", "get");
        std::string id = _session.getNextID();
        iq.putAttrib("id", id);
        iq.putAttrib("to", e.getAttrib("from"));
        judo::Element* item_query = iq.addElement("item");
        item_query->putAttrib("xmlns", "jabber:iq:browse");

        _session.registerIQ(id, SigC::slot(*this, &DiscoDB::browseCB));

        jabberoo::Packet pkt(iq);
        _session << pkt;

        return;
    }

    std::string jid = e.getAttrib("from");

    DiscoDB::Item* item = NULL;
    const judo::Element* query = e.findElement("query");
    // If we are working on a specific node we need to pull it out
    std::string node = query ? query->getAttrib("node") : "";
	if (_persistNodes.find(e.getAttrib("id")) != _persistNodes.end()) {
		if (node.empty())
			node = _persistNodes[e.getAttrib("id")];
		_persistNodes.erase(e.getAttrib("id"));
	}

	item = find(jid, node);
	if (!item) {
        item = new DiscoDB::Item(jid);
		item->setNode(node);
	    _items.insert(ItemMap::value_type(jid, item));
	} else {
		// Clean up info
		item->_features.clear();
		while(!item->_identities.empty())
		{
			DiscoDB::Item::Identity* ident = item->_identities.front();
			delete ident;
			item->_identities.pop_front();
		}

	}

    if (query)
    {
        // Now we can add the identities and features
        for (judo::Element::const_iterator it = query->begin(); 
             it != query->end(); ++it)
        {
            if ((*it)->getType() == judo::Node::ntElement)
            {
                judo::Element* elem = static_cast<judo::Element*>((*it));
                std::string name = elem->getName();
                if (name == "feature")
                {
                    std::string feature = elem->getAttrib("var");
                    if (!feature.empty())
                        item->addFeature(feature);
                }
                else if (name == "identity")
                {
                    item->addIdentity(*elem);
                }
            }
        }
    }
	item->setState((Item::State)((item->getState() & Item::stReady) | Item::stInfo));
    runCallbacks(jid, node, item);
}

void DiscoDB::discoItemsCB(const judo::Element& e)
{
    // Catch errors and see if we can browse instead
    if (e.cmpAttrib("type", "error"))
    {
        judo::Element iq("iq");
        iq.putAttrib("type", "get");
        std::string id = _session.getNextID();
        iq.putAttrib("id", id);
        iq.putAttrib("to", e.getAttrib("from"));
        judo::Element* item_query = iq.addElement("item");
        item_query->putAttrib("xmlns", "jabber:iq:browse");

        _session.registerIQ(id, SigC::slot(*this, &DiscoDB::browseCB));

        jabberoo::Packet pkt(iq);
        _session << pkt;

        return;
    }

    std::string type = e.getAttrib("type");
    std::string jid = e.getAttrib("from");

    DiscoDB::Item *item = NULL;
    const judo::Element* query = e.findElement("query");
    std::string node = query ? query->getAttrib("node") : "";
	if (_persistNodes.find(e.getAttrib("id")) != _persistNodes.end()) {
		if (node.empty())
			node = _persistNodes[e.getAttrib("id")];
		_persistNodes.erase(e.getAttrib("id"));
	}

    // Make our new item to insert
	item = find(jid, node);
	if (!item) {
		item = new DiscoDB::Item(jid);
		item->setNode(node);
	    _items.insert(ItemMap::value_type(jid, item));
	} else {
		// clean up children
		item->_children.clear();
	}

	/* We shouldn't delete other nodes belonging to same JID! Only update the current one...

    // Clean up old entries since we are now the newest version
    std::pair<ItemMap::iterator, ItemMap::iterator> items = _items.equal_range(jid);
    for (ItemMap::iterator it = items.first; it != items.second; ++it)
    {
        delete it->second;
    }
    _items.erase(items.first, items.second);
	
    // They don't have us at all, just pop them in
    _items.insert(ItemMap::value_type(jid, item));
	*/
    
    // Add any children that are there

    if (query)
    {
        for (judo::Element::const_iterator it = query->begin(); 
             it != query->end(); ++it)
        {
            if ( (*it)->getType() != judo::Node::ntElement )
            {
                continue;
            }

            // Build the child and stick it in
            judo::Element* elem = static_cast<judo::Element*>(*it);
            if (elem->getName() == "item")
            {
                std::string cjid = elem->getAttrib("jid");
                std::string cname = elem->getAttrib("name");
                std::string cnode = elem->getAttrib("node");
                DiscoDB::Item* child = find(cjid , cnode);
                if (!child)
                {
                    child = new DiscoDB::Item(cjid);
                    if (!cnode.empty())
                        child->setNode(cnode);
                    _items.insert(ItemMap::value_type(cjid, child));
                }
                else
                {
                    // XXX Do something more here?  Possibly update name?
                }
                if (!cname.empty())
                    child->setName(cname);
                item->appendChild(child);
            }
        }
    }
	item->setState(Item::stItems);
            
    // Now we need the info element
    judo::Element iq("iq");
    iq.putAttrib("type", "get");
    std::string id = _session.getNextID();
    iq.putAttrib("id", id);
    iq.putAttrib("to", jid);
    judo::Element* info_query = iq.addElement("query");
    info_query->putAttrib("xmlns", "http://jabber.org/protocol/disco#info");
	if (!node.empty()) {
        info_query->putAttrib("node", node);
		_persistNodes[id] = node; //RL
	}

    _session.registerIQ(id, SigC::slot(*this, &DiscoDB::discoInfoCB));

    // Send it out
    _session << iq.toString().c_str();
}

void DiscoDB::browseCB(const judo::Element& e)
{
    judo::Element* child = NULL;
    std::string jid;

    // Find the first child element and ensure it has a jid
    for (judo::Element::const_iterator it = e.begin(); it != e.end(); ++it)
    {
        if ( (*it)->getType() == judo::Node::ntElement )
        {
           child = static_cast<judo::Element*>(*it);
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

    // Cache it up
    DiscoDB::Item* item = find(jid);
	if (!item) {
		item = new DiscoDB::Item(jid);
	    _items.insert(ItemMap::value_type(jid, item));
	} else {
		// Clean up info
		item->_features.clear();
		item->_children.clear();
		while(!item->_identities.empty())
		{
			DiscoDB::Item::Identity* ident = item->_identities.front();
			delete ident;
			item->_identities.pop_front();
		}
	}
    // Now we add all the children and sneak in there identities
    judo::Element* celem;
    for (judo::Element::iterator i = child->begin(); i != child->end(); ++i)
    {
        if ( (*i)->getType() != judo::Node::ntElement )
        {
            continue;
        }

        celem = static_cast<judo::Element*>(*i);

        if ( (*i)->getName() == "ns")
        {
            item->addFeature( celem->getCDATA() );
        }
        else
        {
            std::string cjid = celem->getAttrib("jid");
            // This would be fubar anyway
            if (cjid.empty())
                continue;

            // Hopefully it's something else that's usable

			// We should UPDATE if there're some nodes already existing...
			DiscoDB::Item* citem = find(cjid, "");
			if (!citem) {
				citem = new DiscoDB::Item(cjid);
				_items.insert(ItemMap::value_type(cjid, citem));
			} else {
				// Clean up info
				citem->_features.clear();
				citem->_children.clear();
				while(!citem->_identities.empty())
				{
					DiscoDB::Item::Identity* ident = citem->_identities.front();
					delete ident;
					citem->_identities.pop_front();
				}
			}
            citem->setName(celem->getAttrib("name"));
            item->appendChild(citem);
            std::string cat = celem->getAttrib("category");
            // See if it's old skool
            if (cat.empty())
                cat = citem->getName();
            std::string type = celem->getAttrib("type");
            citem->addIdentity(cat, type);
			citem->setState(Item::stReady);
            // See if we have namespaces to fake as features
            for (judo::Element::iterator nsi = celem->begin();
                 nsi != celem->end(); ++nsi)
            {
                if ( ((*nsi)->getType() == judo::Node::ntElement) &&
                     ((*nsi)->getName() == "ns") )
                {
                    judo::Element* ns = static_cast<judo::Element*>(*nsi);
                    citem->addFeature(ns->getCDATA());
                }
            }
        }
    }
	item->setState(Item::stReady);
    runCallbacks(jid, "", item);
}

void DiscoDB::runCallbacks(const std::string& jid, const std::string& node, const DiscoDB::Item* item)
{
    // Tell the world we have a new item
    signal_new_item(*item);

    // See if we have a callback for this lookup
    std::pair<CallbackMap::iterator, CallbackMap::iterator> cis =
		_callbacks.equal_range(node.empty() ? jid : (jid + "::" + node));

    if (cis.first == cis.second)
        return;
	// We have to remove callbacks from list BEFORE signaling them in case
	// if callback registers new callbacks and spoils the map
	std::list <DiscoCallbackFunc> cb;
	// Put found items on the list...
    for(CallbackMap::iterator i = cis.first; i != cis.second; ++i)
		cb.push_back(i->second);
	_callbacks.erase(cis.first , cis.second);
	while (cb.size()) {
		cb.front()(item);
		cb.erase(cb.begin());
	}
	/*
	// XXX this could be a for_each
    for(CallbackMap::iterator i = cis.first; i != cis.second; ++i)
    {
        i->second(item);
    }
	_callbacks.erase(cis.first , cis.second);
	*/

}
} // namespace jabberoo
