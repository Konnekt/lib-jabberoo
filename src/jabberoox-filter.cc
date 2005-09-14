/* jabberoox-filter.cc
 * Jabber filter support
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
#include "jabberoox.hh"
#include <judo.hpp>

namespace jabberoo
{

const bool Filter::Action::ParamReq[5] = { true, true, true, false, false };
const bool Filter::Condition::ParamReq[7]  = { false, true, true, true, true, true, true };

// ---------------------------------------------------------
//
// Filter::Action methods
//
// ---------------------------------------------------------
std::string Filter::Action::toXML() const
{
     std::string name;
     switch (_value)
     {
     case SetType: 
	  name = "settype"; break;
     case ForwardTo: 
	  name = "forward"; break;
     case ReplyWith:
	  name = "reply"; break;
     case StoreOffline:
	  name = "offline"; break;
     case Continue:
	  name = "continue"; break;
     default:
	  return "";
     }
     if (_param.empty())
	  return "<" + name + "/>";
     else
	  return "<" + name + ">" + judo::escape(_param) + "</" + name + ">";
}

Filter::Action::Value Filter::Action::translate(const std::string& s)
{
     if (s == "settype")
	  return Action::SetType;
     else if (s == "forward")
	  return Action::ForwardTo;
     else if (s == "reply")
	  return Action::ReplyWith;
     else if (s == "offline")
	  return Action::StoreOffline;
     else if (s == "continue")
	  return Action::Continue;
     else
	  return Action::Invalid;
}

// ---------------------------------------------------------
//
// Filter::Condition methods
//
// ---------------------------------------------------------
std::string Filter::Condition::toXML() const
{
     std::string name;
     switch(_value)
     {
     case Unavailable:
	  name = "unavailable"; break;
     case From:
	  name = "from"; break;
     case MyResourceEquals:
	  name = "resource"; break;
     case SubjectEquals:
	  name = "subject"; break;
     case BodyEquals:
	  name = "body"; break;
     case ShowEquals:
	  name = "show"; break;
     case TypeEquals:
	  name = "type"; break;
     default:
	  return "";
     }
     if (_param.empty())
	  return "<" + name + "/>";
     else
	  return "<" + name + ">" + judo::escape(_param) + "</" + name + ">";
}

Filter::Condition::Value Filter::Condition::translate(const std::string& s)
{
     if (s == "unavailable")
	  return Condition::Unavailable;
     else if (s == "from")
	  return Condition::From;
     else if (s == "resource")
	  return Condition::MyResourceEquals;
     else if (s == "subject")
	  return Condition::SubjectEquals;
     else if (s == "body")
	  return Condition::BodyEquals;
     else if (s == "show")
	  return Condition::ShowEquals;
     else if (s == "type")
	  return Condition::TypeEquals;
     else
	  return Condition::Invalid;
}

// ---------------------------------------------------------
//
// Filter methods
//
// ---------------------------------------------------------
Filter::Filter(const std::string& name)
     : _name(name)
{}
/*Filter::Filter()
     : _name("")
{}*/

void dumpFilter(Filter& f)
{
    std::cerr << "Filter(" << f.Name() <<") @ " << &f << "\tActions: " << f.Actions().size() << "\tConditions: " << f.Conditions().size() << std::endl;
}

Filter::Filter(const Filter& f)
     : _name(f._name), _actions(f._actions), _conditions(f._conditions)
{}

Filter::Filter(const judo::Element& rule)
{
     // Retrieve the rule name
     _name = rule.getAttrib("name");
     if (_name == "")
	  _name = "Default rule";

     // Walk the rule's child tags
     judo::Element::const_iterator it = rule.begin();
     for (; it != rule.end(); it++)
     {
	  if ((*it)->getType() != judo::Node::ntElement)
	       continue;
	  // Cast the child judo::element into a tag..
	  judo::Element& t = *static_cast<judo::Element*>(*it);
	  
	  // See if this is an action
	  Action::Value aval = Action::translate(t.getName());
	  if (aval != Action::Invalid)
	  {
	       _actions.push_back(Action(aval, t.getCDATA()));
	  }
	  
	  // See if this is a condition
	  Condition::Value cval = Condition::translate(t.getName());
	  if (cval != Condition::Invalid)
	  {
	       _conditions.push_back(Condition(cval, t.getCDATA()));
	  }
     }
}


std::string Filter::toXML() const
{
     std::string result = "<rule name='" + _name;
     if (!_actions.empty() || !_conditions.empty())
     {
	  result += "'>";
	  for (ActionList::const_iterator it = _actions.begin(); it != _actions.end(); it++)
	       result += it->toXML();
	  for (ConditionList::const_iterator it = _conditions.begin(); it != _conditions.end(); it++)
	       result += it->toXML();
	  return result + "</rule>";
     }
     else
	  return result + "'/>";
}

// ---------------------------------------------------------
//
// FilterList methods
//
// ---------------------------------------------------------
FilterList::FilterList(const judo::Element& query)
{
     // Make sure this is the proper namespace
     if (!query.cmpAttrib("xmlns", "jabber:iq:filter"))
	  return;

     // Walk each of the child <rule> tags and build rule objects
     judo::Element::const_iterator it = query.begin();
     for (; it != query.end(); it++)
     {
	  if ((*it)->getType() != judo::Node::ntElement)
	       continue;
	  // Cast the child judo::element to a tag.
	  judo::Element& t = *static_cast<judo::Element*>(*it);

	  // Ensure this is a <rule> tag
	  if (t.getName() == "rule")
	       // Construct and insert a rule
	       push_back(Filter(t));
     }
}

std::string FilterList::toXML() const
{
     std::string result = "<query xmlns='jabber:iq:filter'";
     if (!empty())
     {
	  result += ">";
	  for (const_iterator it = begin(); it != end(); it++)
	       result += it->toXML();
	  return result + "</query>";
     }
     else
	  return result + "/>";
}

} // namespace jabberoo
