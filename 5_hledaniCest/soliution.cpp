#ifndef __PROGTEST__
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <list>
#include <algorithm>

#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#endif /* C++ 11 */

using namespace std;
#endif /* __PROGTEST__ */

class NoRouteException 
{ 
} NRExcp;

template <typename _T, typename _E> class CRoute
{
  public:
    CRoute(void) {}
    ~CRoute(void) {}
    static bool Default (const _E & e) { return true;}
    CRoute<_T, _E> & Add (const _T & u1, const _T & u2, const _E & e);
    list<_T> Find (const _T & start, const _T & finish, const function<bool(const _E & e)> func = CRoute<_T, _E>::Default) const;
  private:
    map<_T, vector<pair<_T, _E>>> db;
    void addNode(_T node);
};

template <typename _T, typename _E>
CRoute<_T, _E> & CRoute<_T, _E>::Add (const _T & u1, const _T & u2, const _E & e)
{
    typedef typename map<_T, vector<pair<_T, _E>>>::iterator mapIterator;
    mapIterator it;

    if ((it = db.find(u1)) == db.end())
    {
      vector<pair<_T,_E>> tmp;
      tmp.push_back(pair<_T,_E>(u2,e));
      db.insert(pair<_T,vector<pair<_T,_E>>>(u1,tmp));
    }
    else
    {
      it->second.push_back(pair<_T, _E>(u2, e));
    }

    if ((it = db.find(u2)) == db.end())
    {
      vector<pair<_T,_E>> tmp;
      tmp.push_back(pair<_T,_E>(u1,e));
      db.insert(pair<_T,vector<pair<_T,_E>>>(u2,tmp));
    }
    else
    {
      it->second.push_back(pair<_T, _E>(u1, e));
    }

    return *this;
}


template <typename _T, typename _E>
list<_T> CRoute<_T, _E>::Find (const _T & start, const _T & finish, const function<bool(const _E & e)> func) const
{
  queue<_T> q;
  map<_T, _T> m;
  set<_T> s;
  list<_T> l;

  if (start == finish)
  {
    l.push_back(start);
    return l;
  }

  if (db.find(start) == db.end() || db.find(finish) == db.end())
  {
    throw NoRouteException();
  }

  q.push(start);
  while(true)
  {
    if (q.front() == finish)
    {
      break;
    }

    s.insert(q.front());

    for (size_t i = 0; i < db.at(q.front()).size(); ++i)
    {
      if (!func(db.at(q.front()).at(i).second))
      {
        continue;
      }
      if (s.find(db.at(q.front()).at(i).first) != s.end())
      {
        continue;
      }

      q.push(db.at(q.front()).at(i).first);
      m.insert(pair<_T, _T>(db.at(q.front()).at(i).first, q.front()));
    }

    q.pop();
    if (q.size() == 0)
    {
      throw NoRouteException();
    }
  }

  _T tmp = m.at(finish);
  l.push_front(finish);
  while(true)
  {
    if (tmp == start)
    {
      break;
    }
    l.push_front(tmp);
    tmp = m.at(tmp);
  }

  l.push_front(start);

  return l;
}


#ifndef __PROGTEST__
//=================================================================================================
class CTrain
{
  public:
    CTrain(const string & company, int speed) : m_Company(company), m_Speed(speed)
      { 
      }
    string                   m_Company;
    int                      m_Speed; 
};
//=================================================================================================
class TrainFilterCompany
{
  public:
                             TrainFilterCompany            ( const set<string> & companies ) 
                             : m_Companies ( companies ) 
    { 
    }
    //---------------------------------------------------------------------------------------------
    bool                     operator ()                   ( const CTrain & train ) const
    { 
      return m_Companies . find ( train . m_Company ) != m_Companies . end (); 
    }
    //---------------------------------------------------------------------------------------------
  private:  
    set <string>             m_Companies;    
};
//=================================================================================================
class TrainFilterSpeed
{
  public:
                             TrainFilterSpeed              ( int               min, 
                                                             int               max )
                             : m_Min ( min ), 
                               m_Max ( max ) 
    { 
    }
    //---------------------------------------------------------------------------------------------
    bool                     operator ()                   ( const CTrain    & train ) const
    { 
      return train . m_Speed >= m_Min && train . m_Speed <= m_Max; 
    }
    //---------------------------------------------------------------------------------------------
  private:  
    int                      m_Min;
    int                      m_Max; 
};
//=================================================================================================
bool NurSchnellzug(const CTrain & zug)
{
  return ( zug . m_Company == "OBB" || zug . m_Company == "DB" ) && zug . m_Speed > 100;
}
//=================================================================================================
static string toText(const list<string> & l)
{
  ostringstream oss;
  
  auto it = l . cbegin();
  oss << *it;
  for ( ++it; it != l . cend (); ++it )
    oss << " > " << *it;
  return oss . str ();
}
//=================================================================================================
int main ( void )
{
  CRoute<string,CTrain> lines;
  
  lines . Add ( "Berlin", "Prague", CTrain ( "DB", 120 ) )
        . Add ( "Berlin", "Prague", CTrain ( "CD",  80 ) )
        . Add ( "Berlin", "Dresden", CTrain ( "DB", 160 ) )
        . Add ( "Dresden", "Munchen", CTrain ( "DB", 160 ) )
        . Add ( "Munchen", "Prague", CTrain ( "CD",  90 ) )
        . Add ( "Munchen", "Linz", CTrain ( "DB", 200 ) )
        . Add ( "Munchen", "Linz", CTrain ( "OBB", 90 ) )
        . Add ( "Linz", "Prague", CTrain ( "CD", 50 ) )
        . Add ( "Prague", "Wien", CTrain ( "CD", 100 ) )
        . Add ( "Linz", "Wien", CTrain ( "OBB", 160 ) )
        . Add ( "Paris", "Marseille", CTrain ( "SNCF", 300 ))
        . Add ( "Paris", "Dresden",  CTrain ( "SNCF", 250 ) );
        
  list<string> r1 = lines . Find ( "Berlin", "Linz" );
  cout << toText(r1);
  assert ( toText ( r1 ) == "Berlin > Prague > Linz" );
  
  list<string> r2 = lines . Find ( "Linz", "Berlin" );
  assert ( toText ( r2 ) == "Linz > Prague > Berlin" );

  list<string> r3 = lines . Find ( "Wien", "Berlin" );
  assert ( toText ( r3 ) == "Wien > Prague > Berlin" );

  list<string> r4 = lines . Find ( "Wien", "Berlin", NurSchnellzug );
  assert ( toText ( r4 ) == "Wien > Linz > Munchen > Dresden > Berlin" );

  list<string> r5 = lines . Find ( "Wien", "Munchen", TrainFilterCompany ( set<string> { "CD", "DB" } ) );
  assert ( toText ( r5 ) == "Wien > Prague > Munchen" );

  list<string> r6 = lines . Find ( "Wien", "Munchen", TrainFilterSpeed ( 120, 200 ) );
  assert ( toText ( r6 ) == "Wien > Linz > Munchen" );

  list<string> r7 = lines . Find ( "Wien", "Munchen", [] ( const CTrain & x ) { return x . m_Company == "CD"; } );
  assert ( toText ( r7 ) == "Wien > Prague > Munchen" );

  list<string> r8 = lines . Find ( "Munchen", "Munchen" );
  assert ( toText ( r8 ) == "Munchen" );
  
  list<string> r9 = lines . Find ( "Marseille", "Prague" );
  assert ( toText ( r9 ) == "Marseille > Paris > Dresden > Berlin > Prague" 
           || toText ( r9 ) == "Marseille > Paris > Dresden > Munchen > Prague" );
  
  try 
  { 
    list<string> r10 = lines . Find ( "Marseille", "Prague", NurSchnellzug ); 
    assert ( "Marseille > Prague connection does not exist!!" == NULL );
  }
  catch ( const NoRouteException & e ) 
  { 
  }
  
  list<string> r11 = lines . Find ( "Salzburg", "Salzburg" );
  assert ( toText ( r11 ) == "Salzburg" );

  list<string> r12 = lines . Find ( "Salzburg", "Salzburg", [] ( const CTrain & x ) { return x . m_Company == "SNCF"; }  );
  assert ( toText ( r12 ) == "Salzburg" );
 
  try 
  { 
    list<string> r13 = lines . Find ( "London", "Oxford" ); 
    assert ( "London > Oxford connection does not exist!!" == NULL );
  }
  catch ( const NoRouteException & e ) 
  { 
  }
  return 0;
}
#endif  /* __PROGTEST__ */
