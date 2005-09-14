#ifndef INCL_JUDO_XPATH_FUNCTIONS_H
#define INCL_JUDO_XPATH_FUNCTIONS_H

namespace judo {
namespace XPath {

    struct TextFunction : public XPath::Function
    {
        bool run(Value* ctxt, Op::OpList& args)
        {
            Value::ElemList& elems = ctxt->getList();
            Value::ElemList::iterator it = elems.begin();

            Value::ValueList values;

            while (it != elems.end())
            {
                std::string val;
                Value::ElemList::iterator next = it;
                ++next;

                judo::Element* elem = *it;
                val = value(elem, args);
                values.push_back(val);

                it = next;
            }

            if (elems.empty())
            {
                return false;
            }

            ctxt->setValues(values);

            return true;
        }
            
        std::string value(judo::Element* elem, Op::OpList& args)
        {
            return elem->getCDATA();
        }
    };
            
    struct NameFunction : public XPath::Function
    {
        bool run(XPath::Value* ctxt, Op::OpList& args)
        {
            Value::ElemList& elems = ctxt->getList();
            Value::ElemList::iterator it = elems.begin();

            Value::ValueList values;

            while (it != elems.end())
            {
                std::string val;
                Value::ElemList::iterator next = it;
                ++next;

                judo::Element* elem = *it;
                val = value(elem, args);
                values.push_back(val);

                it = next;
            }

            if (elems.empty())
            {
                return false;
            }

            ctxt->setValues(values);

            return true;
        }
        
        std::string value(judo::Element* elem, Op::OpList& args)
        {
            return elem->getName();
        }
    };

    struct NotFunction : public XPath::Function
    {
        bool run(XPath::Value* ctxt, Op::OpList& args)
        {
            Value::ElemList& elems = ctxt->getList();
            Value::ElemList::iterator it = elems.begin();

            while (it != elems.end())
            {
                std::string value;
                Value::ElemList::iterator next = it;
                ++next;

                Value* tmp_ctxt = new Value(*it);
                tmp_ctxt->in_context(true);

                if (args[0]->isValid(tmp_ctxt))
                {
                    elems.erase(it);
                }

                it = next;
            }

            if (elems.empty())
            {
                return false;
            }

            return true;
        }
    };
        
    struct StartsWithFunction : public XPath::Function
    {
        bool run(XPath::Value* ctxt, Op::OpList& args)
        {
            Value::ElemList& elems = ctxt->getList();
            Value::ElemList::iterator it = elems.begin();

            while (it != elems.end())
            {
                std::string value;
                Value::ElemList::iterator next = it;
                ++next;

                judo::Element* elem = *it;
                std::string val1 = args[0]->calcStr(elem);
                std::string val2 = args[1]->calcStr(elem);

                if (val1.find(val2,0) != 0)
                {
                    elems.erase(it);
                }

                it = next;
            }

            if (elems.empty())
            {
                return false;
            }

            return true;
        }
    };
}; // namespace XPath

}; // namespace judo

#endif

