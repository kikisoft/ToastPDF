/**
	@file
	@brief 
*/
// XMLite.h: interface for the XMLite class.
//
// XMLite : XML Lite Parser Library
// by bro ( Cho,Kyung Min: bro@shinbiro.com ) 2002-10-30
// Microsoft MVP (Visual C++) bro@msmvp.com
// Updates for VS2005, additions for CCInfo, CCPrinter, and other fixes hguy@cogniview.com
// 
// History.
// 2002-10-29 : First Coded. Parsing XMLElelement and Attributes.
//              get xml parsed string ( looks good )
// 2002-10-30 : Get Node Functions, error handling ( not completed )
// 2002-12-06 : Helper Funtion string to long
// 2002-12-12 : Entity Helper Support
// 2003-04-08 : Close, 
// 2003-07-23 : add property escape_value. (now no escape on default)
// 2003-10-24 : bugfix) attribute parsing <tag a='1' \r\n/> is now ok
// 2004-03-05 : add branch copy functions
// 2004-06-14 : add _tcseistr/_tcsenistr/_tcsenicmp functions
// 2004-06-14 : now support, XML Document and PI, Comment, CDATA node
// 2004-06-15 : add GetText()/ Find() functions
// 2004-06-15 : add force_parse : now can parse HTML (not-welformed xml)
// 2006-10-08 : GH: multiple fixes for compiling under VS2005, replaced CString with std::string
// 2006-10-18 : GH: added const functions and some helper functions
// 2007-03-25 : GH: fixed for UNICODE compilation
// 2007-06-06 : GH: added some more helper functions
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLITE_H__786258A5_8360_4AE4_BDAF_2A52F8E1B877__INCLUDED_)
#define AFX_XMLITE_H__786258A5_8360_4AE4_BDAF_2A52F8E1B877__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <vector>
#include <deque>
#include <string>
#include "CCTChar.h"
#include <tchar.h>

struct _tagXMLAttr;
typedef _tagXMLAttr XAttr, *LPXAttr;
typedef std::vector<LPXAttr> XAttrs;

struct _tagXMLNode;
typedef _tagXMLNode XNode, *LPXNode;
typedef std::vector<LPXNode> XNodes, *LPXNodes;

struct _tagXMLDocument;
typedef struct _tagXMLDocument XDoc, *LPXDoc;

// Entity Encode/Decode Support
typedef struct _tagXmlEntity
{
	TCHAR entity;					// entity ( & " ' < > )
	TCHAR ref[10];					// entity reference ( &amp; &quot; etc )
	int ref_len;					// entity reference length
}XENTITY,*LPXENTITY;

typedef struct _tagXMLEntitys : public std::vector<XENTITY>
{
	LPXENTITY GetEntity( int entity );
	LPXENTITY GetEntity( LPCTSTR entity );	
	int GetEntityCount( LPCTSTR str );
	int Ref2Entity( LPCTSTR estr, LPTSTR str, int len );
	int Entity2Ref( LPCTSTR str, LPTSTR estr, int estrlen );
	std::tstring Ref2Entity( LPCTSTR estr );
	std::tstring Entity2Ref( LPCTSTR str );	
	int Ref2Entity( LPCTSTR estr, std::tstring& str, int elen );
	int Entity2Ref( LPCTSTR str, std::tstring& estr, int len );

	_tagXMLEntitys(){};
	_tagXMLEntitys( LPXENTITY entities, int count );
}XENTITYS,*LPXENTITYS;
extern XENTITYS entityDefault;
std::tstring XRef2Entity( LPCTSTR estr );
std::tstring XEntity2Ref( LPCTSTR str );	

typedef enum 
{
	PIE_PARSE_WELFORMED	= 0,
	PIE_ALONE_NOT_CLOSED,
	PIE_NOT_CLOSED,
	PIE_NOT_NESTED,
	PIE_ATTR_NO_VALUE
}PCODE;

// Parse info.
typedef struct _tagParseInfo
{
	bool		trim_value;			// [set] do trim when parse?
	bool		entity_value;		// [set] do convert from reference to entity? ( &lt; -> < )
	LPXENTITYS	entitys;			// [set] entity table for entity decode
	TCHAR		escape_value;		// [set] escape value (default '\\')
	bool		force_parse;		// [set] force parse even if xml is not welformed

	LPTSTR		xml;				// [get] xml source
	bool		erorr_occur;		// [get] is occurance of error?
	LPTSTR		error_pointer;		// [get] error position of xml source
	PCODE		error_code;			// [get] error code
	std::tstring error_string;		// [get] error string

	LPXDoc		doc;
	_tagParseInfo() { trim_value = false; entity_value = true; force_parse = false; entitys = &entityDefault; xml = NULL; erorr_occur = false; error_pointer = NULL; error_code = PIE_PARSE_WELFORMED; escape_value = '\\'; }
}PARSEINFO,*LPPARSEINFO;
extern PARSEINFO piDefault;

// display optional environment
typedef struct _tagDispOption
{
	bool newline;			// newline when new tag
	bool reference_value;	// do convert from entity to reference ( < -> &lt; )
	char value_quotation_mark;	// val="" (default value quotation mark "
	LPXENTITYS	entitys;	// entity table for entity encode

	int tab_base;			// internal usage
	_tagDispOption() { newline = true; reference_value = true; entitys = &entityDefault; tab_base = 0; value_quotation_mark = '"'; }
}DISP_OPT, *LPDISP_OPT;
extern DISP_OPT optDefault;

// XAttr : Attribute Implementation
typedef struct _tagXMLAttr
{
	std::tstring name;
	std::tstring value;
	
	_tagXMLNode*	parent;

	std::tstring GetXML( LPDISP_OPT opt = &optDefault ) const;
	std::tstring GetValue( LPDISP_OPT opt = &optDefault ) const;
}XAttr, *LPXAttr;

typedef enum
{
	XNODE_ELEMENT,				// general node '<element>...</element>' or <element/>
	XNODE_PI,					// <?xml version="1.0" ?>
	XNODE_COMMENT,				// <!-- comment -->
	XNODE_CDATA,				// <![CDATA[ cdata ]]>
	XNODE_DOC,					// internal virtual root
}NODE_TYPE;

// XMLNode structure
typedef struct _tagXMLNode
{
	// name and value
	std::tstring name;
	std::tstring value;

	// internal variables
	LPXNode	parent;		// parent node
	XNodes	childs;		// child node
	XAttrs	attrs;		// attributes
	NODE_TYPE type;		// node type 
	LPXDoc	doc;		// document

	// Load/Save XML
	LPTSTR	Load( LPCTSTR pszXml, LPPARSEINFO pi = &piDefault );
	std::tstring GetXML( LPDISP_OPT opt = &optDefault ) const;
	std::tstring GetText( LPDISP_OPT opt = &optDefault ) const;

	// internal load functions
	LPTSTR	LoadAttributes( LPCTSTR pszAttrs, LPPARSEINFO pi = &piDefault );
	LPTSTR	LoadAttributes( LPCTSTR pszAttrs, LPCTSTR pszEnd, LPPARSEINFO pi = &piDefault );
	LPTSTR	LoadProcessingInstrunction( LPCTSTR pszXml, LPPARSEINFO pi = &piDefault );
	LPTSTR	LoadComment( LPCTSTR pszXml, LPPARSEINFO pi = &piDefault ); 
	LPTSTR	LoadCDATA( LPCTSTR pszXml, LPPARSEINFO pi = &piDefault ); 

	// in own attribute list
	LPXAttr	GetAttr( LPCTSTR attrname ); 
	LPCTSTR	GetAttrValue( LPCTSTR attrname ); 
	XAttrs	GetAttrs( LPCTSTR name ); 

	// in one level child nodes
	LPXNode	GetChild( LPCTSTR name ); 
	LPCTSTR	GetChildValue( LPCTSTR name ); 
	std::tstring GetChildText( LPCTSTR name, LPDISP_OPT opt = &optDefault );
	XNodes	GetChilds( LPCTSTR name ); 
	XNodes	GetChilds(); 

	LPXAttr GetChildAttr( LPCTSTR name, LPCTSTR attrname );
	LPCTSTR GetChildAttrValue( LPCTSTR name, LPCTSTR attrname );
	
	// search node
	LPXNode	Find( LPCTSTR name );
	const LPXNode Find ( LPCTSTR name ) const;
	LPXNode	Find( LPCTSTR name, LPCTSTR id );
	const LPXNode Find ( LPCTSTR name, LPCTSTR id ) const;

	// modify DOM 
	int		GetChildCount();
	LPXNode GetChild( XNodes::size_type i );
	XNodes::iterator GetChildIterator( LPXNode node );
	LPXNode CreateNode( LPCTSTR name = NULL, LPCTSTR value = NULL );
	LPXNode	AppendChild( LPCTSTR name = NULL, LPCTSTR value = NULL );
	LPXNode	AppendChild( LPXNode node );
	bool	RemoveChild( LPXNode node );
	LPXNode DetachChild( LPXNode node );

	// node/branch copy
	void	CopyNode( LPXNode node );
	void	CopyBranch( const LPXNode branch );
	void	_CopyBranch( const LPXNode node, LPXDoc	docNew = NULL );
	LPXNode	AppendChildBranch( LPXNode node );

	// modify attribute
	LPXAttr GetAttr( XAttrs::size_type i );
	XAttrs::iterator GetAttrIterator( LPXAttr node );
	LPXAttr CreateAttr( LPCTSTR anem = NULL, LPCTSTR value = NULL );
	LPXAttr AppendAttr( LPCTSTR name = NULL, LPCTSTR value = NULL );
	LPXAttr	AppendAttr( LPXAttr attr );
	bool	RemoveAttr( LPXAttr attr );
	LPXAttr DetachAttr( LPXAttr attr );

	// operator overloads
	LPXNode operator [] ( int i ) { return GetChild(i); }
	XNode& operator = ( const XNode& node ) { CopyBranch((const LPXNode)&node); return *this; }

	_tagXMLNode() { parent = NULL; doc = NULL; type = XNODE_ELEMENT; }
	~_tagXMLNode();

	void Close();

protected:
	// Helper functions
	std::tstring GetTextFromValue(LPDISP_OPT opt) const;

}XNode, *LPXNode;

// XMLDocument structure
typedef struct _tagXMLDocument : public XNode
{
	PARSEINFO	parse_info;

	_tagXMLDocument() { parent = NULL; doc = this; type = XNODE_DOC; }
	
	LPTSTR	Load( LPCTSTR pszXml, LPPARSEINFO pi = NULL );
	LPXNode	GetRoot();
	const LPXNode GetRoot() const;

	const XDoc& operator = ( const XDoc& doc );

}XDoc, *LPXDoc;

// Helper Funtion
inline long XStr2Int( LPCTSTR str, long default_value = 0 )
{
	return ( str && *str ) ? _ttol(str) : default_value;
}

inline bool XIsEmptyString( const std::tstring& s )
{
	std::tstring::const_iterator i = s.begin();
	while ((i != s.end()) && (_tcschr(_T(" \n\r\t"), *s.begin()) != NULL))
		i++;

	return (i == s.end());
}

#endif // !defined(AFX_XMLITE_H__786258A5_8360_4AE4_BDAF_2A52F8E1B877__INCLUDED_)
