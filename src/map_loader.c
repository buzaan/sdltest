#include <ctype.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map_loader.h"

enum Symbol
{
    str,
    num,
    lbracket,
    rbracket,
    eql,
    comma,
    nl,
    eof,
};
typedef enum Symbol Symbol;

static const char *sym_to_str(Symbol s)
{
    static const char *names[] = {
	"string",
	"number",
	"'['",
	"']'",
	"'='",
	"','",
	"<newline>",
	"<eof>"};
    return names[s];
}

static const int TOKEN_MAX = 32;
struct Parser
{
    Symbol sym;
    char token[TOKEN_MAX];
    FILE *stream;

    //Debugging info
    Symbol expected;
    jmp_buf error;
    int lines;
};
typedef struct Parser Parser;
static const int PARSE_ERROR = 1;

static void read_tok(Parser *p, char ch, int (*fn)(int ch), int limit)
{
    p->token[0] = ch;
    ch = fgetc(p->stream);
    int i = 1;
    while(fn(ch))
    {
	if(i < limit - 1) 
	{
	    p->token[i] = ch;
	    ++i;
	}
	ch = fgetc(p->stream);
    }
    p->token[i] = '\0';
    ungetc(ch, p->stream);
}

static int is_str(int ch)
{
    return isalnum(ch) || ch == '.' || ch == ' ';
}

static Symbol next(Parser *p)
{
    int ch = fgetc(p->stream);
    switch(ch)
    {
    case '[': p->sym = lbracket; break;
    case ']': p->sym = rbracket; break;
    case '=': p->sym = eql; break;
    case ',': p->sym = comma; break;
    case '\n': 
	p->sym = nl; 
	++p->lines;
	break;
    case EOF: p->sym = eof; break;
    default:
	if(isalpha(ch))
	{
	    p->sym = str;
	    read_tok(p, ch, is_str, TOKEN_MAX);
	}
	else if(isdigit(ch))
	{
	    p->sym = num;
	    read_tok(p, ch, isdigit, TOKEN_MAX);
	}
	else
	{
	    longjmp(p->error, PARSE_ERROR);
	}
	break;
    }
    return p->sym;
}

static bool accept(Parser *p, Symbol s)
{
    if(p->sym == s)
    {
	next(p);
	return true;
    }
    return false;
}

static bool accept_str(Parser *p, const char *chars)
{
    if(p->sym == str && strcmp(p->token, chars) == 0)
    {
	next(p);
	return true;
    }
    return false;
}

static bool expect(Parser *p, Symbol s)
{
    p->expected = s;
    if(accept(p, s))
    {
	return true;
    }
    else
    {
	longjmp(p->error, PARSE_ERROR);
    }
}

void header(Parser *p)
{
    if(accept_str(p, "width"))
    {
	expect(p, eql);
	expect(p, num);
    }
    else if(accept_str(p, "height"))
    {
	expect(p, eql);
	expect(p, num);
    }
    else if(accept_str(p, "tilewidth"))
    {
	expect(p, eql);
	expect(p, num);
    }
    else if(accept_str(p, "tileheight"))
    {
	expect(p, eql);
	expect(p, num);
    }
}

// tilesets := 'tileset=' '=' str ',' num ',' num ',' num ',' num '\n'
static void tilesets(Parser *p)
{
    if(accept_str(p, "tileset"))
    {
	expect(p, eql);
	expect(p, str); //filename
	expect(p, comma);
	expect(p, num); //width
	expect(p, comma);
	expect(p, num); //height
	expect(p, comma);
	expect(p, num); //margin?
	expect(p, comma);
	expect(p, num); //spacing?
    }
}

// data := {{num ','} '\n'}
static void data(Parser *p)
{
    do
    {
	while(accept(p, num))
	{
	    accept(p, comma);
	}
    }while(accept(p, nl) && p->sym != nl);    
}

static void layer(Parser *p)
{
    if(accept_str(p, "type"))
    {
	expect(p, eql);
	expect(p, str);
    }
    else if(accept_str(p, "data"))
    {	
	expect(p, eql);
	data(p);
    }
}

static void section_header(Parser *p, char *name)
{
    if(accept(p, lbracket))
    {
	expect(p, str);
	if(name)
	{
	    strlcpy(name, p->token, TOKEN_MAX);
	}
	expect(p, rbracket);
    }
}

static void section(Parser *p)
{
    char section[TOKEN_MAX];
    section_header(p, section);
    expect(p, nl);
    if(strcmp(section, "header") == 0)
    {
	do 
	{ 
	    header(p); 
	}while(accept(p, nl) && p->sym != nl);
    }
    else if(strcmp(section, "tilesets") == 0)
    {
	do 
	{
	    tilesets(p);
	}while(accept(p, nl) && p->sym != nl);
    }
    else if(strcmp(section, "layer") == 0)
    {
	do 
	{
	    layer(p); 
	} while(accept(p, nl) && p->sym != nl);
    }
}

static void config(Parser *p)
{
    do
    {
	section(p);
    }while(accept(p, nl));
    expect(p, eof);
}

static bool parse(FILE *stream)
{
    Parser p = {.stream = stream };
    if(setjmp(p.error) != PARSE_ERROR)
    {
	next(&p);
	config(&p);
	return true;
    }
    else
    {
	fprintf(stderr, "Parse failure on line %d. Expected %s, got %s\n", 
		p.lines + 1,
		sym_to_str(p.expected),
		sym_to_str(p.sym));
	if(p.sym == str && p.expected == p.sym)
	{
	    fputs("String format error.\n", stderr);
	}
	return false;
    }
}

bool load_map(FILE *stream, TileMap *map)
{
    if(!stream) return false;
    return parse(stream);
}
