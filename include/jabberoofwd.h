// jaberoofwd.h
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

#ifndef JABBEROOFWD_H
#define JABBEROOFWD_H

namespace SigC {
    template <typename, typename> class Slot1;
    template <typename, typename> class Signal0;
    template <typename, typename, typename, typename, typename> class Signal3;
    template <typename> class Marshal;
}

namespace judo {
    class Element;
}

namespace jutil {
    class CaseInsensitiveCmp;
}

namespace jabberoo {
    class Message;
    class Presence;
    class Roster;
    class Session;
    class DiscoDB;
    class PresenceDB;
    typedef SigC::Slot1<void, const ::judo::Element&> ElementCallbackFunc;
    const int ERR_UNAUTHORIZED = 401;
}

#endif // #define JABBEROOFWD_H
