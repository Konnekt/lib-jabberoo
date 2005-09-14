/* jabberoo.hh
 * Jabber client library
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
 * 2002-03-05       IBM Corp.       Updated to libjudo 1.1.5
 * 2002-07-09       IBM Corp.       Added Roster::getSession()
 */

#ifndef INCL_JABBEROO_H
#define INCL_JABBEROO_H

#ifdef WIN32
#pragma warning (disable:4786)
#pragma warning (disable:4503)
#pragma warning (disable:4355)
#include <windows.h>
#define snprintf _snprintf
#define strcasecmp _stricmp
#include <string>
using namespace std;
#endif

#include <jabberoofwd.h>
#include <jabberoo-component.hh>
#include <jabberoox.hh>
#include <JID.hh>
#include <jutil.hh>
#include <message.hh>
#include <presence.hh>
#include <session.hh>
#include <XCP.hh>
#include <vCard.h>
#include <sha.h>

#endif
