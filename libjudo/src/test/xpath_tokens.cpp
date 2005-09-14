#include <iostream>
#include <list>

#include "judo.hpp"
#include "XPath.h"

using namespace std;
using namespace judo;
using namespace judo::XPath;

int main(int argc, char **argv)
{
    judo::Element doc("doc");
    judo::Element* elem = new judo::Element("test");
    elem->putAttrib("attr", "valu");
    elem->putAttrib("attr2", "valu2");
    elem->addElement("foo","bar1");
    doc.appendChild(elem);
    elem = new judo::Element("test2");
    judo::Element* child = new judo::Element("child");
    elem->appendChild(child);
    doc.appendChild(elem);
    elem = new judo::Element("test");
    doc.appendChild(elem);
    elem = new judo::Element("test2");
    elem->putAttrib("key", "value");
    child = new judo::Element("child");
    elem->appendChild(child);
    doc.appendChild(elem);
    elem = new judo::Element("test");
    elem->putAttrib("attr", "valu-test");
    elem->putAttrib("attr2", "valu3");
    elem->putAttrib("attr3", "something");
    elem->addElement("foo","bar2");
    elem->addElement("foo","bar3");
    elem->addElement("sub", "test-child");
    doc.appendChild(elem);
    elem = doc.addElement("test2");
    elem->putAttrib("attr", "valu-test2");


    if (argc >= 3)
        cout << doc.toString() << endl;

    try
    {
        XPath::Query query = XPath::Query(argv[1]);
        XPath::Value* result = query.execute(&doc);
        if (argc >= 3)
        {
            if (!result->check())
                cout << "No match" << endl;
            else
            {
                cout << "Yay, it matched" << endl;
                XPath::Value::AttribMap attribs = result->getAttribs();
                XPath::Value::ValueList values = result->getValues();
                XPath::Value::ElemList elems = result->getList();
                cout << "And the results are: " << endl;

                if (!attribs.empty())
                {
                    XPath::Value::AttribMap::iterator it = attribs.begin();
                    while(it != attribs.end())
                    {
                        cout << "attrib: " << it->first << " = '" << it->second << "'" << endl;
                        it++;
                    }


                }
                else if (!values.empty())
                {
                    XPath::Value::ValueList::iterator it = values.begin();
                    while( it != values.end() )
                    {
                        std::string val = *it;
                        cout << "value: " << val << endl;
                        it++;
                    }
                }
                else
                {
                    XPath::Value::ElemList::iterator it = elems.begin();
                    while( it != elems.end() )
                    {
                        judo::Element* elem = *it;
                        cout << "elem: " << elem->toString() << endl;
                        it++;
                    }
                }
            }
        }
    }
    catch(XPath::Query::Invalid)
    {
        cout << "Unable to create query" << endl;
    };
}

