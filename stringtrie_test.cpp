// junk.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <fstream>
#include "stringtrie.h"

using namespace std;

void loadPTable(vector<string>& vec)
{
    ifstream strm;
    strm.open("test_TTProdTbl_CME-D_SIM .dat");
    while (strm.good())
    {
        char buf[255];
        strm.getline(buf, sizeof(buf), ';');
        if (strm.good())
        {
            vec.push_back(buf);
        }
        strm.getline(buf, sizeof(buf), '\n');
    }
}

void loadPTable(map<string, int>& m)
{
    ifstream strm;
    strm.open("test_TTProdTbl_CME-D_SIM .dat");
    while (strm.good())
    {
        char buf[255];
        strm.getline(buf, sizeof(buf), ';');
        if (strm.good())
        {
            m.insert(map<string, int>::value_type(buf, 0));
        }
        strm.getline(buf, sizeof(buf), '\n');
    }
}

void loadPTable(unordered_map<string, int>& m)
{
    ifstream strm;
    strm.open("test_TTProdTbl_CME-D_SIM .dat");
    while (strm.good())
    {
        char buf[255];
        strm.getline(buf, sizeof(buf), ';');
        if (strm.good())
        {
            m.insert(unordered_map<string, int>::value_type(buf, 0));
        }
        strm.getline(buf, sizeof(buf), '\n');
    }
}

void loadPTable(stringtrie<int>& trie)
{
    ifstream strm;
    strm.open("test_TTProdTbl_CME-D_SIM .dat");
    while (strm.good())
    {
        char buf[255];
        strm.getline(buf, sizeof(buf), ';');
        if (strm.good())
        {
            trie.insert(stringtrie<int>::value_type(buf, 0));
        }
        strm.getline(buf, sizeof(buf), '\n');
    }
}

void query(stringtrie<int>& tree)
{
    stringtrie<int>::node_type *pNode = NULL;
    while (true)
    {
        char inbuf[255];
        cout << "Product: ";
        gets_s(inbuf, sizeof(inbuf));
        if (inbuf[0] == 'q')
            break;
        stringtrie<int>::iterator it = tree.find(inbuf);
        if (it == tree.end())
            cout << "Not found" << endl;
        else
            cout << (*it).first << " = " << (*it).second << endl;
    }
}

class BasicTest
{
public:
    void test()
    {        
        insert("test");
        insert("testing");
        insert("te__ing");
        insert("testid");        
        insert("hello");        

        erase("testid");
        erase("testing");
        stringtrie<int>::iterator it = tree.find("test");
        assert(it != tree.end());
        erase(it);
        erase("te__ing");
        erase("hello");
    }

    void erase(const string& key)
    {
        auto it = find(keys.begin(), keys.end(), key);
        assert(it != keys.end());
        keys.erase(it);
        tree.erase(key);
        assert(tree.size() == keys.size());
        auto it2 = tree.find(key);
        assert(it2 == tree.end());
    }
    void insert(const string& key)
    {
        keys.push_back(key);
        if (keys.size() % 2)
        {
            pair<stringtrie<int>::iterator, bool> p = tree.insert(stringtrie<int>::value_type(key, keys.size()));
            assert(p.second == true);
        }
        else
        {
            tree[key] = keys.size();
        }
       
        assert(tree.size() == keys.size());
        
        for (unsigned int i = 0; i < keys.size(); ++i)
        {
            if ((keys.size() % 2) == 0)
            {
                stringtrie<int>::iterator it = tree.find(key);
                assert(it != tree.end());
            }
            else
            {
                assert(tree[key] == keys.size());
            }
        }
    }

    stringtrie<int> tree;    
    vector<string> keys;
};


enum
{
    TEST_ITERATIONS = 1000000
};

void testMap(vector<string>& data)
{
    map<string, int> m;

    LARGE_INTEGER loadStart;
    LARGE_INTEGER loadStop;
    LARGE_INTEGER runStart;
    LARGE_INTEGER runStop;
    QueryPerformanceCounter(&loadStart);
    loadPTable(m);
    QueryPerformanceCounter(&loadStop);

    _int64 loadTime = loadStop.QuadPart - loadStart.QuadPart;
    srand(1);
    int n = data.size();
    QueryPerformanceCounter(&runStart);
    for (int i = 0; i < TEST_ITERATIONS; ++i)
    {
        int idx = rand() % n;
        auto it = m.find(data[idx]);
        assert(it != m.end());
    }
    QueryPerformanceCounter(&runStop);
    _int64 runTime = runStop.QuadPart - runStart.QuadPart;

    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double load = (double)loadTime/(double)freq.QuadPart;
    double run = (double)runTime/(double)freq.QuadPart;
    cout << "map: LoadTime: " << load << " secs, runTime: " << run << " secs" << endl;
    cout << "avg find: " << (run/(double)TEST_ITERATIONS) * 1000000 << " usec, " << (run/(double)TEST_ITERATIONS) * 1000000000 << " nsec" << endl;
    cout << "avg load: " << (load/(double)TEST_ITERATIONS) * 1000000 << " usec, " << (run/(double)TEST_ITERATIONS) * 1000000000 << " nsec" << endl;

}

void testUnorderedMap(vector<string>& data)
{
    unordered_map<string, int> m;

    LARGE_INTEGER loadStart;
    LARGE_INTEGER loadStop;
    LARGE_INTEGER runStart;
    LARGE_INTEGER runStop;
    QueryPerformanceCounter(&loadStart);
    loadPTable(m);
    QueryPerformanceCounter(&loadStop);

    _int64 loadTime = loadStop.QuadPart - loadStart.QuadPart;
    srand(1);
    int n = data.size();
    QueryPerformanceCounter(&runStart);
    for (int i = 0; i < TEST_ITERATIONS; ++i)
    {
        int idx = rand() % n;
        auto it = m.find(data[idx]);
        assert(it != m.end());
    }
    QueryPerformanceCounter(&runStop);
    _int64 runTime = runStop.QuadPart - runStart.QuadPart;

    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double load = (double)loadTime/(double)freq.QuadPart;
    double run = (double)runTime/(double)freq.QuadPart;
    cout << "unordered_map: LoadTime: " << load << " secs, runTime: " << run << " secs" << endl;
    cout << "avg find: " << (run/(double)TEST_ITERATIONS) * 1000000 << " usec, " << (run/(double)TEST_ITERATIONS) * 1000000000 << " nsec" << endl;
    cout << "avg load: " << (load/(double)TEST_ITERATIONS) * 1000000 << " usec, " << (run/(double)TEST_ITERATIONS) * 1000000000 << " nsec" << endl;
}

void testTrie(vector<string>& data)
{
    stringtrie<int> tree;

    LARGE_INTEGER loadStart;
    LARGE_INTEGER loadStop;
    LARGE_INTEGER runStart;
    LARGE_INTEGER runStop;
    QueryPerformanceCounter(&loadStart);
    loadPTable(tree);
    QueryPerformanceCounter(&loadStop);

    _int64 loadTime = loadStop.QuadPart - loadStart.QuadPart;

    srand(1);
    int n = data.size();
    QueryPerformanceCounter(&runStart);
    for (int i = 0; i < TEST_ITERATIONS; ++i)
    {
        int idx = rand() % n;
        stringtrie<int>::iterator it = tree.find(data[idx]);
        assert((*it).second != false);
    }
    QueryPerformanceCounter(&runStop);

    _int64 runTime = runStop.QuadPart - runStart.QuadPart;
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double load = (double)loadTime/(double)freq.QuadPart;
    double run = (double)runTime/(double)freq.QuadPart;
    cout << "trie: LoadTime: " << load << " secs, runTime: " << run << " secs" << endl;
    cout << "avg find: " << (run/(double)TEST_ITERATIONS) * 1000000 << " usec, " << (run/(double)TEST_ITERATIONS) * 1000000000 << " nsec" << endl;
    cout << "avg load: " << (load/(double)TEST_ITERATIONS) * 1000000 << " usec, " << (run/(double)TEST_ITERATIONS) * 1000000000 << " nsec" << endl;

    cout << "size: " << tree.size() << ", Num nodes: " << tree.getnumnodes() << ", mem: " << tree.getmemusage() << ", mem/node: " << tree.getmemusage()/tree.size() << endl;
}

class DataCompare
{
public:
    bool operator()(const pair<string, int>& p1, const pair<string, int>& p2)
    {
        return p1.first < p2.first;
    }

    bool operator()(const pair<string, int>& p1, const string& s)
    {
        return p1.first < s;
    }

    bool operator()(const string& s, const pair<string, int>& p1)
    {
        return s < p1.first;
    }
};

void testSortedVector(vector<string>& data)
{
    vector<std::pair<std::string, int> > m;
    
    LARGE_INTEGER loadStart;
    LARGE_INTEGER loadStop;
    LARGE_INTEGER runStart;
    LARGE_INTEGER runStop;
    QueryPerformanceCounter(&loadStart);
    for each(const string &s in data)
    {
        m.push_back(pair<string, int>(s, 0));
    }

    sort(m.begin(), m.end(), DataCompare());
    QueryPerformanceCounter(&loadStop);

    _int64 loadTime = loadStop.QuadPart - loadStart.QuadPart;

    srand(1);
    int n = data.size();
    QueryPerformanceCounter(&runStart);
    for (int i = 0; i < TEST_ITERATIONS; ++i)
    {
        int idx = rand() % n;
        vector<std::pair<std::string, int> >::iterator it = lower_bound(m.begin(), m.end(), data[idx], DataCompare());
        assert(it != m.end());
    }
    QueryPerformanceCounter(&runStop);

    _int64 runTime = runStop.QuadPart - runStart.QuadPart;
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double load = (double)loadTime/(double)freq.QuadPart;
    double run = (double)runTime/(double)freq.QuadPart;
    cout << "vector: LoadTime: " << load << " secs, runTime: " << run << " secs" << endl;
    cout << "avg find: " << (run/(double)TEST_ITERATIONS) * 1000000 << " usec, " << (run/(double)TEST_ITERATIONS) * 1000000000 << " nsec" << endl;
    cout << "avg load: " << (load/(double)TEST_ITERATIONS) * 1000000 << " usec, " << (run/(double)TEST_ITERATIONS) * 1000000000 << " nsec" << endl;
}

void performancetest()
{
    vector<string> vec;
    
    loadPTable(vec);

    testMap(vec);
    cout << endl;
    testTrie(vec);
    cout << endl;
    testSortedVector(vec);
    cout << endl;
    testUnorderedMap(vec);
}

void iteratortest()
{
    stringtrie<int> trie;

    loadPTable(trie);

    stringtrie<int>::iterator it = trie.begin();
    stringtrie<int>::iterator eit = trie.end();
    while (it != eit)
    {
        cout << "Key: " << (*it).first << ", value: " << (*it).second << endl;
        ++it;
    }
    cout << "memtest: " << trie.getmemusage() << ", nodes: " << trie.getnumnodes() << ", size: " << trie.size() << endl;
}

class TestData
{
public:
    TestData()
        :nData(0)
    {}
    TestData(const char * p, int n)
        :text(p)
         , nData(n)
    {}
    std::string text;
    int nData;
};

int _tmain(int argc, _TCHAR* argv[])
{
    BasicTest bt;
    bt.test();
    return 0;
}
