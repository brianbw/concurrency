// Brian Wong
// CS 490 - Deliverable (9/15/19)
// Linked List
/*  This is a sequential linked list that acts as an index for the hash table.
    Each index of the hash table contains a linked list, which expands when a
    collision occur. Each linked list node will contain a key, item pair, and 
    a link to the next node.
*/

/*  To prove the correctness of this linked list
    the following invariants were held:
	- key/item cannot be the "empty" state of any data type
		* ex. integer cannot have a key/item of 0
    - duplicate <key, item> pairs are allowed.
*/

#include <iostream>
#include <string>
#include <mutex>
#include <cstdlib>

using namespace std;

template <class myType>
class coarseList 
{
    public:
        coarseList();                           // Constructor.
        ~coarseList();                          // Destructor.
        void insert(string, myType);            // Insert node w/ <key, item> to the front of the list.
        void remove(string, myType);            // Remove node w/ <key, item> if it exists.
        void printList();                       // Prints out the contents of the linked list.

        unsigned int getCount();                // Get total node count in list.
        myType search(string, myType);          // Search node w/ <key, item>.
                                                // If node is in list, return item, else return {}.
        
    private:
        struct listNode                         // listNode contains <key, item> & link to next node.
        {   
            string key;                         // <Key, Item> pair.
            myType item;                       
            listNode *link;                     
        };

        listNode *start;                        // Initial node (sentinel).
        listNode *end;                          // Last node (sentinel, essentially NULL).

        unsigned int count;                     // Total nodes in linked list (# of <key, item>).
};


// Initialize all values to NULL or 0.
template <class myType>
coarseList<myType>::coarseList()
{
    start = new listNode;
    start->item = {};
    end = NULL;
    start->link = end;
    count = 0;
}


// Delete the whole linked list.
template <class myType>
coarseList<myType>::~coarseList()
{
    listNode *deleter = start;          // Point to first node.
    while (deleter != end)              // Loop through whole list.
    {   
        deleter = deleter->link;        // Traverse to next node.
        delete deleter;                 // Deallocate node.
    }

    delete start;                       // Deallocate initial node (sentinel).
    start = NULL;                       
}


// When given a <key, item> pair, create a new node
// with the <key, item> and insert it to the front of
// the linked list.
template <class myType>
void coarseList<myType>::insert(string key, myType item)
{
    listNode *inserter = new listNode;      // Create a new node.
    inserter->item = item;                  // Insert <key, item> into node.
    inserter->key = key;                    
    inserter->link = start->link;           // Connect it to the first node.
    start->link = inserter;                 // Reset start pointer.
    count ++;                               // Update count.
}


// When given a <key, item> pair, search for the node
// with that <key, item> pair from the start, and
// remove if if it exists. Else, we return.
template <class myType>
void coarseList<myType>::remove(string key, myType item)
{
    listNode *remover = start->link;                        // Point to first valid node.
    listNode *prev = start;                                 // Point to sentinel node.
    while (remover != end)                                  // Loop through whole list.
    {
        if (remover->item == item && remover->key == key)   // If <key, item> matches,
        {
            prev->link = remover->link;                     // Disconnect node & deallocate.
            delete remover;
            count --;                                       // Update count.
            return;
        }

        prev = remover;                                     // Traverse to next node.
        remover = remover->link;
    }
}


// Print out the item of each node from the first valid node.
template <class myType>
void coarseList<myType>::printList()
{
    listNode *printer = start->link;    // Point to first valid node.
    while (printer != end)              // Loop through whole list.
    {
        cout << printer->item << " ";   // Print each item.
        printer = printer->link;        // Traverse to next node.
    }

    cout << endl;
}


// Returns the current number of nodes in the list.
template <class myType>
unsigned int coarseList<myType>::getCount()
{
    return count;
}

// When given a <key, item> pair, search for the node
// with that <key, item> pair from the start, and 
// return the item of that node if it exists. Else, return {}.
template <class myType>
myType coarseList<myType>::search(string key, myType item)
{
    listNode *searcher = start->link;                           // Point to first valid node.
    while (searcher != end)                                     // Loop through whole list.
    {
        if (searcher->item == item && searcher->key == key)     // If <key, item> matches,
            return searcher->item;                              // Return item of that node.

        searcher = searcher->link;                              // Traverse to next node.
    }   
    
    return {};
}