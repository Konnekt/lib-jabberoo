#ifndef INCL_XPATH_OPS_H
#define INCL_XPATH_OPS_H

#include <iostream>

namespace judo
{
    namespace XPath
    {
        class JUDO_API PositionOp : public Op
        {
        public:
            PositionOp(int pos) : Op(OP_POSITION, ""), _pos(pos)
            { }

            bool isValid(XPath::Value* ctxt)
            {
                Value::ElemList elems = ctxt->getList();

                Value::ElemList::iterator it = elems.begin();
                for (int x = 1; (x < _pos) && (it != elems.end()); x++) it++;
                if (it == elems.end())
                    return false;
                //std::cout << "Got position " << (*it)->getName() << std::endl;
                ctxt->setElems(*it);

                return true;
            }
        private:
            int _pos;
        };

        class JUDO_API ContextOp : public Op
        {
        public:
            ContextOp(Op* op) : Op(OP_CONTEXT_CONDITION, ""), _op(op)
            { }

            ~ContextOp()
            {
                delete _op;
            }
            
            bool isValid(XPath::Value* ctxt)
            {
                Value::ElemList& elems = ctxt->getList();
                Value::ElemList::iterator it = elems.begin();
                while (it != elems.end())
                {
                    Value::ElemList::iterator next = it;
                    ++next;

                    Value* tmp_ctxt = new Value(*it);
                    tmp_ctxt->in_context(true);

                    if (!_op->isValid(tmp_ctxt))
                    {
                        elems.erase(it);
                    }

                    it = next;
                }

                if (elems.empty())
                    return false;

                return true;
            }

        private:
            Op* _op;
        };
        
        class JUDO_API NodeOp : public Op
        {
        public:
            NodeOp(const std::string& name, bool is_root=false) : 
                Op(Op::OP_NODE, name), _is_root(is_root)
            { }

            std::string calcStr(judo::Element* elem)
            {
                return elem->getCDATA();
            }

            bool isValid(XPath::Value* ctxt)
            {
                judo::Element* elem = NULL;

                // If we're only checking the root node bail early
                if (_is_root)
                {
                    elem = ctxt->getElem();
                    if (elem->getName() != _value)
                        return false;
                    return true;
                }

                // Get children
                Value::ElemList& elems = ctxt->getList();
                Value::ElemList valid_elems;

                if (elems.empty())
                {
                    elem = ctxt->getElem();
                    if (!elem)
                        return false;
                    elems.push_back(elem);
                }

                for (Value::ElemList::iterator it = elems.begin(); 
                        it != elems.end(); it++)
                {
                    bool valid = false;

                    elem = *it;

                    judo::Element::iterator sit = elem->begin();
                    for (; sit != elem->end(); sit++)
                    {
                        if ((*sit)->getType() == Node::ntElement && 
                            (_value == "*") || ((*sit)->getName() == _value))
                        {
                            if (ctxt->in_context())
                                valid = true;
                            else
                                valid_elems.push_back(static_cast<judo::Element*>(*sit));
                        }
                    }
                    if (valid)
                        valid_elems.push_back(static_cast<judo::Element*>(*it));
                }

                // Nothing in the set
                if (valid_elems.empty())
                    return false;

                ctxt->setElems(valid_elems);

                return true;
            }
        private:
            bool _is_root;
        };

        class JUDO_API AllOp : public Op
        {
        public:
            AllOp(const std::string& name) : Op(Op::OP_ALL, name)
            { }

            bool isValid(XPath::Value* ctxt)
            {
                judo::Element* elem = NULL;
                
                // If we're only checking the root node bail early
                if (_is_root)
                {
                    elem = ctxt->getElem();
                    if (elem->getName() != _value)
                        return false;
                    return true;
                }

                // Get children
                Value::ElemList& elems = ctxt->getList();
                Value::ElemList valid_elems;

                if (elems.empty())
                {
                    return false;
                }

                for (Value::ElemList::iterator it = elems.begin(); 
                        it != elems.end(); it++)
                {
                    descend(static_cast<judo::Element*>(*it),&valid_elems);
                }

                // Nothing in the set
                if (valid_elems.empty())
                    return false;

                ctxt->setElems(valid_elems);

                return true;
            }

            void descend(judo::Element* elem, Value::ElemList* valid_elems)
            {
                judo::Element* temp_elem = NULL;
                
                if ((_value == "*") || (elem->getName() == _value))
                {
                    valid_elems->push_back(elem);
                }

                judo::Element::iterator it = elem->begin();
                for (; it != elem->end(); it++)
                {
                    if ((*it)->getType() == Node::ntElement)
                    {
                        temp_elem = static_cast<judo::Element*>(*it);

                        descend(temp_elem,valid_elems);
                    }
                }
            }
            
        private:
            bool _is_root;
        };

        class JUDO_API EqualOp : public Op
        {
        public:
            EqualOp(Op* op_lh, Op* op_rh) : 
                Op(Op::OP_EQUAL, ""), _op_lh(op_lh), _op_rh(op_rh)
            { }

            ~EqualOp()
            {
                delete _op_lh;
                delete _op_rh;
            }

            bool isValid(XPath::Value* ctxt)
            {
                XPath::Value* tmp_ctxt = new Value(ctxt->getList());

                if (!_op_lh->isValid(tmp_ctxt) || !_op_rh->isValid(tmp_ctxt))
                    return false;

                Value::ElemList& elems = tmp_ctxt->getList();
                Value::ElemList::iterator it = elems.begin();

                while (it != elems.end())
                {
                    Value::ElemList::iterator next = it;
                    ++next;

                    judo::Element* elem = *it;
                    if (_op_lh->calcStr(elem) != _op_rh->calcStr(elem))
                    {
                        elems.erase(it);
                    }
                    it = next;
                }

                if (elems.empty())
                {
                    Value::ElemList& elems = ctxt->getList();
                    elems.clear();
                    return false;
                }

                return true;
            }

        private:
            Op* _op_lh;
            Op* _op_rh;
        };

        class JUDO_API NotEqualOp : public Op
        {
        public:
            NotEqualOp(Op* op_lh, Op* op_rh) : 
                Op(Op::OP_NOTEQUAL, ""), _op_lh(op_lh), _op_rh(op_rh)
            { }

            ~NotEqualOp()
            {
                delete _op_lh;
                delete _op_rh;
            }

            bool isValid(XPath::Value* ctxt)
            {
                XPath::Value* tmp_ctxt = new Value(ctxt->getList());

                if (!_op_lh->isValid(tmp_ctxt) || !_op_rh->isValid(tmp_ctxt))
                    return false;

                Value::ElemList& elems = tmp_ctxt->getList();
                Value::ElemList::iterator it = elems.begin();

                while (it != elems.end())
                {
                    Value::ElemList::iterator next = it;
                    ++next;

                    judo::Element* elem = *it;
                    if (_op_lh->calcStr(elem) == _op_rh->calcStr(elem))
                    {
                        elems.erase(it);
                    }
                    it = next;
                }

                if (elems.empty())
                {
                    Value::ElemList& elems = ctxt->getList();
                    elems.clear();
                    return false;
                }

                return true;
            }

        private:
            Op* _op_lh;
            Op* _op_rh;
        };

        class JUDO_API AttributeOp : public Op
        {
        public:
            AttributeOp(const std::string& name) : Op(Op::OP_ATTRIBUTE, name)
            { }

            std::string getValue()
            {
                return _val;
            }

            std::string calcStr(judo::Element* elem)
            {
                return elem->getAttrib(_value);
            }
            
            bool isValid(XPath::Value* ctxt)
            {
                Value::ElemList& elems = ctxt->getList();
                Value::ElemList::iterator it = elems.begin();

                Value::ValueList values;
                Value::AttribMap attribs;

                while (it != elems.end())
                {
                    Value::ElemList::iterator next = it;
                    ++next;

                    judo::Element* elem = *it;
                    if (_value != "*")
                    {
                        _val = calcStr(elem);
                        if (_val.empty())
                            elems.erase(it);
                        else
                            values.push_back(_val);
                    }
                    else
                    {
                        Value::AttribMap temp_attribs = elem->getAttribs();
                        if (temp_attribs.empty())
                        {
                            elems.erase(it);
                        }
                        else
                        {
                            Value::AttribMap::const_iterator ait = temp_attribs.begin();
                            while(ait != temp_attribs.end())
                            {
                                attribs[ait->first] = ait->second;
                                ait++;
                            }
                        }
                    }

                    it = next;
                }

                if (elems.empty())
                {
                    //std::cout << "Invalid attribute: " << _value << std::endl;
                    return false;
                }

                ctxt->setValues(values);
                ctxt->setAttribs(attribs);

                return true;
            }
        private:
            std::string _val;
        };

        class JUDO_API AndOp : public Op
        {
        public:
            AndOp(Op* lh, Op* rh) : Op(OP_AND, "and"), _lh(lh), _rh(rh)
            { }

            ~AndOp()
            {
                delete _lh;
                delete _rh;
            }

            bool isValid(XPath::Value* ctxt)
            {
                if (_lh->isValid(ctxt) && _rh->isValid(ctxt))
                    return true;
                else
                    return false;
            }
        private:
            Op* _lh;
            Op* _rh;
        };

        class JUDO_API OrOp : public Op
        {
        public:
            OrOp(Op* lh, Op* rh) : Op(OP_OR, "or"), _lh(lh), _rh(rh)
            { }

            ~OrOp()
            {
                delete _lh;
                delete _rh;
            }

            bool isValid(XPath::Value* ctxt)
            {
                Value::ElemList& elems = ctxt->getList();
                Value::ElemList::iterator it = elems.begin();
                while (it != elems.end())
                {
                    Value::ElemList::iterator next = it;
                    ++next;

                    Value* tmp_ctxt_l = new Value(*it);
                    tmp_ctxt_l->in_context(true);
                    Value* tmp_ctxt_r = new Value(*it);
                    tmp_ctxt_r->in_context(true);
                    if (!_lh->isValid(tmp_ctxt_l) && !_rh->isValid(tmp_ctxt_r))
                    {
                        elems.erase(it);
                    }

                    it = next;
                }

                if (elems.empty())
                    return false;

                return true;
            }
        private:
            Op* _lh;
            Op* _rh;
        };

        class FunctionOp : public Op
        {
        public:
            JUDO_API FunctionOp(const std::string& func) : Op(OP_FUNCTION, func) , _closed(false)
            { }

            JUDO_API ~FunctionOp()
            {
                while (!_arg_list.empty())
                {
                    Op *op = _arg_list.back();
                    _arg_list.pop_back();
                    delete op;
                }
            }

            JUDO_API bool isValid(XPath::Value* ctxt)
            {
                FunctionMap::iterator it = XPath::functions.find(_value);
                if (it == XPath::functions.end())
                {
                    std::cerr << "Unknown query function " << _value << "()" << std::endl;
                    return false;
                }
                
                return it->second->run(ctxt, _arg_list);
            }

            std::string calcStr(judo::Element* elem)
            {
                FunctionMap::iterator it = XPath::functions.find(_value);
                if (it == XPath::functions.end())
                {
                    std::cerr << "Unknown query function " << _value << "()" << std::endl;
                    return "";
                }
                
                return it->second->value(elem, _arg_list);
            }
            
            void addArg(Op* arg)
            {
                _arg_list.push_back(arg);
            }

        private:
            Query::OpList _arg_list;
            bool _closed;
        };

    };
};

#endif
