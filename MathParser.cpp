#include "MathParser.h"
#include <stack>
#include "string-switch.h"

using namespace mathS;

#define ERROR_CHECK(x) if (x->GetType() == MathObject::ERROR) return x
#define ERROR_CHECK_LIST(x) if (x->GetType() == MathObject::ERROR) return {x}
#define LIST_ERROR_CHECK(x) if (x.size()>0 && x[0]->GetType() == MathObject::ERROR) return x[0]

// 解析表达式
Ptr<MathObject> mathS::Parse(const std::string & c) {
    Lexer lexer(c);
    std::vector<Token> tokens;
    Token tok;
    while (true) {
        lexer.get(tok);
        if (tok.type == Token::END) break;
        tokens.push_back(tok);
    }
    
    int i;
    auto obj = parseObject(tokens, 0, i);
    ERROR_CHECK(obj);
    if (i < tokens.size()) {
        return New<ErrorObject>("Parse: Syntax Error. Unexpected Symbol " + tokens[i].text);
    }
    
    return obj;
}

short mathS::level(const std::string &c) {
    SWITCH(c.c_str()) {
        CASE(c, "(") return MathObject::LEVEL_FUNCTION;
        CASE(c, "<<") return MathObject::LEVEL_FUNCOPERATOR;
        CASE(c, "[") return MathObject::LEVEL_LOCATE;
        CASE(c, "^") return MathObject::LEVEL_POWER;
        CASE(c, "/") return MathObject::LEVEL_ITEM;
        CASE(c, "*") return MathObject::LEVEL_ITEM;
        CASE(c, "-") return MathObject::LEVEL_POLYNOMIAL;
        CASE(c, "+") return MathObject::LEVEL_POLYNOMIAL;
        CASE(c, "==") return MathObject::LEVEL_COMPARE;
        CASE(c, "<") return MathObject::LEVEL_COMPARE;
        CASE(c, ">") return MathObject::LEVEL_COMPARE;
        CASE(c, "<=") return MathObject::LEVEL_COMPARE;
        CASE(c, ">=") return MathObject::LEVEL_COMPARE;
        CASE(c, ",") return MathObject::LEVEL_LIST;
        CASE(c, "->")return MathObject::LEVEL_MAP;

        CASE(c, ")") return MathObject::LEVEL_CLOSED;
        CASE(c, "|") return MathObject::LEVEL_CLOSED;
        CASE(c, ">>") return MathObject::LEVEL_CLOSED;
        CASE(c, "]") return MathObject::LEVEL_CLOSED;
        CASE(c, "}") return MathObject::LEVEL_CLOSED;

        default: return MathObject::LEVEL_ERROR;
    }
    return 0;
}


Ptr<MathObject> mathS::parseObject(const std::vector<Token>& tokens, const int start, int& i)
{
    i = start;
    return parseCompare(tokens, i, i);
}
Ptr<MathObject> mathS::parseAtom(const std::vector<Token>& tokens, const int start, int& i)
{
    Ptr<MathObject> obj;
    i = start;
    if (i >= tokens.size())
        return New<ErrorObject>("Parse: Expression is incompleted.");
    if (tokens[i].type == Token::NUMORSYMB || tokens[i].type == Token::STRING) {
        obj = New<Atom>(tokens[i].text);
        i++;
        return obj;
    }
    else if (tokens[i].type == Token::OPERATOR) {
        // 如果是操作符，那么必须是(_OBJECT)或{_OBJECT}
        if (tokens[i].text == "(") {
            obj = parseObject(tokens, i + 1, i);
            ERROR_CHECK(obj);
            if (i < tokens.size() && tokens[i].text == ")") {
                i++;
                return obj;
            }
            else {
                return New<ErrorObject>("Parser: Unmatched brace (");
            }
        }
        else if(tokens[i].text == "{")
        {
            Ptr<Vector> vobj = New<Vector>();
            vobj->components = parseList(tokens, start + 1, i);
            LIST_ERROR_CHECK(vobj->components);
            
            if (i < tokens.size() && tokens[i].text == "}") {
                i++;
                return vobj;
            }
            else {
                return New<ErrorObject>("Parser: Unmatched brace {");
            }
        }
    }
    return  New<ErrorObject>("Parse: Syntax Error. Unexpected Symbol. " + tokens[i].text);
}


Ptr<MathObject> mathS::parseFunction(const std::vector<Token>& tokens, const int start, int& i)
{
    i = start;

    auto f = parseAtom(tokens, start, i);
    ERROR_CHECK(f);
    if (i >= tokens.size()) 
        return f;
    
    if (tokens[i].text == "(") {
        // ( ) 表示函数
        auto p = parseList(tokens, i + 1, i);
        LIST_ERROR_CHECK(p);
        if (i < tokens.size() && tokens[i].text == ")") {
            i++;
            Ptr<Function> fobj = New<Function>();
            fobj->function = f;
            fobj->parameter = p;
            return fobj;
        }
        else {
            return New<ErrorObject>("Parse: Syntax Error. Unmatched brace (");
        }
    }
    else if (tokens[i].text == "<<") {
        // << | >>( ) 表示泛函算子
        std::vector<Ptr<Atom>> vars;
        // variables
        while (true) {
            auto v = parseAtom(tokens, i + 1, i);
            ERROR_CHECK(v);
            vars.push_back(Dynamic_cast<Atom, MathObject>(v));
            if (i >= tokens.size())
                return New<ErrorObject>("Parse: Syntax Error. Unmatched <<. ");
            
            if (tokens[i].text == "|") 
                break;
            else if (tokens[i].text != ",") 
                return New<ErrorObject>("Parse: Syntax Error. Unmatched <<. ");
        }
        auto fpara = parseList(tokens, i + 1, i);
        LIST_ERROR_CHECK(fpara);
        if (!(i < tokens.size() && tokens[i].text == ">>"))
            return New<ErrorObject>("Parser: Unmatched <<");
        
        i++;
        if (!(i < tokens.size() && tokens[i].text == "("))
            return New<ErrorObject>("Parser: Incompleted Functional Operator");
        
        auto para = parseList(tokens, i + 1, i);
        LIST_ERROR_CHECK(para);
        if (!(i < tokens.size() && tokens[i].text == ")")) 
            return New<ErrorObject>("Parser: Unmatched brace (");

        Ptr<FunctionalOperator> fop = New<FunctionalOperator>();

        fop->function = f;
        fop->variables = vars;
        fop->fparameter = fpara;
        fop->parameter = para;
        i++;
        return fop;
    }
    else if (level(tokens[i].text) > MathObject::LEVEL_FUNCTION) {
        // 后面已经截断，说明并不是函数形式，把 f 返回即可
        return f;
    }
    else {
        return New<ErrorObject>("Parse: Syntax Error. Unexpected symbol " + tokens[i].text);
    }

}

Ptr<MathObject> mathS::parseLocate(const std::vector<Token>& tokens, const int start, int& i)
{
    i = start;
    auto obj = parseFunction(tokens, start, i);
    ERROR_CHECK(obj);
    if (i >= tokens.size() || level(tokens[i].text) > MathObject::LEVEL_LOCATE) {
        return obj;
    }
    if (tokens[i].text == "[") {
        auto loc = parseList(tokens, i + 1, i);
        LIST_ERROR_CHECK(loc);

        if (!(i < tokens.size() && tokens[i].text == "]")) 
            return New<ErrorObject>("Parse: Syntax Error. Unmatched brace [");
        i++;
        Ptr<Locate> lc = New<Locate>();
        lc->object = obj;
        lc->location = loc;
        return lc;
    }
    else {
        return New<ErrorObject>("Parse: Syntax Error. Unexpected symbol " + tokens[i].text);
    }
       
}

Ptr<MathObject> mathS::parsePower(const std::vector<Token>& tokens, const int start, int& i)
{
    i = start;
    auto b = parseLocate(tokens, start, i);
    ERROR_CHECK(b);
    // 是否是指数形式
    if (!(i < tokens.size() && tokens[i].text == "^")) {
        return b;
    }
    auto e = parsePower(tokens, i + 1, i);
    ERROR_CHECK(e);
    // parsePower 和 parseAtom 一定不会返回 Empty
    Ptr<Power> pw = New<Power>();
    pw->base = b;
    pw->exponent = e;
    return pw;
}

Ptr<MathObject> mathS::parseItem(const std::vector<Token>& tokens, const int start, int& i)
{
    i = start;
    if (i >= tokens.size() || level(tokens[i].text) > MathObject::LEVEL_ITEM) {
        return New<ErrorObject>("Parse: Syntax Error. Incomplete expression.");
    }
    Ptr<Item> itm = New<Item>();
    if (tokens[i].text != "/") {
        auto fct = parsePower(tokens, start, i);
        ERROR_CHECK(fct);
        itm->push_back(fct);
    }
    while (true) {
        if (i >= tokens.size() || level(tokens[i].text) > MathObject::LEVEL_ITEM) {
            // 若只有一项，简化表达式
            if (itm->factors.size() > 1)
                return itm;
            else
                return itm->factors[0];
        }
        if (tokens[i].text == "*") {
            // 增加乘因子
            auto t = parsePower(tokens, i + 1, i);
            ERROR_CHECK(t);
            itm->push_back(t);
        }
        else if (tokens[i].text == "/") {
            // 增加除因子
            auto t = parsePower(tokens, i + 1, i);
            ERROR_CHECK(t);
            itm->push_back(New<Inverse>(t));
        }
        else {
            return New<ErrorObject>("Parse: Syntax Error. Unexpected Symbol " + tokens[i].text);
        }
        
    }
    
}

Ptr<MathObject> mathS::parsePolynomial(const std::vector<Token>& tokens, const int start, int& i)
{
    i = start;
    if (i >= tokens.size() || level(tokens[i].text) > MathObject::LEVEL_POLYNOMIAL) {
        return New<ErrorObject>("Parse: Syntax Error. Incomplete expression.");
    }
    Ptr<MathObject> itm;
    Ptr<Polynomial> poly = New<Polynomial>();
    if (tokens[i].text != "+" && tokens[i].text != "-") {
        itm = parseItem(tokens, i, i);
        ERROR_CHECK(itm);
        poly->push_back(itm);
    }
    while (true) {
        if (i >= tokens.size() || level(tokens[i].text) > MathObject::LEVEL_POLYNOMIAL) {
            // 若只有一项，简化表达式
            if (poly->items.size() > 1)
                return poly;
            else 
                return poly->items[0];
        }
        if (tokens[i].text == "+") {
            // 增加正项
            auto t = parseItem(tokens, i + 1, i);
            ERROR_CHECK(t);
            poly->push_back(t);
        }
        else if (tokens[i].text == "-") {
            // 增加负项
            auto t = parseItem(tokens, i + 1, i);
            ERROR_CHECK(t);
            poly->push_back(New<Opposite>(t));
        }
        else {
            // 多项式没有终止，而下一个字符无法识别有效，报错
            return New<ErrorObject>("Parse: Syntax Error. Unexpected Symbol " + tokens[i].text);
        }
    }
}

Ptr<MathObject> mathS::parseMap(const std::vector<Token>& tokens, const int start, int& i)
{
    i = start;
    auto a = parsePolynomial(tokens, i, i);
    ERROR_CHECK(a);
    if (i >= tokens.size() || level(tokens[i].text) > MathObject::LEVEL_MAP)
        return a;
    if (tokens[i].text != "->")
        return New<ErrorObject>("Parse: Syntax Error. Unexpected Symbol " + tokens[i].text);
    
    auto b = parsePolynomial(tokens, i + 1, i);
    ERROR_CHECK(b);
    Ptr<Map> mp = New<Map>(a, b);
    return mp;
}

Ptr<MathObject> mathS::parseCompare(const std::vector<Token>& tokens, const int start, int& i)
{
    i = start;
    auto a = parseMap(tokens, i, i);
    ERROR_CHECK(a);
    if (i >= tokens.size() || level(tokens[i].text) > MathObject::LEVEL_COMPARE)
        return a;
    if (level(tokens[i].text) != MathObject::LEVEL_COMPARE) {
        return New<ErrorObject>("Parse: Syntax Error. Unexpected Symbol " + tokens[i].text);
    }
    std::string op = tokens[i].text;
    auto b = parseMap(tokens, i + 1, i);
    ERROR_CHECK(b);
    Ptr<Compare> cmp = New<Compare>(a, op, b);
    return cmp;
}


std::vector<Ptr<MathObject>> mathS::parseList(const std::vector<Token>& tokens, const int start, int& i)
{
    i = start;
    if (i >= tokens.size() || level(tokens[i].text) > MathObject::LEVEL_LIST)
        return std::vector<Ptr<MathObject>>();
    auto obj = parseCompare(tokens, start, i);
    ERROR_CHECK_LIST(obj);

    std::vector<Ptr<MathObject>> lst;
    lst.push_back(obj);
    while (true) {
        if (i >= tokens.size() || level(tokens[i].text) > MathObject::LEVEL_LIST)
            return lst;
        if (tokens[i].text == ",") {
            auto t = parseCompare(tokens, i + 1, i);
            ERROR_CHECK_LIST(t);
            lst.push_back(t);
        }
        else {
            return { New<ErrorObject>("Parse: Syntax Error. Unexpected Symbol " + tokens[i].text) };
        }
    }
}

