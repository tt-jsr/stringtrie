#ifndef _STRING_TRIE_H_
#define _STRING_TRIE_H_

#include <utility>
#include <assert.h>
#include <iostream>

/******************************************************************************************
 * stringtrie
 *
 * This associative container is a modification of a radix trie (or patricia trie, 
 * see wikipedia). It has been optimized for speed at the expense of memory. It is also
 * specialized for std::string keys.
 *
 * Each node in the tree contains a table of 128 pointers. Each table entry is
 * a pointer to a child node, so each node can be a parent to 128 child nodes. (This number, 128,
 * is defined as RANGE)
 *
 * Conceptually, the key of a node is the concatenation of all the keys from the root to the
 * node itself, so each node only contains a portion of the complete key.
 *
 * The concatenated key of a node is the prefix of all its child node keys. It would be a simple
 * process to provide an interface to perform a search based on key prefixes, but this has not 
 * been implemented.
 *
 * In this implemenation, the table size is 128, the reason for this is to support a direct
 * table lookup of a child node given the next character of the key. 128 was selected to support
 * all the printable characters of a 7bit ASCII character set, so this implementation does not
 * support unicode or wide character keys.
 *
 * Performance:
 *
 * The performance of a radix trie is O(k), when compared to the stl::map which is O(log n)
 * it would apprear that the radix tree would be slower, however the map requires a key compare
 * for every node the lookup visits, this implementation requires at most, one key compare.
 *
 *
 * Each node requires approx 128*sizeof(pointer) + sizeof(T) of memory
 *
 * STL conformance
 *
 * stringtrie has the same interface as the stl::map<> but is not completely implemented, the
 * following still need to be completed:
 *   const_iterators
 *   upper_bound(), lower_bound
 *   equal_range
 *   insert with hint
 *   rbegin(), rend(), reverse_iterators (maybe i'll do this)
 *   
 *   Because a radix trie, which this is based on, supports lookups using a key prefix, an 
 *   interface could be defined to support these kind of lookups.
 *
 * Testing:
 *
 * The performance was tested by loading 1000 products, 300 user names and 300 MGT and comparing to
 * stl::map<>, sorted stl::vector<>, stl::unorderedmap<> and the stringtrie and performing
 * one million random lookups
 *
 * The amount of memory consumed by the trie in this test is approx 1.05 MB
 * Based on this test, the trie is almost 4 times faster than the fastest stl hash container.
 *
 * ----map-----
 * LoadTime: 0.00202613 secs, runTime: 0.296505 secs
 * avg find: 0.296505 usec, 296.505 nsec
 * avg load: 0.00202613 usec, 296.505 nsec
 * 
 * ----vector----
 * LoadTime: 0.000870421 secs, runTime: 0.299101 secs
 * avg find: 0.299101 usec, 299.101 nsec
 * avg load: 0.000870421 usec, 299.101 nsec
 * 
 * ----unordered_map----
 * LoadTime: 0.00184307 secs, runTime: 0.0907445 secs
 * avg find: 0.0907445 usec, 90.7445 nsec
 * avg load: 0.00184307 usec, 90.7445 nsec
 *
 * ----trie----
 * LoadTime: 0.00225069 secs, runTime: 0.023903 secs
 * avg find: 0.023903 usec, 23.903 nsec
 * avg load: 0.00225069 usec, 23.903 nsec
 * Num nodes: 1901, mem: 1056956
 *
 * ****************************************************************************************/

namespace tt_coreutils_ns
{
    template < typename T>
    class stringtrie;

    template <typename T> 
    class stringtrie_node
    {
    public:
        typedef T value_type;
        typedef std::string key_type;
        typedef stringtrie_node<T> node_type;
        enum {
            RANGE = 128
        };    

        stringtrie_node();
        ~stringtrie_node();

        const std::string& getkey() const;

        bool hasValue() const { return bInUse; }

        const value_type& getvalue() const {return value;}
        value_type& getvalue() {return value;}
        
    private:
        node_type *parent;
        node_type *table[RANGE];
        value_type value;
        bool bInUse;
        std::string nodeKey;             // The full key of this node, a concatenation of all nodes from the root to here
        unsigned int posNodeKeyStart;    // The key of this node starts at this position. This is an index into nodeKey
        friend class stringtrie<T>;
    private:
        void addchild(node_type *);
        void setvalue(const value_type& v);
        node_type *_find( const std::string& key, unsigned int pos );
        node_type* _findpartial( const std::string& key, unsigned int pos );
        int gettableindex() const;
    };

    template <typename T> 
    class stringtrie
    {
    public:
        typedef std::pair<const std::string, T> value_type;
        typedef std::string key_type;
        typedef T mapped_type;
        typedef T& reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef stringtrie_node<T> node_type;

        enum {
            RANGE = 128
			, RANGE_MASK = 0x7f
        };

        class iterator
        {
        public:
            iterator()
                :pTrie(NULL)
                 , pNode(NULL)
            {
            }

            iterator(stringtrie<T> *pt, stringtrie_node<T> *pn)
                :pTrie(pt)
                 , pNode(pn)
            {

            }

            iterator(const iterator& it)
            {
                pTrie = it.pTrie;
                pNode = it.pNode;
            }

            iterator operator=(const iterator& rhs)
            {
                if (this == &rhs)
                    return *this;
                pTrie = rhs.pTrie;
                pNode = rhs.pNode;
                return *this;
            }
            
            std::pair<const std::string, reference> operator*()
            {
                return std::pair<const std::string, reference>(pNode->getkey(), pNode->getvalue());
            }

            std::pair<const std::string, reference> operator->()
            {
                return std::pair<const std::string, reference>(pNode->getkey(), pNode->getvalue());
            }

            iterator operator++()
            {
                if (pNode == NULL || pTrie == NULL)
                    return *this;
                pNode = pTrie->next(pNode);
                while (pNode && pNode->hasValue() == false)
                    pNode = pTrie->next(pNode);
                return *this;
            }

            iterator operator++(int)
            {            
                if (pNode == NULL || pTrie == NULL)
                    return *this;
                iterator r = *this;
                pNode = pTrie->next(pNode);
                while (pNode && pNode->hasValue() == false)
                    pNode = pTrie->next(pNode);
                return r;
            }

            bool operator==(const iterator& rhs) const
            {
                if (pNode == rhs.pNode)
                    return true;
                return false;
            }
            
            bool operator!=(const iterator& rhs) const
            {
                if (pNode != rhs.pNode)
                    return true;
                return false;
            }

        private:
            friend class stringtrie<T>;
            stringtrie<T> *pTrie;
            stringtrie_node<T> *pNode;
        };

        stringtrie();
        ~stringtrie();

        // Not implemented
        stringtrie(const stringtrie<T>& cc);

        // Not implemented
        stringtrie<T>& operator=(const stringtrie<T>& rhs);

        std::pair<iterator, bool> insert(const value_type&);
        void erase ( iterator position );
        size_type erase ( const key_type& k );
        size_type count ( const key_type& k ) const
        {
            iterator it = find(k);
            if (it != end())
                return 1;
            return 0;
        }

        bool empty() const
        {
            return nsize != 0;
        }

        void clear()
        {
            for (int i = 0; i < RANGE; ++i)
            {
                if (root->table[i])
                {
                    delete root->table[i];
                    root->table[i] = NULL;
                }
            }
        }

        iterator find(const std::string& key);

        inline T& operator[](const key_type& k)
        {
            iterator i = find( k );
           
            if( i==end() )
            {
                std::pair<stringtrie<T>::iterator, bool> p = insert( value_type(k, T()) );
                stringtrie<T>::iterator it = find(k);
                return (*it).second;
            }
            return (*i).second;
        }

        size_t size() const
        {
            return nsize;
        }

        iterator begin()
        {
            node_type *pNode = next(root);
            while (pNode->hasValue() == false)
                pNode = next(pNode);
            return iterator(this,pNode);
        }

        iterator end()
        {
            return iterator();
        }

        int getmemusage() const;
        int getnumnodes() const;

    private:
        node_type *root;
        int numnodes;
        size_t nsize;
    private:
        unsigned int substrlength(const std::string& s1, const std::string& s2);
        node_type *next(node_type *current)
        {
            node_type *pn = current;
            int tblidx = 0;
            while (pn)
            {
                // depth first
                for (; tblidx < RANGE; ++tblidx)
                {
                    if (pn->table[tblidx])
                    {
                        return pn->table[tblidx];
                    }
                }
                if (NULL == pn->parent)
                {
                    return NULL;
                }
                // Get the index this node is in the parent
                tblidx = pn->gettableindex();
                ++tblidx;
                pn = pn->parent;
            }
            return NULL;
        }
    };

    //=================================================================
    // stringtrie
    //=================================================================
    template<typename T>
    inline stringtrie<T>::stringtrie()
        : root(NULL)
        , numnodes(0)
        , nsize(0)
    {
        root = new node_type();
    }

    template<typename T>
    stringtrie<T>::~stringtrie()
    {
        delete root;
    }

    template<typename T>
    inline int stringtrie<T>::getmemusage( ) const
    {
        return this->numnodes * sizeof(node_type);
    }

    template<typename T>
    inline int stringtrie<T>::getnumnodes( ) const
    {
        return this->numnodes;
    }

    template<typename T>
    inline typename stringtrie<T>::iterator stringtrie<T>::find( const std::string& key )
    {
        int pos = 0;
        node_type *pNode = root->_find(key, pos);
        if (pNode == NULL || pNode->hasValue() == false)
            return end();
        return iterator(this, pNode);
    }

    template<typename T>
    inline std::pair<typename stringtrie<T>::iterator, bool> stringtrie<T>::insert(const value_type& v)
    {
        ++nsize;
        const string& key = v.first;
        const T& value = v.second;

        // Find the deepest node that at least partially
        // matches the key
        node_type *pNode = root->_findpartial(key, 0);

        if (pNode->getkey() == key)
        {
            if (pNode->bInUse)
            {
                --nsize;
                return std::pair<iterator, bool>(iterator(), false);   // key exists;
            }
            pNode->setvalue(value);
            return std::pair<iterator, bool>(iterator(this, pNode), true);
        }

        unsigned int pos = substrlength(pNode->getkey(), key);
        if (pos == pNode->getkey().length())
        {
            //The new key is a superset of this node's key, this will be easy...
            node_type *pNewChildNode = new node_type();
            ++numnodes;
            pNewChildNode->nodeKey = key;
            pNewChildNode->posNodeKeyStart = pos;
            pNewChildNode->parent = pNode;
            pNewChildNode->setvalue(value);
            pNode->addchild(pNewChildNode);
            return std::pair<iterator, bool>(iterator(this, pNode), true);
        }
        // We need to split this node
        // Insert a new node 
        node_type *orig_parent = pNode->parent;
        node_type *pNewParentNode = new node_type();
        ++numnodes;
        pNewParentNode->parent = pNode->parent;        
        pNewParentNode->nodeKey = pNode->getkey().substr(0, pos);
        pNewParentNode->posNodeKeyStart = pNode->posNodeKeyStart;
        orig_parent->addchild(pNewParentNode);       

        pNode->parent = pNewParentNode;
        pNode->posNodeKeyStart = pos;

        pNewParentNode->addchild(pNode);

        // Now add the new node
        pNode = new node_type();
        ++numnodes;
        pNode->parent = pNewParentNode;
        pNode->setvalue(value);
        pNode->nodeKey = key;
        pNode->posNodeKeyStart = pos;
        pNewParentNode->addchild(pNode);
        return std::pair<iterator, bool>(iterator(this, pNode), true);
    }

    template<typename T> 
    void stringtrie<T>::erase(typename stringtrie<T>::iterator it)
    {
        erase((*it).first);
    }

    template<typename T> 
    size_t stringtrie<T>::erase(const key_type& k)
    {
        iterator it = find(k);
        if (it == end())
            return 0;
        node_type *pNode = it.pNode;
        pNode->bInUse = false;
        --nsize;

        // If this node has no children, delete it
        while (pNode)
        {
            int tblidx = 0;
            for (tblidx = 0; tblidx < RANGE; ++tblidx)
            {
                if (pNode->table[tblidx])
                {
                    break;
                }
            }
            if (RANGE == tblidx && pNode->bInUse == false)
            {
                if (pNode->parent)
                {
                    int idx = pNode->gettableindex();
                    pNode->parent->table[idx] = NULL;
                }
                node_type *tmp = pNode;
                pNode = pNode->parent;  // Do the loop again with the parent
                delete tmp;
                --numnodes;
            }
            else
            {
                pNode = NULL;   // break out of loop
            }
        }
        // Space optimization todo:
        // If this node has only one child, we might be able to combine nodes
        return 1;
    }

    // Returns the number of characters of s1 contained in s2
    template<typename T> 
    unsigned int stringtrie<T>::substrlength(const std::string& s1, const std::string& s2)
    {
        unsigned int p1 = 0;
        unsigned int p2 = 0;
        for (; p1 < s1.length() && p2 < s2.length(); ++p1, ++p2)
        {
            if (s1[p1] != s2[p2])
                break;
        }
        return p1;
    }

    //=================================================================
    // stringtrie
    //=================================================================

    template<typename T>
    stringtrie_node<T>::stringtrie_node()
        :parent(0)
        , value(T())
        , bInUse(false)
        , posNodeKeyStart(0) 
    {
        memset(table, 0, sizeof(table));
    }

    template<typename T>
    stringtrie_node<T>::~stringtrie_node()
    {
        for (int i = 0; i < RANGE; ++i)
        {
            delete table[i];
        }
    }

    template<typename T>
    inline const std::string& stringtrie_node<T>::getkey() const
    {
        return nodeKey;
    }

    template<typename T>
    inline int stringtrie_node<T>::gettableindex() const
    {
        return nodeKey[posNodeKeyStart] & RANGE_MASK;
    }

    template<typename T>
    inline void stringtrie_node<T>::setvalue(const value_type& v)
    {
        value = v;
        bInUse = true;
    }

    template<typename T>
    inline void stringtrie_node<T>::addchild(typename stringtrie_node<T>::node_type *pNode)
    {
        table[pNode->gettableindex()] = pNode;
    }

    // Internal helper function. Given a key, this will return the deepest node that contains
    // at least a partial match. insert() uses this to find the node where a new key should be added
    template<typename T>
    inline typename stringtrie_node<T>::node_type *stringtrie_node<T>::_findpartial( const std::string& key, unsigned int pos )
    {
        unsigned int posPartialKey = posNodeKeyStart;

        for (; posPartialKey < nodeKey.size() && pos < key.length(); ++posPartialKey, ++pos)
        {
            if (nodeKey[posPartialKey] != key[pos])
                break;
        }

        // We matched the entire search key and the entire node key so we match
        if (pos == key.length() && posPartialKey == nodeKey.length())
        {
            return this;
        }
        
        // We didn't match the entire node key so we return this
        if (posPartialKey < nodeKey.size())
        {
            return this;
        }

        // We still have some 'key' left over so dive into a child
        node_type *t = this->table[key[pos]];
        if (NULL == t)
        {
            // No child nodes, return this
            return this;
        }
        return t->_findpartial(key, pos);
    }

    // TODO: Unroll this to remove recursion
    template<typename T>
    inline typename stringtrie_node<T>::node_type *stringtrie_node<T>::_find( const std::string& key, unsigned int pos )
    {
        node_type * t = this;
        unsigned int posPartialKey = posNodeKeyStart;

        for (; posPartialKey < nodeKey.size() && pos < key.length(); ++posPartialKey, ++pos)
        {
            if (nodeKey[posPartialKey] != key[pos])
                break;
        }

        // We matched the entire search key and the entire node key so we match
        if (pos == key.length() && posPartialKey == nodeKey.length())
        {
            return this;
        }
        
        // We didn't match the entire node key so we fail
        if (posPartialKey < nodeKey.size())
        {
            return NULL;
        }

        // We still have some 'key' left over so dive into a child
        t = t->table[key[pos]];
        if (NULL == t)
        {
            // No child nodes, we fail
            return NULL;
        }
        return t->_find(key, pos);
    }
}   // namespace tt_coreutils_ns
#endif _STRING_TRIE_H_

