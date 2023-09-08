//this file is part of notepad++
//Copyright (C)2022 Don HO <don.h@free.fr>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE /*hModule*/)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
    setCommand(0, TEXT("Hello Notepad++"), hello, NULL, false);
    setCommand(1, TEXT("Hello (with dialog)"), helloDlg, NULL, false);
    setCommand(2, TEXT("File Jump"), BTest, NULL, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void hello()
{
    // Open a new document
    ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

    // Get the current scintilla
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return;
    HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;

    // Say hello now :
    // Scintilla control has no Unicode mode, so we use (char *) here
    ::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)"Hello, Notepad++!");
}

void helloDlg()
{
    ::MessageBox(NULL, TEXT("Hello, Notepad++!"), TEXT("Notepad++ Plugin Template"), MB_OK);
}



#include	<string>
#include	<atlstr.h>
#include	<vector>

using namespace std;

class Cntnr
{
	vector<wstring> *	pW;		// 切り出した文字列
	vector<int> *		pI;		// 文字列が""でくくられていた時に not0
	int					nItem;	// 切り出した文字列の数
	public:
	Cntnr () {
		pW = new vector<wstring>();
		pI = new vector<int>();
		nItem = 0;
	}
	~Cntnr () {
		delete	pW;
		delete	pI;
	}
	void set ( wstring s, int i ) {
		pW->push_back( s );
		pI->push_back( i );
		++nItem;
	}
	void get ( int n, wstring & s, int & i ) {
		if( n < 0 ){ i = -1; return; }
		if( n >= nItem ){ i = -1; return; }
		s = *(pW->begin()+n);
		i = *(pI->begin()+n);
	}
};

/*
	UTF8文字列をスペースで切り分ける。
	スペースは、全角半角スペース、改行、タブを認識。
    改行コードは "\r\n", "\r", "\n", "\n\r"の４種類に対応。
*/
Cntnr & ChopStr ( wstring strString )
{

	Cntnr	* pRStr = new Cntnr();

    wstring::iterator ite    = strString.begin();
    wstring::iterator iteEnd = strString.end();
 
    wstring 	strRet;
	int			fSpace = 0;	// スペースを発見すると not0
	int			fFix = 0;	// "" を発見すると not0

    while( ite != iteEnd ) {
 
        wchar_t wChar = *ite++;
 
		if(
			( wChar == ' ' ) ||
			( wChar == L'　' ) ||
			( wChar == '\t' ) ||
			( wChar == '\r' ) ||
			( wChar == '\n' )
		){
			if( fFix ){
				// ""内では空白もあり
				fSpace = 0;
				strRet += wChar;
				continue;
			}
			if( ! fSpace ){
				if( strRet.size() == 0 )	continue;
				pRStr->set( strRet, 0 );	// 文字列ひとつ切り出した
				strRet.erase();
			}
			fSpace = 1;
			continue;
		}// if スペース

		fSpace = 0;

		if( wChar == '"' ){
			if( fFix ){
				pRStr->set( strRet, 1 );	// 文字列ひとつ切り出した
				strRet.erase();
				fFix = 0;
				continue;
			}
			fFix = 1;
			continue;
		}

		strRet += wChar;

    }// while

	if( strRet.size() > 0 )	pRStr->set( strRet, 0 );

    return *pRStr;

}



int exists_file ( wstring path )
{

	WIN32_FIND_DATA		fd;
	HANDLE	h = ::FindFirstFile( path.c_str(), &fd );
	if( h == INVALID_HANDLE_VALUE )	return 0;

	::FindClose( h );

	return 1;

}



void BTest ()
{

	TCHAR *	pBuf = new TCHAR[1000+1];
	if( pBuf == NULL )		return;
	::ZeroMemory( pBuf, sizeof(TCHAR)*(1000+1) );

	LRESULT	r = ::SendMessage( nppData._nppHandle,
								NPPM_GETCURRENTLINESTR, 1000, (LPARAM)pBuf );
	if( r == 0 ){ delete[] pBuf; return; }

	wstring	ws( pBuf );
	Cntnr	vw = ChopStr( ws );
	int		fFoundFile = 0;	// ファイルが見つかったらnot0
	for( int i=0; ; ++i ){

		wstring	w;
		int		d;
		vw.get( i, w, d );
		if( d < 0 )		break;

		if( fFoundFile ){
			break;
		}

		if( exists_file( w ) == 0 )		continue;

		r = ::SendMessage( nppData._nppHandle, NPPM_DOOPEN,
														0, (LPARAM)w.c_str() );
		if( r == 0 )	break;

		fFoundFile = 1;

		//WCHAR	buf[100];
		//TCHAR const *	pB = w.c_str();
		//swprintf( buf, 100, L"%d", d );
		//::MessageBox( NULL, pB, buf, MB_OK );

	}// for i

	/*

	TCHAR *	pDir = new TCHAR[MAX_PATH+1];
	if( pDir == NULL ){
		delete[] pBuf;
		return;
	}
	::ZeroMemory( pDir, sizeof(TCHAR)*(MAX_PATH+1) );

	//r = ::SendMessage( nppData._nppHandle,
	//					NPPM_GETCURRENTDIRECTORY, MAX_PATH, (LPARAM)pDir );

	//::MessageBox( NULL, pDir, TEXT( "File Jump" ), MB_OK );

	CString	d( pDir );
	CString	f( pBuf );
	CString	p = d + TEXT("\\") + f;

	::MessageBox( NULL, (LPCTSTR)p, TEXT( "File Jump" ), MB_OK );

	delete[] pDir;

	*/

	//r = ::SendMessage( nppData._nppHandle,NPPM_DOOPEN, 0, (LPARAM)pB );
	//r = ::SendMessage( nppData._nppHandle,
	//						NPPM_DOOPEN, 0, (LPARAM)TEXT("test.cpp") );
	//r = ::SendMessage( nppData._nppHandle,
	//						NPPM_DOOPEN, 0, (LPARAM)TEXT("こんにちは.cpp") );

	//delete	&vw;
	delete[] pBuf;

}



