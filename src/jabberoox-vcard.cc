#include "stdafx.h"
#include "vCard.h"
#include "XPath.h"


using namespace judo;
using namespace jabberoo;

vCard::vCard(const judo::Element& elem): _elem(elem)
{
    _queries[Firstname] = "/vCard/N/GIVEN";
    _queries[Middlename] = "/vCard/N/MIDDLE";
    _queries[Lastname] = "/vCard/N/FAMILY";
    _queries[Fullname] = "/vCard/FN";
    _queries[Nickname] = "/vCard/NICKNAME";
    _queries[EMail] = "/vCard/EMAIL";
    _queries[Birthday] = "/vCard/BDAY";
    _queries[Telephone] = "/vCard/TEL";
    _queries[URL] = "/vCard/URL";
    _queries[Description] = "/vCard/DESC";
    
    _queries[Street] = "/vCard/ADR/STREET";
    _queries[Street2] = "/vCard/ADR/EXTADD";
    _queries[City] = "/vCard/ADR/LOCALITY";
    _queries[State] = "/vCard/ADR/REGION";
    _queries[PCode] = "/vCard/ADR/PCODE";
    _queries[Country] = "/vCard/ADR/COUNTRY";
    
    _queries[OrgName] = "/vCard/ORG/ORGNAME";
    _queries[OrgUnit] = "/vCard/ORG/ORGUNIT";
    _queries[PersonalTitle] = "/vCard/TITLE";
    _queries[PersonalRole] = "/vCard/ROLE";
}

vCard::~vCard()
{
    for (EntryIter it = _entries.begin(); it != _entries.end(); it++)
    {
        delete it->second;
    }
}

vCard::Entry& vCard::operator[](vCard::Field field)
{
    EntryIter it = _entries.find(field);
    if (it == _entries.end())
    {
        judo::Element* entry_elem = NULL;
        // See if there is an old value in the vCard to proxy
        XPath::Query query(_queries[field]);
        XPath::Value* val = query.execute(&_elem);
        if (!val->check())
        {
            // We need to build it into our node
            const XPath::Query::OpList& ops = query.getOps();
            entry_elem = &_elem;
            for (XPath::Query::OpList::const_iterator it = ops.begin(); 
                 it != ops.end(); ++it)
            {
                if ((*it)->isType(XPath::Op::OP_NODE))
                {
                    std::string name = (*it)->getValue();
                    // This really just gets past the first entry
                    if (name == entry_elem->getName())
                    {
                        continue;
                    }

                    // See if we have that child
                    judo::Element* tmp = entry_elem->findElement(name);
                    if (tmp)
                    {
                        entry_elem = tmp;
                    }
                    else
                    {
                        entry_elem = entry_elem->addElement((*it)->getValue());
                    }
                }
            }
        }
        else
        {
            entry_elem = val->getElem();
        }

        Entry* entry = new Entry(*entry_elem);
        _entries[field] = entry;
        return *entry;
    }
    else
    {
        return *_entries[field];
    }
}

const judo::Element& vCard::getBaseElement()
{
     for(EntryIter it = _entries.begin(); it != _entries.end(); ++it)
     {
          it->second->update();
     }
     
     return _elem;
}

std::string vCard::toString(void)
{
    for(EntryIter it = _entries.begin(); it != _entries.end(); ++it)
    {
        it->second->update();
    }

    return _elem.toString();
}

