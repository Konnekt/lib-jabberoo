#ifndef INCL_JABBEROOX_VCARD
#define INCL_JABBEROOX_VCARD

#include "judo.hpp"

#include <sigc++/object.h>
#include <sigc++/slot.h>
#include "jabberoo_conf.h"
namespace jabberoo
{
    class  vCard : public SigC::Object
    {
    public:
        enum Field
        {
            Firstname,
            Middlename,
            Lastname,
            Fullname,
            Nickname,
	    EMail,
	    Birthday,
	    Telephone,
	    URL,
	    Description,
             Street,
             Street2,
             City,
             State,
             PCode,
             Country,
             OrgName,
             OrgUnit,
             PersonalTitle,
             PersonalRole
        };

        class  Entry
        {
        public:
            Entry(judo::Element& elem): _elem(elem)
            { 
                _data = _elem.getCDATA();
            }
            ~Entry()
            { }

            void update()
            { 
                judo::Element::iterator i = _elem.find("#CDATA", judo::Node::ntCDATA);
                if (i != _elem.end())
                    _elem.erase(i);
                _elem.addCDATA(_data.c_str(), _data.length(), true); 
            }

            // Pretend to be a string
            operator std::string() const
            { return std::string(_data); }
            Entry& operator=(const std::string& val)
            { _data = val; return *this; }

        private:
            judo::Element&  _elem;
            std::string     _data;
        };

	JOO_API vCard(const judo::Element& elem);
       JOO_API  ~vCard();

        JOO_API vCard::Entry& operator[](vCard::Field field);
        
        JOO_API const judo::Element& getBaseElement();
        JOO_API std::string toString(void);
    private:
        judo::Element  _elem;
        typedef std::map<vCard::Field, vCard::Entry*>::iterator EntryIter;
        std::map<vCard::Field, vCard::Entry*> _entries;
        typedef std::map<vCard::Field, std::string>::iterator QueryIter;
        std::map<vCard::Field, std::string> _queries;
    };
};

#endif // INCL_JABBEROOX_VCARD
