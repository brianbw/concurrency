// Brian Wong
// CS 490 - Deliverable (10/6/19)
// Fine-Grained Linked List
/*  This is a fine-grained concurrent linked list that will act as an index
    for the hash table. We improve the concurrency of the hash table by 
    making the linked list concurrent. Now, by locking every node,
    rather than the whole table, we can pipeline instructions that do not
    use the same node. 
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
class fineList 
{
    public:
        fineList();                           // Constructor.
        ~fineList();                          // Destructor.
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
            listNode *link;                     // Link to next node.
            mutex lock;                         // Fine grained lock.          
        };

        listNode *start;                        // Initial node (sentinel).
        listNode *end;                          // Last node (sentinel, essentially NULL).
};


// Initialize all values to NULL or 0.
template <class myType>
fineList<myType>::fineList()
{
    start = new listNode;
    start->item = {};
    
    end = new listNode;
    end->item = {};
    end->link = NULL;

    start->link = end;
}


// Delete the whole linked list.
template <class myType>
fineList<myType>::~fineList()
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
void fineList<myType>::insert(string key, myType item)
{
    start->lock.lock();                     // Lock sentinel node.
    listNode* prev = start;                 // Point to sentinel node.

    prev->link->lock.lock();                // Lock first valid node.
    listNode *curr = prev->link;            // Point to first valid node.

    listNode *inserter = new listNode;      // Create a new node.
    inserter->item = item;                  // Insert <key, item> into node.
    inserter->key = key;

    inserter->link = curr;                  // Connect it to the first valid node.
    prev->link = inserter;                  // Connect sentinel node to new node.

    prev->lock.unlock();                    // Unlock sentinel node.
    curr->lock.unlock();                    // Unlock first valid node.
}


// When given a <key, item> pair, search for the node
// with that <key, item> pair from the start, and
// remove if if it exists. Else, we return.
template <class myType>
void fineList<myType>::remove(string key, myType item)
{
    start->lock.lock();                                     // Lock sentinel node.
    listNode *prev = start;                                 // Point to sentinel node.

    prev->link->lock.lock();                                // Lock first valid node.
    listNode *remover = start->link;                        // Point to first valid node.

    while (remover != end)                                  // Loop through whole list.
    {
        if (remover->item == item && remover->key == key)   // If <key, item> matches,
        {
            remover->link->lock.lock();                     // Lock next node. prev->link = remover = still locked
            prev->link = remover->link;                     // Disconnect node & deallocate.
            remover->link->lock.unlock();                   // Unlock next node.

            prev->lock.unlock();
            remover->lock.unlock();
            return;
        }

        prev->lock.unlock();                                // Unlock previous node
        prev = remover;                                     // Traverse to next node.

        remover->link->lock.lock();                         // Lock next node.
        remover = remover->link;                            // Traverse to next node.
    }

    prev->lock.unlock(); 
    remover->lock.unlock();
}


// Print out the item of each node from the first valid node.
template <class myType>
void fineList<myType>::printList()
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
unsigned int fineList<myType>::getCount()
{
    unsigned int sum = 0;
    listNode *adder = start->link;    // Point to first valid node.

    while (adder != end)              // Loop through whole list.
    {
        sum ++;
        adder = adder->link;
    }

    return sum;
}


// When given a <key, item> pair, search for the node
// with that <key, item> pair from the start, and 
// return the item of that node if it exists. Else, return {}.
template <class myType>
myType fineList<myType>::search(string key, myType item)
{
    start->lock.lock();                                         // Lock sentinel node.
    listNode *prev = start;                                     // Point to sentinel node.

    prev->link->lock.lock();                                    // Lock first valid node.
    listNode *searcher = start->link;                            // Point to first valid node.

    while (searcher != end)                                     // Loop through whole list.
    {   
        if (searcher->item == item && searcher->key == key)     // If <key, item> matches,
        {   
            prev->lock.unlock();                                // Unlock previous node when found.
            searcher->lock.unlock();                            // Unlock current node when found.
            return searcher->item;                              // Return item of that node.
        }

        prev->lock.unlock();                                    // Unlock previous node.
        prev = searcher;                                        // Previous is now current.
        searcher->link->lock.lock();                            // Lock new current.
        searcher = searcher->link;                              // Points to new current.
    }   

    prev->lock.unlock();                                        // Unlock previous node when not found.
    searcher->lock.unlock();                                    // Unlock current node when not found.
    return {};
}
