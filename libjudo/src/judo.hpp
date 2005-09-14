//============================================================================
// Project:       Jabber Universal Document Objects (Judo)
// Filename:      judo.h
// Description:   Primary header
// Created at:    Mon Jul  2 17:27:04 2001
// Modified at:   Tue Oct 16 10:28:40 2001
// 
//   License:
// 
// The contents of this file are subject to the Jabber Open Source License
// Version 1.0 (the "License").  You may not copy or use this file, in either
// source code or executable form, except in compliance with the License.  You
// may obtain a copy of the License at http://www.jabber.com/license/ or at
// http://www.opensource.org/.  
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the License
// for the specific language governing rights and limitations under the
// License.
//
//   Copyrights
//
// Portions created by or assigned to Jabber.com, Inc. are 
// Copyright (c) 1999-2001 Jabber.com, Inc.  All Rights Reserved.  
// 
// $Id: judo.hpp,v 1.10 2004/06/02 00:36:58 temas Exp $
//============================================================================

#ifndef INCL_JUDO_H
#define INCL_JUDO_H

#if !defined(_LIB)
	#if defined(JUDO_EXPORTS)
		#define JUDO_API __declspec(dllexport)
	#else
		#define JUDO_API __declspec(dllimport)
	#endif
#else
	#define JUDO_API __declspec(dllexport)
#endif

#ifdef WIN32
#pragma warning (disable:4786)
#include <windows.h>
#endif

#include <assert.h>

#include <cstdio>
#include <cstring>
#include <list>
#include <map>
#include <string>
#include <set>
#include <algorithm>

#include "expat.h"

#ifdef TEST
#define TESTER(s) friend class s;
#else
#define TESTER(s)
#endif

/**
   Core namespace for Jabber Universal Document Objects
*/
namespace judo
{
    class XMLAccumulator;

    /**
       Parent class for all XML objects
    */
    class Node
    {
    public:
        enum Type
        {
            ntElement,
            ntCDATA
        };

        /**
           Primary constructor
           @param name Name of this element
           @param type Node type classifier
        */
        JUDO_API Node(const std::string& name, Type ntype)
	    : _name(name), _type(ntype)
	    {}
        JUDO_API virtual ~Node() {}
    public:
        /**
           Accessor for the nodes name
           @return Reference to the name of the node
        */
        JUDO_API const std::string& getName() const
	    { return _name; }

        /**
           Accessor for the Nodes type
           @return The type of the Node (Element CDATA)
        */
        JUDO_API Node::Type getType() const
	    { return _type; }

        /**
           Accessor for the XML string representation of the
           Node. This is a virtual abstract method that is
           implemented by the child classes. 
           @return The XML representation of an Node
        */
        JUDO_API virtual std::string toString() const = 0;

	/**
	   Accumulator entry point for recursive XML
	   serialization. This is a virtual abstract method that is
	   implemented by subclasses.
	   @param acc An instance of an XMLAccumulator
	*/
	JUDO_API virtual void accumulate(XMLAccumulator& acc) const = 0;

    protected:
        std::string        _name;
        Node::Type _type;    

        // Ensure that no one can initialize this variable without
        // using the proper constructor
        Node();
    };

    // Utility routines
    JUDO_API void   unescape(const char* src, unsigned int srcLen, std::string& dest, bool append = false);  
    JUDO_API std::string escape(const std::string& src);

    class XMLAccumulator
    {
    public:
	JUDO_API XMLAccumulator(std::string& s)
	    : _result(s) {}

	JUDO_API void operator()(const Node* n)
	    { n->accumulate(*this); }
	JUDO_API void operator()(const std::pair<const std::string, std::string> p)
	    { _result += " " + p.first + "='" + 
		  escape(p.second) + "'";
	    }
	template <class T>
	XMLAccumulator& operator<<(T data)
	    { _result += data; return *this; }
    private:
	std::string& _result;
    };

    /**
       Character data storage class
    */
    class CDATA : 
        public Node
    {
    public:
        /**
           Default constructor.
        */
        JUDO_API CDATA(const char* text, unsigned int textsz, bool escaped = false)
            : Node("#CDATA", Node::ntCDATA)
	    {
		if (escaped)
		{
		    unescape(text, textsz, _text);
		}
		else
		{
		    _text.assign(text, textsz);
		}
	    }
	
        /**
           Overwrite all existing character data in this object with
           specified text
           @param text New character data to store
           @param textsz Length of new character data
	   @param escaped Is the text escaped?
        */
        JUDO_API void setText(const char* text, unsigned int textsz, bool escaped = false)
	    { 
		if (escaped)
		{
		    unescape(text, textsz, _text); 
		}
		else
		{
		    _text.assign(text, textsz);
		}
	    }

        /**
           Append new character data to the existing text in this
           object
           @param text New character data to store
           @param textsz Length of new character data
	   @param escaped Is the text escaped?
        */
        JUDO_API void appendText(const char* text, unsigned int textsz, bool escaped = false)
	    { 
		if (escaped)
		{
		    unescape(text, textsz, _text, true); 
		}
		else
		{
		    _text.append(text, textsz);
		}
	    }

        /**
           Get a read-only reference to the character data in
           this object. Note that this data is not properly escaped
           XML and is not suitable for passing to an XML parser.
        */
        JUDO_API const std::string& getText() const
            { return _text; }

        /**
           Get a properly escaped XML string representation of this
           object.
           @see Node::toString
        */
        JUDO_API std::string toString() const
            { return escape(_text); }

	JUDO_API void accumulate(XMLAccumulator& acc) const
	    { acc << escape(_text); }

    private:
	TESTER(CDATATest)

        std::string _text;           
    };

    /** 
        XML Element representation class
    */
    class Element: 
        public Node
    {
    public:
        JUDO_API Element(const std::string& name, const char** attribs = NULL);
	JUDO_API Element(const Element& e);
        JUDO_API ~Element();

        JUDO_API Element& operator=(const Element& e);

        JUDO_API Element* addElement(const std::string& name, const char** attribs = NULL);
	JUDO_API Element* addElement(const std::string& name, const std::string& cdata, 
			    bool escaped = false);
        JUDO_API CDATA*   addCDATA(const char* data, int datasz, bool escaped = false);	


        JUDO_API bool hasAttrib(const std::string& name) const;
        JUDO_API void   putAttrib(const std::string& name, const std::string& value);
        JUDO_API std::string getAttrib(const std::string& name) const;
        JUDO_API void   delAttrib(const std::string& name);
        JUDO_API bool   cmpAttrib(const std::string& name, const std::string& value) const;
        JUDO_API std::map<std::string,std::string> getAttribs() const { return _attribs; }

        JUDO_API std::string toString() const;
	JUDO_API std::string toStringEx(bool recursive = false, bool closetag = false) const;

	JUDO_API void accumulate(XMLAccumulator& acc) const;

	JUDO_API std::string getCDATA() const;
	

	typedef std::list<Node*>::iterator iterator;
	typedef std::list<Node*>::const_iterator const_iterator;
	typedef std::list<Node*>::reverse_iterator reverse_iterator;
	typedef std::list<Node*>::const_reverse_iterator const_reverse_iterator;

	/**
	   Append child node to this Element
	   @param child child node pointer
	*/
	JUDO_API void appendChild(Node* child)
	    { _children.push_back(child); }
        
        Node* detachChild(iterator it);
        
	/** 
	    Determine if this Element is empty (i.e. has no child nodes)
	    @returns True if no child nodes exist
	*/
	JUDO_API bool empty() const
	    { return _children.empty(); }

    /**
        Remove all children elements.
    */
    void clear()
    { _children.clear(); }

	/**
	   Determine the number of child nodes this Element
	   has. Prefer @Element::empty to comparing size() == 0
	   @returns Number of child nodes in this Element
	*/
	JUDO_API int size() const
	    { return _children.size(); }
	
	/**
	   Return an iterator to the first child Node.
	*/
	JUDO_API iterator begin()
	    { return _children.begin(); }

	/**
	   Return a const iterator to the first child Node.
	*/
	JUDO_API const_iterator begin() const
	    { return _children.begin(); }

    reverse_iterator rbegin()
    { return _children.rbegin(); }

    const_reverse_iterator rbegin() const
    { return _children.rbegin(); }

	/**
	   Return a iterator indicating the end of child nodes.
	*/
	JUDO_API iterator end()
	    { return _children.end(); }

	/**
	   Return a const iterator indicating the end of child nodes.
	*/
	JUDO_API const_iterator end() const
	    { return _children.end(); }

	JUDO_API reverse_iterator rend()
	    { return _children.rend(); }

	JUDO_API const_reverse_iterator rend() const
	    { return _children.rend(); }

	JUDO_API iterator find(const std::string& name, Node::Type type = Node::ntElement);

        JUDO_API const_iterator find(const std::string& name, Node::Type type = Node::ntElement) const;
	/**
	   Delete the child Node designated by the supplied iterator.
	   @param it Iterator pointing to the child Node 
	*/
	JUDO_API void Element::erase(Element::iterator it)
	    { delete *it; _children.erase(it); }

	JUDO_API Element* findElement(const std::string& name);
	JUDO_API const Element* findElement(const std::string& name) const;
	JUDO_API void     eraseElement(const std::string& name);

	JUDO_API std::string getChildCData(const std::string& name) const;
	JUDO_API int         getChildCDataAsInt(const std::string& name, int defaultvalue) const;


    protected:
	TESTER(ElementTest)
	
        std::list<Node*>        _children;
        std::map<std::string,std::string> _attribs;
    };
    
    /**
       XML parser event interface
    */
    class ElementStreamEventListener
    {
    public:
	JUDO_API virtual ~ElementStreamEventListener() {}
	/**
	   Event point for XML document start event. Override this method
	   to get notified when the XML document is starting
	   @param e Root document element. Be sure to delete this once you
	   are done with it
	*/
	JUDO_API virtual void onDocumentStart(Element* e) = 0;

	/**
	   Event point for immediate children of XML document
	   root. Override this method to get notified when a complete
	   child Element has been parsed.
	   @param e Child element. Be sure to delete this once you are
	   done with it
	*/
	JUDO_API virtual void onElement(Element* e) = 0;

	/**
	   Event point for immediate CDATA of an XML document
	   root. Override this method to get notified when CDATA is
	   encountered immediately below the root document. This is a
	   fairly rare condition within most Jabber processing, but is
	   required for those rare cases. If you don't override this
	   method, the CDATA is automatically cleaned up for you.
	   @param c CDATA node. Be sure to delete this once you are
	   done with it.
	*/
	JUDO_API virtual void onCDATA(CDATA* c) 
	    { delete c; }
	    

	/**
	   Event point for XML document end event. Override this
	   method to get notified when the XML document is ending
	*/
	JUDO_API virtual void onDocumentEnd() = 0;
    };
    

    /**
       XML parser wrapper class
    */
    class ElementStream 
    {
    public:
	JUDO_API ElementStream(ElementStreamEventListener* l);
	JUDO_API virtual ~ElementStream();

    JUDO_API void push(const std::string& data)
    { push(data.c_str(), data.size()); }
	JUDO_API void push(const char* data, int datasz);
	JUDO_API void reset();

	JUDO_API static Element* parseAtOnce(const char* buffer);

	struct exception
	{
	    JUDO_API class ParserError 
		{
		public:
		    ParserError(int code)
			: _code(code), _message(XML_ErrorString((XML_Error)code))
			{}
		    const std::string& getMessage() const
			{ return _message; }
		    int getCode() const
			{ return _code; }
		private:
		    int _code;
		    std::string _message;
		};
	    class IncompleteParse{};
	};

    private:
	TESTER(ElementStreamTest)

	XML_Parser     _parser;
	bool           _parser_ready;
	std::list<Element*> _element_stack;
	bool           _document_started;
	bool           _document_ended;

	ElementStreamEventListener* _event_listener;

	// Expat callbacks
	static void onStartElement(void* userdata, const char* name, const char** attribs);
	static void onEndElement(void* userdata, const char* name);
	static void onCDATA(void* userdata, const char* cdata, int cdatasz);

	// Expat initializers
	void initExpat();
	void cleanupExpat();
       };
};
#endif
