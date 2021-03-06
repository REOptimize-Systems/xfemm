#include "fparse.h"

#include "stringTools.h"

#include <algorithm>
#include <string>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace femm
{

char* StripKey(char *c)
{
    char *d;
    int i,k;

    k=strlen(c);

    for(i=0; i<k; i++)
    {
        if (c[i] == '=')
        {
            d=c+i+1;
            return d;
        }
    }

    return c+k;
}

char *ParseDbl(char *t, double *f)
{
    if (t==NULL) return NULL;

    int i,j,k,u,ws;
    static char w[]="\t, \n";
    char *v;

    k=strlen(t);
    if(k==0) return NULL;

    for(i=0,u=0,v=NULL; i<k; i++)
    {
        for(j=0,ws=0; j<4; j++)
        {
            if (t[i]==w[j])
            {
                ws=1;
                if (u==1) u=2;
            }
        }
        if ((ws==0) && (u==0)) u=1;
        if ((ws==0) && (u==2))
        {
            v=t+i;
            break;
        }
    }

    if (u==0) return NULL;	//nothing left in the string;
    if (v==NULL) v=t+k;

    sscanf(t,"%lf",f);

    return v;
}

char *ParseInt(char *t, int *f)
{
    if (t==NULL) return NULL;

    int i,j,k,u,ws;
    static char w[]="\t, \n";
    char *v;

    k=strlen(t);
    if(k==0) return NULL;

    for(i=0,u=0,v=NULL; i<k; i++)
    {
        for(j=0,ws=0; j<4; j++)
        {
            if (t[i]==w[j])
            {
                ws=1;
                if (u==1) u=2;
            }
        }
        if ((ws==0) && (u==0)) u=1;
        if ((ws==0) && (u==2))
        {
            v=t+i;
            break;
        }
    }

    if (u==0) return NULL;	//nothing left in the string;
    if (v==NULL) v=t+k;

    sscanf(t,"%i",f);

    return v;
}

char *parseString(char *t, string *s)
{
    if (t==NULL) return NULL;
    if (std::strlen(t)==0) return t;

    int n1,n2,k;

    // find first quote in the source string
    for(k=0,n1=-1; k< (int) std::strlen(t); k++)
    {
        if (t[k]=='\"')
        {
            n1=k;
            break;
        }
    }

    if (n1<0) return t;

    // find second quote in the source string
    for(k=n1+1,n2=-1; k< (int) std::strlen(t); k++)
    {
        if (t[k]=='\"')
        {
            n2=k;
            break;
        }
    }

    if (n2<0) return t;

    *s=t;
    *s=s->substr(n1+1,n2-n1-1);

    return (t+n2+1);
}

// default function for displaying warning messages
int PrintWarningMsg(const char* message, ...)
{
    return printf("%s", message);
}

bool expectChar(istream &input, char c,  std::ostream &err)
{
    input >> std::ws;
    if ( input.peek() == c )
    {
        input.ignore();
        return true;
    }
    err << "Expected char code " << (int)c
        << "(" << c << "), but got " << input.peek();
    return false;
}

bool expectToken(std::istream &input, const std::string str, std::ostream &err)
{
    string token;

    nextToken(input, &token);
    if ( token != str )
    {
        err << "Expected token " <<str<< ", but got " <<token;
        return false;
    }
    return true;
}

void nextToken(istream &input, string *token)
{
    token->clear();
    input >> *token;
    to_lower(*token);
#ifdef DEBUG_PARSER
    std::cerr << "Read token: " << *token << "\n";
    if (input.eof())
        std::cerr << "Stream is eof\n";
#endif
}

void parseString(istream &input, string *s)
{
    // use dummy stream to suppress output
    std::stringstream dummy;
    parseString(input, s, dummy);
}
bool parseString(istream &input, string *s, ostream &err)
{
    if (s)
        s->clear();
    if (!expectChar(input, '"', err))
    {
        err << "Error: Invalid begin of string literal!\n";
        return false;
    }

    std::string line;
    std::getline(input,line);
    // FEMM42 allows unescaped double-quotes ('"') inside a string;
    // i.e. the last quote in the line terminates the string
    // consuming the rest of the line should not be a problem,
    // because FEMM does only allow one token per line and no multi-line strings
    size_t pos = line.find_last_of('"');
    if (pos == std::string::npos)
    {
        err << "Error: unterminated string literal!\n"
            << "Offending string: " << line << "\n";
        return false;
    }
    if (s)
    {
        *s = line.substr(0,pos);
#ifdef DEBUG_PARSER
        std::cerr << "parsing String "<< *s <<"\n";
#endif
    }
    return true;
}

bool parseValue(istream &input, double &val, ostream &err)
{
    std::string valueString;
    // read rest of the line into a string:
    std::getline(input, valueString);
    trim(valueString);

    try {
        std::string::size_type sz;
        val = std::stod(valueString, &sz);
#ifdef DEBUG_PARSER
        std::cerr << "parsing "<< valueString << " as " << val << " (" << sz << " chars)\n";
#endif
        if (sz != valueString.size())
        {
            err << "Warning: trailing characters: '" << valueString.substr(sz) << "'\n";
        }
    } catch (const std::invalid_argument &e)
    {
        err << "Could not convert '" << valueString << "' to double: " << e.what() << "\n";
        return false;
    } catch (const std::out_of_range &e)
    {
        err << "Value out of range when converting '" << valueString << "' to double: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool parseValue(istream &input, int &val, ostream &err)
{
    std::string valueString;
    // read rest of the line into a string:
    std::getline(input, valueString);
    trim(valueString);

    try {
        std::string::size_type sz;
        val = std::stoi(valueString, &sz);
#ifdef DEBUG_PARSER
        std::cerr << "parsing "<< valueString << " as " << val << " (" << sz << " chars)\n";
#endif
        if (sz != valueString.size())
        {
            err << "Warning: trailing characters: '" << valueString.substr(sz) << "'\n";
        }
    } catch (const std::invalid_argument &e)
    {
        err << "Could not convert '" << valueString << "' to int: " << e.what() << "\n";
        return false;
    } catch (const std::out_of_range &e)
    {
        err << "Value out of range when converting '" << valueString << "' to int: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool parseValue(istream &input, bool &val, ostream &err)
{
    int i=0;
    if (! parseValue(input, i, err))
        return false;

    if ( i != 0 && i != 1)
    {
        err << "Warning: bool out of range: " << i << "\n";
    }
    val = i;
    return true;
}

}
