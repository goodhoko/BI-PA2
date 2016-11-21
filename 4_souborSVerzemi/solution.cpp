#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <stdint.h>
using namespace std;
#endif /* __PROGTEST__ */

struct Version
{
	char* data;
	uint32_t pos;
	uint32_t size;
};

class CFile
{
	public:
    	CFile(void);
    	CFile(const CFile& other);
    	~CFile(void);
    	CFile &operator=(const CFile &other);
    	bool Seek ( uint32_t offset );
    	uint32_t Read ( uint8_t* dst, uint32_t bytes );
    	uint32_t Write ( const uint8_t* src, uint32_t bytes );
    	void Truncate ( void );
    	uint32_t FileSize ( void ) const;
    	void AddVersion ( void );
    	bool UndoVersion ( void );
    	void Print(void);
	private:
		Version* db;
		uint32_t versionCnt;
};

CFile::CFile(void) : db(NULL), versionCnt(0)
{
}

CFile::CFile(const CFile& other)
{
	versionCnt = other.versionCnt;
	if (versionCnt == 0)
	{
		db = NULL;
		return;
	}

	db = (Version*)realloc(NULL, versionCnt * sizeof(Version));

	for (uint32_t i = 0; i < versionCnt; ++i)
	{
		db[i].pos = other.db[i].pos;
		db[i].size = other.db[i].size;

		if (db[i].size == 0)
		{
			db[i].data = NULL;
		}

		db[i].data = (char*)realloc(NULL, db[i].size);
		for (uint32_t j = 0; j < db[i].size; ++j)
		{
			db[i].data[j] = other.db[i].data[j];
		}	
	}
}

CFile::~CFile(void)
{
	for (uint32_t i = 0; i < versionCnt; ++i)
	{
		delete[] db[i].data;
	}

	delete[] db;
}

CFile& CFile::operator=(const CFile &other)
{	
	versionCnt = other.versionCnt;
	if (versionCnt == 0)
	{
		db = NULL;
		return *this;
	}

	db = (Version*)realloc(NULL, versionCnt * sizeof(Version));

	for (uint32_t i = 0; i < versionCnt; ++i)
	{
		db[i].pos = other.db[i].pos;
		db[i].size = other.db[i].size;

		if (db[i].size == 0)
		{
			db[i].data = NULL;
		}

		db[i].data = (char*)realloc(NULL, db[i].size);
		for (uint32_t j = 0; j < db[i].size; ++j)
		{
			db[i].data[j] = other.db[i].data[j];
		}	
	}

	return *this;
}

bool CFile::Seek(uint32_t offset)
{

	if ( db == NULL || offset < 0 || offset > db[versionCnt - 1].size)
	{
		return false;
	}

	db[versionCnt - 1].pos = offset;
	return true;
}

uint32_t CFile::Read(uint8_t* dst, uint32_t bytes)
{
	if (db == NULL || dst == NULL || bytes == 0)
		return 0;
	uint32_t x = (db[versionCnt - 1].size < (db[versionCnt - 1].pos + bytes)) ? db[versionCnt - 1].size - db[versionCnt - 1].pos : bytes;
	uint32_t i;
	for (i = 0; i < x; ++i)
	{
		dst[i] = db[versionCnt - 1].data[db[versionCnt - 1].pos + i];
	}

	db[versionCnt - 1].pos += x;

	return i;
}

uint32_t CFile::Write(const uint8_t* src, uint32_t bytes)
{
	if (src == NULL || bytes == 0)
	{
		return 0;
	}

	if (db == NULL)
	{
		this->AddVersion();
	}

	if (db[versionCnt - 1].size < (db[versionCnt - 1].pos + bytes))
	{
		db[versionCnt - 1].data = (char*)realloc(db[versionCnt - 1].data, db[versionCnt - 1].pos + bytes);
		db[versionCnt - 1].size = (db[versionCnt - 1].pos + bytes);
	}

	uint32_t i;
	for (i = 0; i < bytes; ++i)
	{
		db[versionCnt - 1].data[db[versionCnt - 1].pos + i] = src[i];
	}
	
	db[versionCnt - 1].pos += i;

	return i;
}

void CFile::Truncate ( void )
{
	db[versionCnt - 1].data = (char*)realloc(db[versionCnt - 1].data, db[versionCnt - 1].pos);
	db[versionCnt - 1].size = db[versionCnt - 1].pos;
}

uint32_t CFile::FileSize ( void ) const
{
	return db[versionCnt - 1].size;
}

void CFile::AddVersion ( void )
{
	db = (Version*)realloc(db, ++versionCnt * sizeof(Version));
	if (versionCnt == 1)
	{
		db[versionCnt - 1].data = NULL;
		db[versionCnt - 1].pos = 0;
		db[versionCnt - 1].size = 0;
		return;
	}

	db[versionCnt - 1].pos = db[versionCnt - 2].pos;
	db[versionCnt - 1].size = db[versionCnt - 2].size;
	db[versionCnt - 1].data = NULL;
	db[versionCnt - 1].data = (char*)realloc(db[versionCnt - 1].data, db[versionCnt - 1].size);
	for (uint32_t i = 0; i < db[versionCnt - 1].size; ++i)
	{
		db[versionCnt - 1].data[i] = db[versionCnt - 2].data[i]; 
	}
	
	return;
}

bool CFile::UndoVersion ( void )
{
	if (versionCnt == 1)
	{
		return false;
	}

	delete[] db[versionCnt - 1].data;
	db = (Version*)realloc(db, --versionCnt * sizeof(Version));
	return true;
}

void CFile::Print(void)
{
	cout << "pocet verzi: " << versionCnt << endl;
	cout << "size: " << db[versionCnt - 1].size << endl;
	cout << "position: " << db[versionCnt - 1].pos << endl;
}



//-------------------------------------------------------------------------------------------------

#ifndef __PROGTEST__
bool writeTest (CFile& x, const initializer_list<uint8_t> & data, uint32_t wrLen )
{
  uint8_t  tmp[100];
  uint32_t idx = 0;

  for ( auto v : data )
    tmp[idx++] = v;
  return x . Write ( tmp, idx ) == wrLen;
}

bool readTest (CFile& x, const initializer_list<uint8_t> & data, uint32_t rdLen)
{
  uint8_t  tmp[100];
  uint32_t idx = 0;

  if ( x . Read ( tmp, rdLen ) != data . size ())
  	{
  		return false;
  	}
  for ( auto v : data )
  {
    if ( tmp[idx++] != v )
    {
    	return false;
    }
  }
  return true;
}

int main ( void )
{

	CFile f0;
	assert ( writeTest ( f0, { 10, 20, 30 }, 3 ) );
	assert ( f0 . FileSize () == 3 );
	assert ( writeTest ( f0, { 60, 70, 80 }, 3 ) );
	assert ( f0 . FileSize () == 6 );
	assert ( f0 . Seek ( 2 ));
	assert ( writeTest ( f0, { 5, 4 }, 2 ) );
	//cout << "file size: " << f0.FileSize() << endl;
	assert ( f0 . FileSize () == 6 );
	assert ( f0 . Seek ( 1 ));
	assert ( readTest ( f0, { 20, 5, 4, 70, 80 }, 7 ));
	assert ( f0 . Seek ( 3 ));
	f0 . AddVersion();
	assert ( f0 . Seek ( 6 ));
	assert ( writeTest ( f0, { 100, 101, 102, 103 }, 4 ) );
	f0 . AddVersion();
	assert ( f0 . Seek ( 5 ));
	CFile f1 ( f0 );
	f0 . Truncate ();
	assert ( f0 . Seek ( 0 ));
	assert ( readTest ( f0, { 10, 20, 5, 4, 70 }, 20 ));
	assert ( f0 . UndoVersion () );
	assert ( f0 . Seek ( 0 ));
	assert ( readTest ( f0, { 10, 20, 5, 4, 70, 80, 100, 101, 102, 103 }, 20 ));
	assert ( f0 . UndoVersion () );
	assert ( f0 . Seek ( 0 ));
	assert ( readTest ( f0, { 10, 20, 5, 4, 70, 80 }, 20 ));
	assert ( !f0 . Seek ( 100 ));
	assert ( writeTest ( f1, { 200, 210, 220 }, 3 ) );
	assert ( f1 . Seek ( 0 ));
	assert ( readTest ( f1, { 10, 20, 5, 4, 70, 200, 210, 220, 102, 103 }, 20 ));
	assert ( f1 . UndoVersion () );
	assert ( f1 . UndoVersion () );
	assert ( readTest ( f1, { 4, 70, 80 }, 20 ));
	assert ( !f1 . UndoVersion () );

	return 0;
}
#endif /* __PROGTEST__ */
