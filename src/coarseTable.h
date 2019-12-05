// Brian Wong
// CS 490 - Deliverable (9/15/19)
// Coarse-Grained Concurrent Hash Table
/* 	Table has a single lock where every method must acquire.
	This hash table, is thread safe, but because every method
	has to hold the lock while executing, it is essentially
	sequential. 
*/

/* 	To prove the correctness of this hash table,
	the following invariants were held:
	- <key, item> cannot be the "empty" state of any data type.
		* ex. integer cannot have a key/item of 0.
	- duplicate <key, item> pairs are allowed.
*/

#include "coarseList.h"
#include <iostream>
#include <mutex>
#include <cstdlib>
#include <string>

using namespace std;

template <class myType>
class coarseTable 							
{
	public:
		coarseTable();							// Default constructor, size of 100,000 (indices).
		coarseTable(unsigned int);				// Secondary constructor, specified size (indices).
		~coarseTable();							// Destructor.
		
		void insert(string, myType);			// Insert w/ provided <key, item> pair.
		void remove(string, myType);			// Remove w/ provided <key, item> pair if it exists.
		void printTable();						// Prints out the contents of the hash table.

		unsigned int getCount();				// Get total entry count in table (# of items).
		myType search(string, myType);			// Search w/ provided <key, item> pair.
												// If pair is in table, return key, else return {}.
	private:	
		struct hashNode							// hashNode represents an index of the hash table.
		{
			unsigned int count;					// Entry count for one index.
			coarseList<myType> entries;			// Linked list for separate chaining.
		};

		unsigned int entryCount;				// Total entry count in table (# of items).
		unsigned int size;						// Size of hash table (indices).
		unsigned int hash(string);				// FNV-1a hash algorithm.
		hashNode *table;						// The hash table itself.
		mutex lock;								// Coarse grain lock.
};


// Initialize all values, set default size to 100,000.
template <class myType>
coarseTable<myType>::coarseTable() 
{
	entryCount = 0;
	size = 10000000;								
	table = new hashNode[size];
}


// Initialize all values, set size to specified size.
template <class myType>
coarseTable<myType>::coarseTable(unsigned int tableSize) 
{
	entryCount = 0;
	size = tableSize;							
	table = new hashNode[size];
}


// Deallocate dynamically allocated variables.
template <class myType>
coarseTable<myType>::~coarseTable() 
{
	delete []table;
}


// When given a <key, item> pair, hash the key to get an index, 
// and store the item in that hashed index.
// If the index is empty, create new node in the linked list.
// Else, append it to the linked list.
template <class myType>
void coarseTable<myType>::insert(string key, myType item) 
{
	lock_guard<mutex>guard(lock);				// Acquire lock.
	
	unsigned hashKey = hash(key);				// Hash provided key to get index.			
	table[hashKey].entries.insert(key, item);	// Insert <key, item> into that index.
	table[hashKey].count++;						// Update count variables.
	entryCount++;
}


// When given a <key, item> pair, hash the key to get an index, 
// and search for the item in that hashed index.
// If the <key, item> pair is not found, print out error message and return.
// Else, remove the node that holds the item, and update count variables.
template <class myType>
void coarseTable<myType>::remove(string key, myType item) 
{
	lock_guard<mutex>guard(lock);							// Acquire lock.

	myType empty = {};							
	unsigned hashKey = hash(key);							// Hash provided key to get index.			
	myType temp = table[hashKey].entries.search(key, item);	// Search for <key, item> in that index.

	if (temp == empty)											
	{
		cout << "Remove error, key: " << key << " and item: " << item << " is not in table. Skipping." << endl;
		return; 
	}

	table[hashKey].entries.remove(key, item);				// If we found it, remove it from index.
	table[hashKey].count--;									// Update count variables.
	entryCount --;	
}


// Print out the whole hash table starting from the first index.
template <class myType>
void coarseTable<myType>::printTable()
{
	lock_guard<mutex>guard(lock);				// Acquire lock.

	for (unsigned int i = 0; i < size; i ++)	// Loop through whole table.
	{
		cout << "[" << i << "]: ";
		table[i].entries.printList();			// Print linked list in every index.
	}
}

// When given a <key, item> pair, hash the key to get an index, 
// and search for the item in that hashed index.
// If the <key, item> pair is not found, print out error message and return {}.
// Else, return the item.
template <class myType>
myType coarseTable<myType>::search(string key, myType item)
{	
	lock_guard<mutex>guard(lock);								// Acquire lock.

	myType empty = {};
	unsigned int hashKey = hash(key);							// Hash provided key to get index.		
	
	myType temp = table[hashKey].entries.search(key, item);		// Search for <key, item> in that index.
	if (temp == empty)
	{
		cout << "Search error, key: " << key << " and item: " << item << " is not in table. Skipping." << endl;
		return {};
	}
	return temp;												// If we found it, return item.
}

// Returns the current total entry count in the hash table.
template <class myType> 
unsigned int coarseTable<myType>::getCount()
{
	lock_guard<mutex>guard(lock);	// Acquire lock.
	return entryCount;				
}

// FNV-1a Hashing Algorithm. 
template <class myType>
unsigned int coarseTable<myType>::hash(string key)
{
    unsigned int FNV_offset_basis = 2166136261;
    unsigned int FNV_prime = 16777619;

    unsigned int hash = FNV_offset_basis;

    for (unsigned int i = 0; i < key.length(); i++) {
        hash ^= key[i];
        hash *= FNV_prime;
    }

    return (hash % size);
}
