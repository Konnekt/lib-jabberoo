// session.hh
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

#ifndef SESSION_HH
#define SESSION_HH

#include <judo.hpp>
#include <sigc++/object.h>
#include <sigc++/signal.h>
#include <sigc++/slot.h>
// #include <jabberoofwd.h>
#include <discoDB.hh>
#include <roster.hh>
#include <presenceDB.hh>
#include "jabberoo_conf.h"
namespace jabberoo {

     typedef SigC::Slot1<void, const judo::Element&> ElementCallbackFunc;

     /**
      * A session with a Jabber server.
      * This class provides common operations needed for raw communication between the client and the server.
      */
     class Session : 
        public judo::ElementStreamEventListener, 
        public judo::ElementStream, 
        public SigC::Object
	  {
	  public:
	       /**
		* The authorization type.
		*/
	       enum AuthType
	       { 
		    atPlaintextAuth, /**< Send the password as plaintext (plaintext). */
		    atDigestAuth,    /**< Send a hash of the password (digest). */
		    atAutoAuth,      /**< Automatically determine authorization type, defaulting to the most secure. */
		    at0kAuth         /**< Do not send any password information over the wire (zero knowledge). */
	       };

	       /**
		* The connection state.
		*/
	       enum ConnectionState
	       {
		    csNotConnected, /**< Not connected. */
		    csCreateUser,   /**< Create a new user. */
		    csAuthReq,      /**< Auhtorization request. */
		    csAwaitingAuth, /**< Awaiting authorization. */
		    csConnected     /**< Connected. */
	       };

	       // Initializers
	       /**
		* Construct a Session.
		* This constructs a Session.
		* @see push();
		* @see evtTransmitXML
		*/
	       JOO_API Session();
	       /**
		* Deconstruct a Session.
		* This frees memory the Session was using.
		*/
	       JOO_API virtual ~Session();

	       // Helper ops
	       /**
		* Read from the Session.
		* Reads data from the session into a const char* buffer.
		*/
	       JOO_API Session& operator>>(const char* buffer) { push(buffer, strlen(buffer)); return *this; } 
	       /**
		* Send a Packet.
		* Sends a Packet through the session.
		*/
	       JOO_API Session& operator<<(const Packet& p);
	       /**
		* Send text.
		* Sends raw text through the session. Usually, this should be well-formed XML.
		*/
	       JOO_API Session& operator<<(const char* buffer) { evtTransmitXML(buffer); return *this;}
	  public:
	       // Connectivity ops
	       /**
		* Log into a server and authenticate.
		* If should_auth is set to false, you will have to use roster().fetch() manually
		* to get Jabberoo to download the roster. The assumption is that if you don't want 
		* standard authentication, you probably don't want the standard roster either. 
		* Yes, that's one heck of an assumption, but in practice it seems to match up.
		* @see AuthType
		* @param server The server to connect to.
		* @param atype The authentication type.
		* @param username The username to use.
		* @param resource The resource of this session.
		* @param password The password for this username.
		* @param createuser Whether to attempt to create a new user.
		* @param should_auth Whether to actually authenticate using jabber:iq:auth.
		*/
	       JOO_API void connect(const std::string& server, AuthType atype, 
			    const std::string& username, const std::string& resource, const std::string& password, 
			    bool createuser = false, bool should_auth = true);

	       /**
		* Disconnect from the server.
		* @return Whether </stream:stream> was actually sent.
		*/
	       JOO_API bool disconnect();

	       /**
		* Get the ConnectionState.
		* @return The current ConnectionState.
		*/
	       JOO_API ConnectionState getConnState() { return _ConnState; }

	       // Misc ops
	       /**
		* Push raw XML to the session.
		* The socket connector should call this function to push raw XML through to Jabberoo.
		* @param data Character data to give to the session.
		* @param datasz Size of the character data to give to the session.
		*/
	       JOO_API virtual void push(const char* data, int datasz);

	       /**
		* Register an iq callback.
		* The callback will be called once an iq message with the given id is received.
		* @param id The id of the iq message which was sent.
		* @param f The function to call.
		*/
	       JOO_API void registerIQ(const std::string& id, ElementCallbackFunc f);

	       /**
		* Unregister an iq callback.
		* @param id The id of the iq message which was sent.
		*/
	       JOO_API void unregisterIQ(const std::string& id);

		   /**
        * Register a judo::XPath callback
        * The callback will be called with any packets that match the given judo::XPath query.
        * @param query The judo::xpath query to search on
        * @param f the function to call
        * @param incoming When true, default, it is on the incoming data, otherwise it is on the outgoing data
        * @return The id of the registered judo::XPath
        */
           JOO_API judo::XPath::Query* registerXPath(const std::string& query,
               ElementCallbackFunc f, bool incoming=true);

        /**
        * Unregister a judo::XPath callback
        * 
        * @param id The id to remove
        */
        JOO_API void unregisterXPath(judo::XPath::Query* id, bool incoming=true);

	       /**
		* Query a namespace on a specific JabberID.
		* The callback will be called once a response to the namespace query is received.
		* @param nspace The namespace to query.
		* @param f The function to call.
		* @param to The JabberID to query. If empty, it queries the server this Session is connected to.
		*/
	       JOO_API void queryNamespace(const std::string& nspace, ElementCallbackFunc f, const std::string& to = "");

	       // Structure accessors
	       /**
		* Get the Roster.
		* @see Roster
		* @return The Roster.
		*/
	       JOO_API const Roster& roster() const;
	       /**
		* Get the Roster.
		* @see Roster
		* @return The Roster.
		*/
	       JOO_API Roster&       roster();

        /**
        * Get the DiscoDB
        * @see DiscoDB
        * @return the DiscoDB
        */
           JOO_API const DiscoDB&    discoDB() const;

        /**
        * Get the DiscoDB
        * @see DiscoDB
        * @return the DiscoDB
        */
           JOO_API DiscoDB&    discoDB();

	       /**
		* Get the PresenceDB.
		* @see PresenceDB
		* @return The PresenceDB.
		*/
	       JOO_API const PresenceDB& presenceDB() const;
	       /**
		* Get the PresenceDB.
		* @see PresenceDB
		* @return The PresenceDB.
		*/
	       JOO_API PresenceDB&       presenceDB();

	       // Property accessors
	       /**
		* Get the AuthType which was used.
		* @return The AuthType which was used to connect.
		*/
	       JOO_API AuthType      getAuthType() const;
	       /**
		* Get the username which was used.
		* @return The username which was used to connect.
		*/
	       JOO_API const std::string& getUserName() const;

        /** Retrieve the local jid used to connect with. */
        JOO_API const std::string& getLocalJID() const
        { return _local_jid; }

                // ID/Auth ops
	       /**
		* Get the next available id
		* Using this function ensures that ids are not repeated in the same Session.
		* @return A std::string containing a valid id.
		*/
	       JOO_API std::string getNextID();
	       /**
		* Get the digest hash.
		* @return The hash of the password to be used for authentication.
		*/
	       JOO_API std::string getDigest();
        /**
         * Add a feature to be displayed in disco replies
         * @param feature The feature string to add
         */
        JOO_API void addFeature(const std::string& feature);

        /**
        * Dispatches the packet internally and then deletes it.
        * @param elem The element to dispatch
        */
        JOO_API void dispatch(judo::Element* elem);
	  public:
	       // XML transmission signals
	       /**
		* This event is emitted when there is an error parsing the XML received.
		* Jabberoo catches libjudo's ParserError exception, disconnects the session,
		* and then triggers this event.
		* @param error_code The error code as defined by libjudo
		* @param error_msg The error message as defined by libjudo
		* @see XML_Error
		* @see push
		*/
           SigC::Signal2<void, int, const std::string&, SigC::Marshal<void> > evtXMLParserError;
	       /**
		* This event is emitted when XML is transmitted.
		* This should be hooked up to a function in the socket connector which sends the character data.
		* @param XML The XML which is being sent.
		*/
           SigC::Signal1<void, const char*, SigC::Marshal<void> >        evtTransmitXML;
	       /**
		* This event is emitted when XML is received.
		* @param XML The XML which was received.
		*/
           SigC::Signal1<void, const char*, SigC::Marshal<void> >        evtRecvXML;
	       /**
		* This event is emitted when a Packet is transmitted.
		* @param p The Packet which is being sent.
		*/
           SigC::Signal1<void, const Packet&, SigC::Marshal<void> >      evtTransmitPacket;
	       // Connectivity signals
	       /**
		* This event is emitted when the connection to the server has been established.
		* @see Session::connect()
		* @param t The stream header judo::element which was used immediately after connection.
		*/
           SigC::Signal1<void, const judo::Element&, SigC::Marshal<void> >         evtConnected;
	       /**
		* This event is emitted when the connection to the server has been discontinued.
		* This will be emitted whether the connection was due to error or not.
		* @see Session::disconnect()
		*/
           SigC::Signal0<void, SigC::Marshal<void> >                     evtDisconnected;
	       // Basic signals
	       /**
		* This event is emitted when a Message is received.
		* @see jabberoo::Message
		* @param m The Message.
		*/
           SigC::Signal1<void, const Message&, SigC::Marshal<void> >     evtMessage;
	       /**
		* This event is emitted when an IQ judo::element is received.
		* @see judo::Element
		* @param iq The iq judo::element's judo::Element.
		*/ 
           SigC::Signal1<void, const judo::Element&, SigC::Marshal<void> >         evtIQ;
	       /**
		* This event is emitted when a Presence judo::element which appears to be from the server 
		* is received. This probably is an error, such as specifying some invalid type.
		* @see jabberoo::Presence
		* @param p The Presence.
		*/
           SigC::Signal1<void, const Presence&, SigC::Marshal<void> >    evtMyPresence;
	       /**
		* This event is emitted when a Presence judo::element is received.
		* @see jabberoo::Presence
		* @param p The Presence.
		* @param previous_type The Presence::Type of the previous Presence received from this user.
		*/
           SigC::Signal2<void, const Presence&, Presence::Type, SigC::Marshal<void> >    evtPresence;
	       /**
		* This event is emitted when a Presence subscription request is received.
		* @see jabberoo::Presence
		* @param p The Presence subscription request.
		*/
           SigC::Signal1<void, const Presence&, SigC::Marshal<void> >    evtPresenceRequest;
	       /**
		* This event is emitted when a Presence subscription is rejected.
		* This can happen when asking for a subscription or just any old time.
		* @param p The Presence unsubscribed packet.
		*/
	       SigC::Signal1<void, const Presence&, SigC::Marshal<void> > evtPresenceUnsubscribed;
	       // Misc signals
	       /**
		* This event is emitted when an unknown XML judo::element is received.
		* @see judo::Element
		* @param t The judo::element's judo::Element
		*/
           SigC::Signal1<void, const judo::Element&, SigC::Marshal<void> >         evtUnknownPacket;
	       /**
		* This event is emitted when an authorization error occurs.
		* @param errorcode The error code.
		* @param errormsg The error message.
		*/
           SigC::Signal2<void, int, const char*, SigC::Marshal<void> >   evtAuthError;	  
	       /**
		* This event is emitted when a roster is received.
		* Note that this could be simply a roster push or the complete roster.
		* The only reason you'd attach to this is if you want notification
		* of *all* roster changes, whether or not you need to refresh the roster.
		* It probably makes more sense to use jabberoo::Roster::evtRefresh
		* @see jabberoo::Roster::evtRefresh
		*/
		   /*RL: evtOnRoster with 'query' element*/
           SigC::Signal1<void, const judo::Element&, SigC::Marshal<void> > evtOnRoster;
	       /**
		* This event is emitted when a client version is requested.
		* @param name The name of the client.
		* @param version The version of the client.
		* @param os The operating system the client is running on.
		*/
           SigC::Signal3<void, std::string&, std::string&, std::string&, SigC::Marshal<void> > evtOnVersion;
	       /**
		* This event is emitted when idle time is requested.
		* @param seconds The number of seconds, as a std::string, of idleness.
		*/
           SigC::Signal1<void, std::string&, SigC::Marshal<void> >            evtOnLast;

	       /**
		* This event is emitted when the client machine's local time is requested.
		* @param localUTF8Time Local time, given in a nice human-readable std::string.
		* @param UTF8TimeZone Client's time zone
		*/
           SigC::Signal2<void, std::string&, std::string&, SigC::Marshal<void> >            evtOnTime;

        /**
        * Event that is fired as soon as an element comes in.
        * This is made to allow for modification before actual
        * processing.
        */
        SigC::Signal1<void, judo::Element&> evtOnRecvElement;

	  protected:
	       // Authentication handler
	       void authenticate();
	       void OnAuthTypeReceived(const judo::Element& t);
	       void sendLogin(Session::AuthType atype, const judo::Element* squery);

	       // ElementStream events
	       virtual void onDocumentStart(judo::Element* t);
	       virtual void onElement(judo::Element* t);
	       virtual void onCDATA(judo::CDATA* c);
	       virtual void onDocumentEnd();

	       // Basic packet handlers
	       void handleMessage(judo::Element& t);
	       void handlePresence(judo::Element& t);
	       void handleIQ(judo::Element& t);

	  private:
	       // Values
	       std::string          _ServerID;
	       std::string          _Username;
	       std::string          _Password;
	       std::string          _Resource;
           std::string          _local_jid;
	       std::string          _SessionID;
	       long            _ID;
	       // Enumeration values (states)
	       AuthType        _AuthType;
	       ConnectionState _ConnState;
 	       // Stream header 
	       judo::Element*            _StreamElement;
	       // Whether or not we have received the starting stream tag
	       bool            _StreamStart;
	       // Whether or not we want to use jabber:iq:auth
	       bool            _Authenticate;
	       // Structures
           std::multimap<std::string, ElementCallbackFunc> _Callbacks;			 /* IQ callback funcs */
           typedef std::list<std::pair<judo::XPath::Query*, ElementCallbackFunc> > XPQueryList;
           struct queryFinder : 
               public std::unary_function<XPQueryList::value_type, bool>
           {
               queryFinder(judo::XPath::Query* key) : _key(key) { }
               bool operator()(const XPQueryList::value_type val) const
               {
                   return val.first == _key;
               }
               judo::XPath::Query* _key;
           };
           XPQueryList _incoming_queries;
           XPQueryList _outgoing_queries;
	       // Internal roster & presence db structures
	       Roster          _Roster;
           DiscoDB         _DDB;
	       PresenceDB      _PDB;

           std::list<std::string> _features;
	       // Internal IQ handlers
	       void IQHandler_Auth(const judo::Element& t);
	       void IQHandler_CreateUser(const judo::Element& t);
           
           // Helper structs
           
           struct PrintFeature
           {
               PrintFeature(judo::Element* query) : _q(query) { }
               void operator()(std::string& feat)
               {
                   _q->addElement("feature")->putAttrib("var", feat);
               }
               judo::Element* _q;
           };
	  };    
} // namespace jabberoo

#endif // SESSION_HH
