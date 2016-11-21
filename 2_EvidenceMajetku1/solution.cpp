// lower_bound, upper_bound

#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */


struct property{
    int id;
    string m_name;
    string m_deptName;
    string m_city;
    string m_country;
    property *next;
};

bool compareId(property *a, property *b)
{
    return a->id < b->id;
}

bool compareDept(property *a, property *b)
{
    if (a->m_deptName == b->m_deptName)
    {
        if (a->m_city == b->m_city)
        {
            if (a->m_country == b->m_country)
            {
                return a->id < b->id;
            }
            return a->m_country < b->m_country;
        }
        return a->m_city < b->m_city;
    }
    return a->m_deptName < b->m_deptName;
}

bool compareDept2(const property *a, const property *b)
{
    if (a->m_deptName == b->m_deptName)
    {
        if (a->m_city == b->m_city)
        {
            return a->m_country < b->m_country;
        }
        return a->m_city < b->m_city;
    }
    return a->m_deptName < b->m_deptName;
}

void printProp(property *a)
{
    cout<< "PROPERTY" << endl;
    cout << "id: " << a->id << endl;
    cout << "name: " << a->m_name << endl;
    cout << "deptName: " << a->m_deptName << endl;
    cout << "city: " << a->m_city << endl;
    cout << "country: " << a->m_country << "\n" << endl;
}


//---------------------------------------------------------------------------------

class CPropList{
public:
    CPropList(void) : pos(0)
    {
    }

    void addProp(property* prop)
    {
        db.push_back(prop);
    }

    int InvID(void) const
    {
        return db[pos]->id;
    }
    string Name(void) const
    {
        return db[pos]->m_name;
    }
    int Remains(void) const
    {
        return db.size() - pos;
    }
    void Next(void)
    {
        pos++;
    }

private:
    vector<property*> db;
    int pos;




};

//---------------------------------------------------------------------------------

class CInventory {
public:
    CInventory(void) : memberCount(0)
    {
    }
    ~CInventory(void)
    {
        for (vector<property*>::iterator it = byId.begin(); it != byId.end(); ++it)
        {
            delete *it;
        }
    }


#if defined (__cplusplus) && __cplusplus > 199711L  /* C++ 11 */
    // copying is not required / tested
    CInventory(const CInventory& src) = delete;
    CInventory & operator=(const CInventory& src) = delete;
#endif /* C++ 11 */



    bool AddProp(int invID, const string & name, const string & deptName, const string & city, const string & country)
    {
        if (byId.empty())
        {
            property *first = new property;

            first->id = invID;
            first->m_name = name;
            first->m_deptName = deptName;
            first->m_city = city;
            first->m_country = country;

            byId.push_back(first);
            byDept.push_back(first);
            memberCount++;

            return true;
        }

        property *tmpAdd = new property;
        tmpAdd->id = invID;
        tmpAdd->m_name = name;
        tmpAdd->m_deptName = deptName;
        tmpAdd->m_city = city;
        tmpAdd->m_country = country;

        vector<property*>::iterator it = lower_bound(byId.begin(), byId.end(), tmpAdd, compareId);



        if (it == byId.end())
        {
            byId.insert(it, tmpAdd);

            it = lower_bound(byDept.begin(), byDept.end(), tmpAdd, compareDept);
            byDept.insert(it, tmpAdd);

            memberCount++;

            return true;
        }

        property *tmpComp = *it;


        //printProp(tmpComp);

        if (tmpAdd->id == tmpComp->id)
        {
            delete tmpAdd;
            return false;
        }

        byId.insert(it, tmpAdd);

        it = lower_bound(byDept.begin(), byDept.end(), tmpAdd, compareDept);
        byDept.insert(it, tmpAdd);
        //printProp(*it);

        memberCount++;

        return true;
    }


    bool DelProp(int invID)
    {
        property *tmp = new property;
        tmp->id = invID;

        vector<property*>::iterator it = lower_bound(byId.begin(), byId.end(), tmp, compareId);
        if(it == byId.end())
        {
            delete tmp;
            return false;
        }

        property *tmp2 = *it;
        if (tmp2->id == tmp->id)
        {
            byId.erase(it);
            byDept.erase(lower_bound(byDept.begin(), byDept.end(), tmp2, compareDept));
            delete tmp2;
            delete tmp;
            memberCount--;
            return true;
        }

        delete tmp;
        return false;
    }


    bool Transfer(int invID, const string & deptName, const string & city, const string & country)
    {
        property *tmp = new property;
        tmp->id = invID;
        tmp->m_deptName = deptName;
        tmp->m_city = city;
        tmp->m_country = country;


        vector<property*>::iterator it = lower_bound(byId.begin(), byId.end(), tmp, compareId);
        if(it == byId.end())
        {
            delete tmp;
            return false;
        }
        property *tmp2 = *it;
        if (tmp->id == tmp2->id) // stejné id
        {
            if (deptName == tmp2->m_deptName && city == tmp2->m_city && country == tmp2->m_country)
            {
                delete tmp;
                return false;
            }
            tmp->m_name = tmp2->m_name;
            DelProp(invID);
            AddProp(invID, tmp->m_name, deptName, city, country);
            delete tmp;
            return true;
        }

        delete tmp;
        return false;
    }


    CPropList PropList(const string & deptName, const string & city, const string & country) const
    {
        CPropList res;
        property *tmp = new property;
        tmp->m_deptName = deptName;
        tmp->m_city = city;
        tmp->m_country = country;

        for (vector<property*>::const_iterator it = lower_bound(byDept.begin(), byDept.end(), tmp, compareDept2); it != upper_bound(byDept.begin(), byDept.end(), tmp, compareDept2); ++it)
        {
            res.addProp(*it);
        }
        delete tmp;
        return res;
    }


    int PropCount(const string & deptName, const string & city, const string & country) const
    {
        property *tmp = new property;
        tmp->m_deptName = deptName;
        tmp->m_city = city;
        tmp->m_country = country;

        vector<property*>::const_iterator lower = lower_bound(byDept.begin(), byDept.end(), tmp, compareDept2);
        vector<property*>::const_iterator upper = upper_bound(byDept.begin(), byDept.end(), tmp, compareDept2);
        delete tmp;
        return upper - lower;
    }

    void printById(void)
    {
        cout << "PRINT BY ID" << endl;
        for (vector<property*>::iterator it = byDept.begin(); it != byDept.end(); ++it)
        {
            printProp(*it);
        }
    }

private:
    vector<property*> byId;
    vector<property*> byDept;
    int memberCount;



};


//---------------------------------------------------------------------------------


#ifndef __PROGTEST__

int main(void) {
    CInventory b1;
    assert(b1 . AddProp(100, "Notebook", "Accounting", "Prague", "CZ") == true);
    assert(b1 . AddProp(101, "Notebook", "Human Resources", "Prague", "CZ") == true);
    assert(b1 . AddProp(102, "Notebook", "Accounting", "Brno", "CZ") == true);
    assert(b1 . AddProp(103, "Notebook", "Accounting", "Prague", "USA") == true);
    assert(b1 . AddProp(104, "Desktop PC", "Accounting", "Prague", "CZ") == true);
    assert(b1 . PropCount("Accounting", "Prague", "CZ") == 2);


    for (CPropList l = b1 . PropList("Accounting", "Prague", "CZ"); l . Remains(); l . Next())
        cout << l . InvID() << ' ' << l . Name() << endl;
    // the following 2 items, in any order:

    // 100 Notebook
    // 104 Desktop PC

    assert(b1 . PropCount("Human Resources", "Prague", "CZ") == 1);
    for (CPropList l = b1 . PropList("Human Resources", "Prague", "CZ"); l . Remains(); l . Next())
        cout << l . InvID() << ' ' << l . Name() << endl;
    // the following property

    // 101 Notebook

    assert(b1 . PropCount("Accounting", "Brno", "CZ") == 1);
    for (CPropList l = b1 . PropList("Accounting", "Brno", "CZ"); l . Remains(); l . Next())
        cout << l . InvID() << ' ' << l . Name() << endl;
    // the following property

    // 102 Notebook

    assert(b1 . PropCount("Accounting", "Prague", "USA") == 1);
    for (CPropList l = b1 . PropList("Accounting", "Prague", "USA"); l . Remains(); l . Next())
        cout << l . InvID() << ' ' << l . Name() << endl;
    // the following property

    // 103 Notebook

    assert(b1 . Transfer(104, "Accounting", "Prague", "USA") == true);
    assert(b1 . PropCount("Accounting", "Prague", "CZ") == 1);
    for (CPropList l = b1 . PropList("Accounting", "Prague", "CZ"); l . Remains(); l . Next())
        cout << l . InvID() << ' ' << l . Name() << endl;
    // the following property

    // 100 Notebook

    assert(b1 . PropCount("Accounting", "Prague", "USA") == 2);
    for (CPropList l = b1 . PropList("Accounting", "Prague", "USA"); l . Remains(); l . Next())
        cout << l . InvID() << ' ' << l . Name() << endl;
    // the following 2 items, in any order:

    // 103 Notebook
    // 104 Desktop PC

    assert(b1 . DelProp(104) == true);
    assert(b1 . PropCount("Accounting", "Prague", "USA") == 1);
    for (CPropList l = b1 . PropList("Accounting", "Prague", "USA"); l . Remains(); l . Next())
        cout << l . InvID() << ' ' << l . Name() << endl;
    // the following property

    // 103 Notebook

    assert(b1 . AddProp(104, "Chair", "Public relations", "Paris", "France") == true);
    assert(b1 . PropCount("Public relations", "Paris", "France") == 1);
    for (CPropList l = b1 . PropList("Public relations", "Paris", "France"); l . Remains(); l . Next())
        cout << l . InvID() << ' ' << l . Name() << endl;
    // the following property

    // 104 Chair


    CInventory b2;
    assert(b2 . AddProp(100, "Notebook", "Accounting", "Prague", "CZ") == true);

    assert(b2 . AddProp(101, "Notebook", "Human Resources", "Prague", "CZ") == true);

    assert(b2 . AddProp(100, "Table", "CEO Office", "Prague", "CZ") == false);

    assert(b2 . DelProp(102) == false);

    assert(b2 . Transfer(102, "CEO Office", "Prague", "CZ") == false);

    assert(b2 . Transfer(100, "Accounting", "Prague", "CZ") == false);
    assert(b2 . DelProp(100) == true);
    assert(b2 . DelProp(100) == false);
    assert(b2 . PropCount("Facility Services", "Prague", "CZ") == 0);
    for (CPropList l = b2 . PropList("CEO Office", "Prague", "USA"); l . Remains(); l . Next())
        cout << l . InvID() << ' ' << l . Name() << endl;
    // empty output

    return 0;
}
#endif /* __PROGTEST__ */

