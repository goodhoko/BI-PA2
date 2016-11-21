#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */
#include <memory>
#endif /* C++ 11 */
using namespace std;
#endif /* __PROGTEST__ */

class CPolynomial
{
  public:
    CPolynomial();
    double& operator[](const int index);
    const double operator[](const int index) const;
    const double operator()(const double value) const;
    const int Degree() const;
    friend ostream& operator<<(ostream& out, const CPolynomial& a);
    friend CPolynomial operator*(const double a, CPolynomial b);
    friend CPolynomial operator*(CPolynomial b, const double a);    
    friend CPolynomial operator+(const CPolynomial& a, const CPolynomial& b);
    friend CPolynomial operator-(const CPolynomial& a, const CPolynomial& b);
    friend CPolynomial operator*(const CPolynomial& a, const CPolynomial& b);
    friend bool operator!=(const CPolynomial& a, const CPolynomial& b);
    friend bool operator==(const CPolynomial& a, const CPolynomial& b);

  private:
    vector<double> polynom;
};
 
//----------------------------------------------------------------------------------

CPolynomial::CPolynomial()
{
}

double& CPolynomial::operator[](const int index)
{
  try
  {
    return polynom.at(index);
  }
  catch(...)
  {
    polynom.resize(index + 1, 0);
    return polynom[index];
  }

}

const double CPolynomial::operator[](const int index) const
{
  try
  {
    return polynom.at(index);
  }catch(...)
  {
    return 0;
  }
  
}

const double CPolynomial::operator()(const double value) const
{
  double res = 0;
  for (vector<double>::const_iterator it = polynom.begin(); it != polynom.end(); ++it)
  {
    res += *it * (pow(value, distance(polynom.begin(), it)));
  }

  return res;
}

const int CPolynomial::Degree() const
{
  int res = polynom.size() -1;
  for (vector<double>::const_reverse_iterator rit = polynom.rbegin(); rit != polynom.rend(); ++rit, --res)
  {
    if (*rit != 0)
    {
      return res;
    }
  }

  return 0;
}

ostream& operator<<(ostream& out, const CPolynomial& a)
{
  if (a.polynom.size() == 0)
  {
    out << "0";
    return out;
  }

  for (vector<double>::const_reverse_iterator it = a.polynom.rbegin(); it != a.polynom.rend(); ++it)
  {

    if (fabs(*it) < DBL_EPSILON) //
    {
      continue;
    }

    if (it != a.polynom.rbegin())
    {
      if (*it < 0)
        out << " - ";
      else
        out << " + ";
      if (*it != 1 && *it != -1)
      {
        out << fabs(*it);
      }

      if ((*it != 1 && *it != -1) && distance(it, a.polynom.rend()) != 1)
      {
        out<< "*";
      }

      if (distance(it, a.polynom.rend()) != 1)
      {
        out << "x^" << distance(it, a.polynom.rend()) - 1;
      }
      
    }
    else
      {
        if (*it < 0)
         out << "- ";
        if (*it != 1 && *it != -1)
        {
          out << fabs(*it);
        }

        if ((*it != 1 && *it != -1) && distance(it, a.polynom.rend()) != 1)
        {
          out << "*";
        }

        if (distance(it, a.polynom.rend()) != 1)
        {
          out << "x^" << distance(it, a.polynom.rend()) - 1;
        }
      }

  }

  return out;
}

CPolynomial operator*(const double a, CPolynomial b)
{
  for (vector<double>::iterator it = b.polynom.begin(); it != b.polynom.end(); ++it)
  {
    *it *= a;
  }

  return b;
}

CPolynomial operator*(CPolynomial b, const double a)
{
  for (vector<double>::iterator it = b.polynom.begin(); it != b.polynom.end(); ++it)
  {
    *it *= a;
  }

  return b;
}

CPolynomial operator+(const CPolynomial& a, const CPolynomial& b)
{
  CPolynomial res;
  int degree = a.Degree() < b.Degree()? b.Degree() : a.Degree();
  for (int i = 0; i < degree + 1; ++i)
  {
    res[i] = a[i] + b[i];
  }

  return res;
}

CPolynomial operator-(const CPolynomial& a, const CPolynomial& b)
{
  CPolynomial res;

  int degree = a.Degree() < b.Degree()? b.Degree() : a.Degree();
  for (int i = 0; i < degree + 1; ++i)
  {
    res[i] = a[i] - b[i];
  }

  return res;
}

CPolynomial operator*(const CPolynomial& a, const CPolynomial& b)
{
  CPolynomial res;
  int degreeA = a.Degree();
  int degreeB = b.Degree();
  for (int i = 0; i < degreeA + 1; ++i)
  {
    for (int j = 0; j < degreeB + 1; ++j)
    {
      res[i + j] += a[i] * b[j];
    }
  }

  return res;
}

bool operator!=(const CPolynomial& a, const CPolynomial& b)
{
  if (a.Degree() != b.Degree())
  {
    return true;
  }

  for (int i = 0; i < a.Degree(); ++i)
  {
    if (a[i] != b[i])
    {
      return true;
    }
  }

  return false;
}

bool operator==(const CPolynomial& a, const CPolynomial& b)
{
  if (a.Degree() != b.Degree())
  {
    return false;
  }

  for (int i = 0; i < a.Degree(); ++i)
  {
    if (a[i] != b[i])
    {
      return false;
    }
  }

  return true;
}




//----------------------------------------------------------------------------------

#ifndef __PROGTEST__
bool smallDiff( double a, double b )
{
  return fabs(a - b) < 1e-10;
}

bool dumpMatch( const CPolynomial & x, const vector<double> & ref )
{
  // DIY 
  return  true;
}

int main( void )
{
  CPolynomial a, b, c;
  ostringstream out;

  a[0] = -10;
  a[1] = 3.5;
  a[3] = 1;
  assert ( smallDiff ( a ( 2 ), 5 ) );
  out . str ("");
  out << a;
  assert ( out . str () == "x^3 + 3.5*x^1 - 10" );
  a = a * -2;
  assert ( a . Degree () == 3
           && dumpMatch ( a, vector<double>{ 20.0, -7.0, -0.0, -2.0 } ) );

  out . str ("");
  out << a;
  assert ( out . str () == "- 2*x^3 - 7*x^1 + 20" );
  out . str ("");
  out << b;
  assert ( out . str () == "0" );
  b[5] = -1;
  out . str ("");
  out << b;
  assert ( out . str () == "- x^5" );
  c = a + b;
  assert ( c . Degree () == 5
           && dumpMatch ( c, vector<double>{ 20.0, -7.0, 0.0, -2.0, 0.0, -1.0 } ) );

  out . str ("");
  out << c;
  assert ( out . str () == "- x^5 - 2*x^3 - 7*x^1 + 20" );
  c = a - b;
  assert ( c . Degree () == 5
           && dumpMatch ( c, vector<double>{ 20.0, -7.0, -0.0, -2.0, -0.0, 1.0 } ) );

  out . str ("");
  out << c;
  assert ( out . str () == "x^5 - 2*x^3 - 7*x^1 + 20" );
  c = a * b;
  assert ( c . Degree () == 8
           && dumpMatch ( c, vector<double>{ -0.0, -0.0, 0.0, -0.0, 0.0, -20.0, 7.0, -0.0, 2.0 } ) );

  out . str ("");
  out << c;
  assert ( out . str () == "2*x^8 + 7*x^6 - 20*x^5" );
  assert ( a != b );
  b[5] = 0;
  assert ( !(a == b) );
  a = a * 0;
  assert ( a . Degree () == 0
           && dumpMatch ( a, vector<double>{ 0.0 } ) );

  assert ( a == b );
  return 0;
}
#endif /* __PROGTEST__ */
