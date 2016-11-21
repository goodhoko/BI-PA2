#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cassert>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <exception>
#include <typeinfo>

#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */
#include <unordered_map>
#include <unordered_set>
#include <memory>
#endif /* C++ 11 */


using namespace std;

class CSyntaxException : public exception
{
  public: 
    //---------------------------------------------------------------------------------------------
                              CSyntaxException             ( const string    & desc )
                              : m_Desc ( desc )
    {
    }
    //---------------------------------------------------------------------------------------------
    virtual const char      * what                         ( void ) const noexcept
    {
      return m_Desc . c_str ();
    }
    //---------------------------------------------------------------------------------------------
  private:
    string                    m_Desc;    
};
#endif /* __PROGTEST__ */

class CDataType
{
  public:
    CDataType() {};
    virtual ~CDataType() {};
    virtual ostream & print(ostream & os) const = 0;
    size_t Sizeof(void) const {return size;};
    virtual bool operator==(const CDataType & other) const {return name == other.name;};
    virtual bool operator!=(const CDataType & other) const {return name != other.name;};
    string getName(void) const {return name;};
    virtual CDataType * clone(void) const = 0;

  protected:
    friend ostream & operator<<(ostream & os, const CDataType & type) {return type.print(os);};

    size_t size;
    string name;
};

struct structHelper
{
  structHelper(const string & n, const CDataType & t) : name(n), type(&t) {};
  string name;
  const CDataType * type;
};

class CDataTypeInt : public CDataType
{
  public:
    CDataTypeInt(void): CDataType() {size = 4; name = "int";}
    ~CDataTypeInt(void) {}
    ostream & print(ostream & os) const
    {
      os << name;
      return os;
    }

    CDataTypeInt * clone(void) const
    {
      CDataTypeInt * tmp = new CDataTypeInt(*this);
      return tmp;
    }
};

class CDataTypeDouble : public CDataType
{
  public:
    CDataTypeDouble(void): CDataType() {size = 8; name = "double";}
    ~CDataTypeDouble(void) {}
    ostream & print(ostream & os) const
    {
      os << name;
      return os;
    }

    CDataTypeDouble * clone(void) const
    {
      CDataTypeDouble * tmp = new CDataTypeDouble(*this);
      return tmp;
    }
};

class CDataTypeEnum : public CDataType
{
  public:
    CDataTypeEnum(void): CDataType() {size = 4; name = "enum";}
    ~CDataTypeEnum(void) {}

    CDataTypeEnum& Add(const string & x)
    {
      for(vector<string>::iterator it = db.begin(); it != db.end(); ++it)
      {
        if(*it == x)
        {
          CSyntaxException e("Duplicate enum value: " + x);
          throw e;
        }
      }
      db.push_back(x);
      return *this;
    }

    ostream & print(ostream & os) const
    {
      os << name << "{\n";
      for(vector<string>::const_iterator it = db.begin(); it != db.end(); ++it)
      {
        os << *it;
        if (it != db.end() - 1)
         {
           os << ",";
         }
        os << "\n";
      }
      os << "}";
      return os;
    }

    bool operator==(const CDataType & other) const
    {
      if (name != other.getName())
        return false;

      const CDataTypeEnum & tmp = static_cast<const CDataTypeEnum&>(other);
      if (db.size() != tmp.db.size())
      {
        return false;
      }

      for (vector<string>::const_iterator it = db.begin(); it != db.end(); ++it)
      {
        if (*it != tmp.db[it-db.begin()])
          return false;
      }
      return true;
    }


     bool operator!=(const CDataType & other) const
    {
      if (name != other.getName())
        return true;

      const CDataTypeEnum & tmp = static_cast<const CDataTypeEnum&>(other);
      if (db.size() != tmp.db.size())
      {
        return true;
      }

      for (vector<string>::const_iterator it = db.begin(); it != db.end(); ++it)
      {
        if (*it != tmp.db[it-db.begin()])
          return true;
      }
      return false;
    }

    CDataTypeEnum * clone(void) const
    {
      CDataTypeEnum * tmp = new CDataTypeEnum(*this);
      return tmp;
    }

    // default constructor
    // Add ( )
    // Sizeof () -> size_t
    // operator == 
    // operator !=
    // operator <<
  private:
    vector<string> db;

};

class CDataTypeStruct : public CDataType
{
  public:
    CDataTypeStruct(void): CDataType() { size = 0; name = "struct";}
    ~CDataTypeStruct(void) {}

    CDataTypeStruct & AddField(const string& name, const CDataType & type)
    {
      CDataType * tmp = type.clone();
      shared_ptr<CDataType> tmpp (tmp);

      for(vector<pair<string, shared_ptr<CDataType> > >::iterator it = db.begin(); it !=  db.end(); ++it)
      {
        if (it->first == name)
        {
          throw CSyntaxException("Duplicate field: " + name);
        }
      }
      pair<string, shared_ptr<CDataType> > tmps = make_pair(name, tmpp);
      db.push_back(tmps);
      size += tmp->Sizeof();
      return *this;
    }

    CDataType & Field(const string& n) const
    {
      for(vector<pair<string, shared_ptr<CDataType> > >::const_iterator it = db.begin(); it !=  db.end(); ++it)
      {
        if (it->first == n)
        {
          return *(it->second);
        }
      }
      throw CSyntaxException("Unknown field: " + n);
    }

    bool operator==(const CDataType & other) const
    {
      if (name != other.getName())
        return false;

      const CDataTypeStruct & tmp = static_cast<const CDataTypeStruct&>(other);

      if (db.size() != tmp.db.size())
        return false;

      for (unsigned int i = 0; i < db.size(); ++i)
      {
        if (*(db[i].second) != *(tmp.db[i].second))
          return false;
      }

      return true;
    }

    bool operator!=(const CDataType & other) const
    {
      if (name != other.getName())
        return true;

      const CDataTypeStruct & tmp = static_cast<const CDataTypeStruct&>(other);

      if (db.size() != tmp.db.size())
      {
        return true;
      }

      for (unsigned int i = 0; i < db.size(); ++i)
      {
        if (db[i].second != tmp.db[i].second)
        return true;
      }

      return false;
    }

    ostream & print(ostream & os) const
    {
      os << name << "{\n";
      for(vector<pair<string, shared_ptr<CDataType> > >::const_iterator it = db.begin(); it !=  db.end(); ++it)
      {
        it->second->print(os) << " " << it->first << ";\n";
      }
      os << "}";
      return os;
    }

    CDataTypeStruct * clone(void) const
    {
      CDataTypeStruct * tmp = new CDataTypeStruct(*this);
      return tmp;
    }

  private:
    vector<pair<string, shared_ptr<CDataType> > > db;
    //vector<structHelper> db;
};


#ifndef __PROGTEST__ 
//-------------------------------------------------------------------------------------------------
static bool        whitespaceMatch                         ( const string    & out,
                                                             const string    & ref )
{
  const char * o = out . c_str ();
  const char * r = ref . c_str ();
  
  while ( 1 )
  {
    while ( isspace ( *o )) o ++;
    while ( isspace ( *r )) r ++;
    if ( ! *r && ! *o ) return true;
    if ( ! *r || ! *o || *r++ != *o++ ) return false;
  }
}
//-------------------------------------------------------------------------------------------------
template <typename _T>
static bool        whitespaceMatch                         ( const _T        & x,
                                                             const string    & ref )
{
  ostringstream oss;
  oss << x;
  return whitespaceMatch ( oss . str (), ref );
}
//-------------------------------------------------------------------------------------------------

int                main                                    ( void )
{
  CDataTypeStruct  a = CDataTypeStruct () .
                        AddField ( "m_Length", CDataTypeInt () ) .
                        AddField ( "m_Status", CDataTypeEnum () . 
                          Add ( "NEW" ) . 
                          Add ( "FIXED" ) . 
                          Add ( "BROKEN" ) . 
                          Add ( "DEAD" ) ).
                        AddField ( "m_Ratio", CDataTypeDouble () );
  
  CDataTypeStruct b = CDataTypeStruct () .
                        AddField ( "m_Length", CDataTypeInt () ) .
                        AddField ( "m_Status", CDataTypeEnum () . 
                          Add ( "NEW" ) . 
                          Add ( "FIXED" ) . 
                          Add ( "BROKEN" ) . 
                          Add ( "READY" ) ).
                        AddField ( "m_Ratio", CDataTypeDouble () );
  
  CDataTypeStruct c =  CDataTypeStruct () .
                        AddField ( "m_First", CDataTypeInt () ) .
                        AddField ( "m_Second", CDataTypeEnum () . 
                          Add ( "NEW" ) . 
                          Add ( "FIXED" ) . 
                          Add ( "BROKEN" ) . 
                          Add ( "DEAD" ) ).
                        AddField ( "m_Third", CDataTypeDouble () );

  CDataTypeStruct  d = CDataTypeStruct () .
                        AddField ( "m_Length", CDataTypeInt () ) .
                        AddField ( "m_Status", CDataTypeEnum () . 
                          Add ( "NEW" ) . 
                          Add ( "FIXED" ) . 
                          Add ( "BROKEN" ) . 
                          Add ( "DEAD" ) ).
                        AddField ( "m_Ratio", CDataTypeInt () );
  
  assert ( whitespaceMatch ( a, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "}") );

  assert ( whitespaceMatch ( b, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    READY\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "}") );

  assert ( whitespaceMatch ( c, "struct\n"
    "{\n"
    "  int m_First;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Second;\n"
    "  double m_Third;\n"
    "}") );

  assert ( whitespaceMatch ( d, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  int m_Ratio;\n"
    "}") );

  assert ( a != b );
  assert ( a == c );
  assert ( a != d );
  assert ( a != CDataTypeInt() );
  assert ( whitespaceMatch ( a . Field ( "m_Status" ), "enum\n"
    "{\n"
    "  NEW,\n"
    "  FIXED,\n"
    "  BROKEN,\n"
    "  DEAD\n"
    "}") );

  b . AddField ( "m_Other", CDataTypeDouble ());

  a . AddField ( "m_Sum", CDataTypeInt ());

  assert ( whitespaceMatch ( a, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    DEAD\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "  int m_Sum;\n"
    "}") );

  assert ( whitespaceMatch ( b, "struct\n"
    "{\n"
    "  int m_Length;\n"
    "  enum\n"
    "  {\n"
    "    NEW,\n"
    "    FIXED,\n"
    "    BROKEN,\n"
    "    READY\n"
    "  } m_Status;\n"
    "  double m_Ratio;\n"
    "  double m_Other;\n"
    "}") );
  assert ( a . Sizeof () == 20 );
  assert ( b . Sizeof () == 24 );
  try
  {
    a . AddField ( "m_Status", CDataTypeInt () );
    assert ( "AddField: missing exception!" == NULL );
  }
  catch ( const exception & e )
  {
    assert ( string ( e . what () ) == "Duplicate field: m_Status" );
  }

  try
  {
    cout << a . Field ( "m_Fail" ) << endl;
    assert ( "Field: missing exception!" == NULL );
  }
  catch ( const exception & e )
  {
    assert ( string ( e . what () ) == "Unknown field: m_Fail" );
  }
  
  try
  {
    CDataTypeEnum en;
    en . Add ( "FIRST" ) .
         Add ( "SECOND" ) .
         Add ( "FIRST" );
    assert ( "Add: missing exception!" == NULL );
  }
  catch ( const exception & e )
  {
    assert ( string ( e . what () ) ==  "Duplicate enum value: FIRST" );
  }
  
  return 0;
}
#endif /* __PROGTEST__ */
