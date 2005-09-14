/* jabberoox.hh
 * Jabber client library extensions
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

#ifndef INCL_JABBEROOX_HH
#define INCL_JABBEROOX_HH

#include <string>
#include <list>
#include <jabberoofwd.h>
#include <sigc++/object.h>
#include <sigc++/signal.h>
#include <boost/shared_ptr.hpp>
#include "jabberoo_conf.h"

//#undef JOO_API
//#define JOO_API

namespace jabberoo
{
     class Filter
     {
     public:
	  class Action
	  {
	  public:
	       enum Value
	       {
		    Invalid = -1, SetType, ForwardTo, ReplyWith, StoreOffline, Continue
	       };
		   JOO_API Action(Value v = SetType, const std::string& param = "")
		    : _value(v), _param(param) {}
	       // Accessors
		   JOO_API bool requires_param()
		    { return Action::ParamReq[_value]; }
		   JOO_API const std::string& param() const 
		    { return _param; }
		   JOO_API Value value() const 
		    { return _value; }
		   JOO_API std::string toXML() const;
	       // Mutators
		   JOO_API Action& operator<<(Value v) 
		    { _value = v; return *this;}
		   JOO_API Action& operator<<(const std::string& param) {
		    _param = param; return *this; }
	       // Translator
	       static JOO_API Value translate(const std::string& s);
	  private:
	       static const bool ParamReq[5];
	       Value  _value;
	       std::string _param;
	  };

	  class Condition
	  {
	  public:
	       enum Value
	       {
		    Invalid = -1, Unavailable, From, MyResourceEquals, SubjectEquals, BodyEquals, ShowEquals, TypeEquals
	       };
	       JOO_API Condition(Value v = Unavailable, const std::string& param = "")
		    : _value(v), _param(param) {}
	       // Accessors
	       JOO_API bool  requires_param() 
		    { return Condition::ParamReq[_value]; }
	       JOO_API const std::string& param() const 
		    { return _param; }
	       JOO_API Value value() const 
		    { return _value; }
	       JOO_API std::string toXML() const;
	       // Mutators
	       JOO_API Condition& operator<<(Value v) 
		    { _value = v; return *this;}
	       JOO_API Condition& operator<<(const std::string& param) 
		    { _param = param; return *this; }
	       // Translator
	       static  JOO_API  Value translate(const std::string& s);
	  private:
	       static const bool ParamReq[7];
	       Value  _value;
	       std::string _param;
	  };

	  typedef std::list<Action> ActionList;
	  typedef std::list<Condition> ConditionList;

     public:
	  // Constructors
	  JOO_API Filter(const std::string& name);
	  JOO_API Filter(const judo::Element& rule);
	  JOO_API Filter(const Filter& f);
	  //Filter();
 	  JOO_API bool operator==(const Filter& f) const { return &f == this; }
 	  JOO_API bool operator==(const Filter& f) { return &f == this; }
	  // XML converter
	  JOO_API std::string toXML() const;
	  // Accessors
	  JOO_API ActionList&    Actions()    { return _actions; }
	  JOO_API ConditionList& Conditions() { return _conditions; }
	  JOO_API const std::string&  Name() const { return _name; }
	  JOO_API void           setName(const std::string& newname) { _name = newname; }
     private:
	  ActionList    _actions;
	  ConditionList _conditions;
	  std::string _name;
     };

     class FilterList
	  : public std::list<Filter>
     {
     public:
	  JOO_API FilterList(const judo::Element& query);
	  JOO_API std::string toXML() const;
     };






/*RL: Klasa nie skompiluje siê w ten sposób, robimy wiêc na shared_ptr */
	 class Agent
//	  : public std::list<Agent>, public SigC::Object
: public std::list<boost::shared_ptr<Agent> >, public SigC::Object
     {
     public:
		 typedef boost::shared_ptr<Agent> agentPtr; //RL
	  JOO_API Agent(const Agent& a);
	  JOO_API Agent(const std::string& jid, const judo::Element& baseElement, Session& s);
	  JOO_API ~Agent();
     public:
	  // Events
      SigC::Signal1<void, bool, SigC::Marshal<void> > evtFetchComplete;
	  // Accessors
	  JOO_API const std::string& JID()         const { return _jid; };
	  JOO_API const std::string& name()        const { return _name; };
	  JOO_API const std::string& description() const { return _desc; };
	  JOO_API const std::string& service()     const { return _service; };
	  JOO_API const std::string& transport()   const { return _transport; };
	  // Info ops
	  JOO_API bool isRegisterable() const { return _registerable; };
	  JOO_API bool isSearchable()   const { return _searchable; };
	  JOO_API bool isGCCapable()    const { return _gc_capable; };
	  JOO_API bool hasAgents()      const { return _subagents; };
	  // Action ops
	  JOO_API void requestRegister(ElementCallbackFunc f);
	  JOO_API void requestSearch(ElementCallbackFunc f);
	  JOO_API void fetch();
     protected:
	  void on_fetch(const judo::Element& iq);
     private:
	  std::string   _jid;
	  std::string   _name;
	  std::string   _desc;
	  std::string   _service;
	  std::string   _transport;
	  bool     _registerable;
	  bool     _searchable;
	  bool     _subagents;
	  bool     _gc_capable;
	  bool     _fetchrequested;
	  Session& _session;
     };

     class AgentList
	  : public std::list<Agent>
     {
     public:
	  JOO_API AgentList(const judo::Element& query, Session& s);
	  JOO_API void load(const judo::Element& query, Session& s);
     };


};

#endif
