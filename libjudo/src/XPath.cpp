#include "XPath.h"
#include "XPathOps.h"
#include "XPathFunctions.h"

using namespace std;
using namespace judo;
using namespace judo::XPath;

FunctionMap judo::XPath::functions;

char Query::getNextToken(string::size_type& cur)
{
    if (cur == std::string::npos)
        return 0;

    std::list<char>::iterator it = find (_tokens.begin(), _tokens.end(),
            _query[cur]);;

    while (it == _tokens.end())
    {
        cur++;
        if (cur > _query.length())
        {
            cur = _query.length();
            return 0;
        }
        it = find(_tokens.begin(), _tokens.end(), _query[cur]);
    }
    return *it;
}

std::string Query::getNextIdentifier(string::size_type& pos)
{
    string::size_type sp = pos;
    getNextToken(pos);
    return _query.substr(sp, pos - sp);
}

Op* Query::getOp(std::string::size_type& pos, char in_context)
{
    Op* ret_op = NULL;

    do
    {
        Op* tmp_op = NULL;
        string::size_type pos_start = pos;
        char token = getNextToken(pos);
        // If we are in a context and get a token barf
        if (token == 0 && in_context)
            throw Invalid();
        string::size_type token_start = ++pos;
        string ident;

        if (pos_start != (token_start - 1))
        {
            pos = pos_start;
            ret_op = new NodeOp(getNextIdentifier(pos), false);
        }
        else
        {
        
            // Get rid of the whitespace
            switch(token)
            {
                case '/':
                    if (_query[token_start] == '/')
                    {
                        pos++;
                        string temp_ident = getNextIdentifier(pos);
                        if (!temp_ident.empty())
                        {
                            ret_op = new AllOp(temp_ident);
                        }
                    }
                    else
                    {
                        string temp_ident = getNextIdentifier(pos);
                        if (!temp_ident.empty())
                        {
                            ret_op = new NodeOp(temp_ident,
                                    (pos_start == 0 ? true : false));
                        }
                    }
                    break;
                case '@':
                    ret_op = new AttributeOp(getNextIdentifier(pos));
                    break;
                case ']':
                    if (in_context == '[')
                    {
                        ret_op = _ops.back();
                        _ops.pop_back();
                        in_context = 0;
                    }
                    else
                    {
                        std::cerr << "Found ']' but not in context" << std::endl;
                        throw Invalid();
                    }
                    break;
                case '\'':
                case '\"':
                    pos = _query.find(token, token_start);
                    ret_op = new Op(Op::OP_LITERAL, _query.substr(token_start, pos - 
                                token_start));
                    pos++;
                    break;
                case ' ':
                    //ident = _query.substr(pos_start, pos - pos_start - 1);
                    ident = getNextIdentifier(pos);
                    if (ident == "and")
                    {
                        pos++;
                        tmp_op = getOp(pos, in_context);
                        if (!tmp_op)
                        {
                            std::cerr << "Invalid and operation" << std::endl;
                            throw Invalid();
                        }
                        ret_op = new AndOp(_ops.back(), tmp_op);
                        in_context = 0;
                        _ops.pop_back();
                    }
                    else if(ident == "or")
                    {
                        pos++;
                        tmp_op = getOp(pos, in_context);
                        ret_op = new OrOp(_ops.back(), tmp_op);
                        in_context = 0;
                        _ops.pop_back();
                    }
                    break;
                case '[':
                    // See if we have a position op
                    if (getNextToken(pos) == ']')
                    {
                        if (pos == token_start)
                        {
                            std::cerr << "Nothing in the []" << std::endl;
                            throw Invalid();
                        }
                        
                        pos = token_start;

                        std::string temp_ident = getNextIdentifier(pos);
                        int val = atoi(temp_ident.c_str());
                        // small hack here... if you get something back
                        // assume it was a number
                        if (val > 0)
                        {
                            ret_op = new PositionOp(val);
                            pos++;
                        }
                        // otherwise assume it was a node op, and start a new
                        // context ("0" is not a number in this model)
                        else
                        {
                            pos = pos_start + 1;
                            ret_op = new ContextOp(getOp(pos, token));
                        }
                    }
                    else
                    {
                        pos = pos_start + 1;
                        ret_op = new ContextOp(getOp(pos, token));
                    }
                    break;
                case '(':
                    //--------------------------------------------------------
                    // The function name would have been mistaken for a NodeOp.
                    // Pop it off the back and get the function name.
                    //--------------------------------------------------------
                    tmp_op = _ops.back();
                    _ops.pop_back();
                    ident = tmp_op->getValue();
                    delete tmp_op;
                    
                    if (!ident.empty())
                    {
                        int op_pos = _ops.size();
                        if (XPath::functions.count(ident) == 0)
                        {
                            std::cerr << "No function named " << ident << 
                                " has been defined." << std::endl;
                            throw Invalid();
                        }
                        ret_op = new FunctionOp(ident);

                        getOp(pos, token);

                        OpList::iterator it = _ops.begin() + op_pos;
                        while( it != _ops.end())
                        {
                            static_cast<FunctionOp*>(ret_op)->addArg(*it);
                            it = _ops.erase(it);
                        }
                    }
                    else
                    {
                        std::cerr << "No function name specified!\n" <<
                            std::endl;
                        throw Invalid();
                    }
                    break;
                case ')':
                    if (in_context == '(')
                    {
                        ret_op = NULL;
                        in_context = 0;
                    }
                    else
                    {
                        std::cerr << "Found ')' but not in context" << std::endl;
                        throw Invalid();
                    }
                    break;
                case ',':
                    if (in_context != '(')
                    {
                        std::cerr << "Found ',' but not in a function" << std::endl;
                        throw Invalid();
                    }
                    break;
                case '=':
                    while (!tmp_op)
                        tmp_op = getOp(pos);
                    
                    ret_op = new EqualOp(_ops.back(), tmp_op);
                    _ops.pop_back();
                    break;
                case '!':
                    if (_query[token_start] != '=')
                    {
                        std::cerr << "Badly formed !=" << std::endl;
                        throw Invalid();
                    }
                    pos++;
                    while (!tmp_op)
                        tmp_op = getOp(pos);
                    
                    ret_op = new NotEqualOp(_ops.back(), tmp_op);
                    _ops.pop_back();
                    break;
                default:
                    std::cerr << "Unhandled \"" << token << "\"" << std::endl;
                    ret_op = NULL;
                    break;
            };
        }

        if (in_context)
        {
            if (ret_op)
            {
                _ops.push_back(ret_op);
            }
            ret_op = NULL;
        }

    } while (in_context);

    return ret_op;
}

bool Query::parseQuery()
{
    Op* op = NULL;
    string::size_type pos = 0;

    while (pos < _query.length())
    {
        op = getOp(pos);
        if (op)
        {
            _ops.push_back(op);
        }
    }

    return true;
}
    
bool Query::check(const judo::Element& root)
{
    judo::Element check_elem(root);
    Value* ctxt = execute(&check_elem);
    bool matched = ctxt->check();
    delete ctxt;
    return matched;
}

Value* Query::execute(judo::Element* root)
{
    OpList::iterator it = _ops.begin();

    Value* ctxt = new Value(root);
    
    for (;it != _ops.end(); it++)
    {
        //(*it)->display();
        if (!(*it)->isValid(ctxt))
        {
            //(*it)->display();
            ctxt->setMatch(false);
            return ctxt;
        }
    }

    /* XXX DEBUG 
    Value::ElemList elems = ctxt.getList();
    for(Value::ElemList::iterator it = elems.begin(); it != elems.end(); 
            it++)
    {
        std::cout << "Ctxt Entry: " << (*it)->toString() << std::endl;
    }
    */
    ctxt->setMatch(true);
    return ctxt;
}

void XPath::init_functions()
{
    functions.clear();
    functions.insert(FunctionMap::value_type("text", new TextFunction()));
    functions.insert(FunctionMap::value_type("name", new NameFunction()));
    functions.insert(FunctionMap::value_type("not", new NotFunction()));
    functions.insert(FunctionMap::value_type("starts-with", new StartsWithFunction()));
}

void XPath::add_function(const std::string& name, XPath::Function* func)
{
    if (XPath::functions.empty())
        init_functions();

    XPath::functions.insert(FunctionMap::value_type(name, func));
}

