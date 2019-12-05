#include "coarseTable.h"
#include "fineTable.h"
#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

enum opCode { INSERT, SEARCH, REMOVE };

unsigned int START = 10000000;
unsigned int RANGE = 10000000;
unsigned int THREAD_MULT = 1;
unsigned int INIT_THREAD_COUNT = 3;
unsigned int MAIN_THREAD_COUNT = 3;
unsigned int END_THREAD_COUNT = 3;

coarseTable<unsigned int> cTable(100000);
fineTable<unsigned int> fTable(100000);

// ****************************************************************************
void coarseTesting(unsigned int from, unsigned int to, opCode op) {
   if (op == INSERT) {
        cout << "THREAD INSERTING, FROM: " << from << " TO: " << to << endl;
        while (from <= to) {
            cTable.insert (to_string(from), from);
            from += 1;
        }
    }
    else if (op == SEARCH) {
        cout << "THREAD SEARCHING, START: " << from << " TO: " << to << endl;
        while (from <= to) {
            if (cTable.search (to_string(from), from) == 0) {
                cout << "search(" << from << ") error." << endl;
            }
            from += 1;
        }
    }
    else if (op == REMOVE) {
        cout << "THREAD REMOVING, START: " << from << " TO: " << to << endl;
        while (from <= to) {
            cTable.remove (to_string(from), from);
            from += 1;
        }
    }
}

// ****************************************************************************
void fineTesting(unsigned int from, unsigned int to, opCode op) {
   if (op == INSERT) {
        cout << "THREAD INSERTING, FROM: " << from << " TO: " << to << endl;
        while (from <= to) {
            fTable.insert (to_string(from), from);
            from += 1;
        }
    }
    else if (op == SEARCH) {
        cout << "THREAD SEARCHING, START: " << from << " TO: " << to << endl;
        while (from <= to) {
            if (fTable.search (to_string(from), from) == 0) {
                cout << "search(" << from << ") error." << endl;
            }
            from += 1;
        }
    }
    else if (op == REMOVE) {
        cout << "THREAD REMOVING, START: " << from << " TO: " << to << endl;
        while (from <= to) {
            fTable.remove (to_string(from), from);
            from += 1;
        }
    }
}

// ****************************************************************************
int main (int argc, char *argv[])
{
// *****************************************************************
//  Data declarations...
    string		stars = "";
    string      bars = "";
    char        userOpt;
    char        userOpt2;
    bool        keepProcessing = true;

    stars.append (65, '*');
    bars.append (60, '-');

// *****************************************************************
//  Get/verify command line arguments.
//	Error out if bad arguments...

    if (argc != 1) {
        cout << "Usage: ./fullTest" << endl;
        exit(1);
    }

    auto s1 = chrono::high_resolution_clock::now();
    auto s2 = chrono::high_resolution_clock::now();

    unsigned long hwthd = thread::hardware_concurrency();

    thread *t0 = NULL;
    thread *t1 = NULL;
    thread *t2 = NULL;

// *****************************************************************
// Display header

	cout << stars << endl << "Concurrent Hash Table Tests" << endl << endl;

// *****************************************************************
// Main...

    while(keepProcessing) {
        cout << bars << endl;
        cout << "Select Option:" << endl;
        cout << "\t'c' - Coarse Grained Synchronization" << endl;
        cout << "\t'f' - Fine Grained Synchronization" << endl;
        cout << "\t'd' - Display machine stats" << endl;
        cout << "\t'n' - Change start and range" << endl;
        cout << "\t'q' - Quit" << endl;
        cout << "> ";
        cin >> userOpt;

        switch(userOpt) {
// *****************************************************************
// Coarse Grained Hash Table
            case 'c':
                cout << "\t '1' - 1*9 = 9 thread" << endl;
                cout << "\t '2' - 2*9 = 18 thread" << endl;
                cout << "\t '4' - 4*9 = 36 thread" << endl;
                cout << "> ";
                cin >> userOpt2;
                THREAD_MULT = userOpt2 - '0';
                
                cout << "\t " << INIT_THREAD_COUNT * THREAD_MULT << " Initializes, " << THREAD_MULT << " Removes, " 
                    << THREAD_MULT << " Searches, " << THREAD_MULT << " Inserts, " << END_THREAD_COUNT* THREAD_MULT << " Deletes." << endl;

                cout << bars << endl << "Coarse Grained Hash Table" << endl << endl;;

                // Allocate threads to run
                t0 = new thread[THREAD_MULT * INIT_THREAD_COUNT];
                t1 = new thread[THREAD_MULT * MAIN_THREAD_COUNT];
                t2 = new thread[THREAD_MULT * END_THREAD_COUNT];


                // -----------------------------------------------------------------
                // INIT SECTION

                for (unsigned int i = 0; i < THREAD_MULT * INIT_THREAD_COUNT; i ++) 
                    t0[i] = thread (&coarseTesting, (START + i*RANGE) / THREAD_MULT, (START + (i+1)*RANGE - 1) / THREAD_MULT, INSERT);
                    
                for (unsigned int i = 0; i < THREAD_MULT * INIT_THREAD_COUNT; i ++)
                    t0[i].join();

                cout << endl << "Length after init section: " << cTable.getCount() << endl << endl;


                // -----------------------------------------------------------------
                // MAIN SECTION

                // Start time
                s1 = chrono::high_resolution_clock::now();

                // Start up threads...
                for (unsigned int i = 0; i < THREAD_MULT * MAIN_THREAD_COUNT; i ++) {
                    if (i % 3 == 0)
                        t1[i] = thread(&coarseTesting, (START + i*RANGE) / THREAD_MULT, (START + (i+1)*RANGE - 1) / THREAD_MULT, REMOVE);

                    else if (i % 3 == 1)
                        t1[i] = thread(&coarseTesting, (START + i*RANGE) / THREAD_MULT, (START + (i+1)*RANGE - 1) / THREAD_MULT, SEARCH);

                    else if (i % 3 == 2)
                        t1[i] = thread(&coarseTesting, (START + (i+1)*RANGE) / THREAD_MULT, (START + (i+2)*RANGE - 1) / THREAD_MULT, INSERT);
                }

                for(unsigned int i = 0; i < THREAD_MULT * MAIN_THREAD_COUNT; i ++)
                    t1[i].join();

                cout << endl << "Length after main section: " << cTable.getCount() << endl << endl;


                // -----------------------------------------------------------------
                // END SECTION

                // Wait for threads to finish...
                for (unsigned int i = 0; i < THREAD_MULT * END_THREAD_COUNT; i ++) 
                    t2[i] = thread(&coarseTesting, (START + (i+1)*RANGE) / THREAD_MULT, (START + (i+2)*RANGE - 1) / THREAD_MULT, REMOVE);

                 for(unsigned int i = 0; i < THREAD_MULT * END_THREAD_COUNT; i ++)
                    t2[i].join();

                 cout << endl << "Length after end section: " << cTable.getCount() << endl << endl;

                // End time
                s2 = chrono::high_resolution_clock::now();

                cout << endl << "Run-time: " << chrono::duration_cast<chrono::milliseconds>(s2 - s1).count()
                     << " milliseconds" << endl << endl;

                delete [] t0;
                delete [] t1;
                delete [] t2;

                break;

// *****************************************************************
// Fine Grained Hash Table
            case 'f':
                cout << "\t '1' - 1*9 = 9 thread" << endl;
                cout << "\t '2' - 2*9 = 18 thread" << endl;
                cout << "\t '4' - 4*9 = 36 thread" << endl;
                cout << "> ";
                cin >> userOpt2;
                THREAD_MULT = userOpt2 - '0';
                
                cout << "\t " << INIT_THREAD_COUNT * THREAD_MULT << " Initializes, " << THREAD_MULT << " Removes, " 
                    << THREAD_MULT << " Searches, " << THREAD_MULT << " Inserts, " << END_THREAD_COUNT * THREAD_MULT << " Deletes." << endl;

                cout << bars << endl << "Fine Grained Hash Table" << endl << endl;;

                // Allocate threads to run
                t0 = new thread[THREAD_MULT * INIT_THREAD_COUNT];
                t1 = new thread[THREAD_MULT * MAIN_THREAD_COUNT];
                t2 = new thread[THREAD_MULT * END_THREAD_COUNT];


                // -----------------------------------------------------------------
                // INIT SECTION

                for (unsigned int i = 0; i < THREAD_MULT * INIT_THREAD_COUNT; i ++) 
                    t0[i] = thread (&fineTesting, (START + i*RANGE) / THREAD_MULT, (START + (i+1)*RANGE - 1) / THREAD_MULT, INSERT);
                    
                for (unsigned int i = 0; i < THREAD_MULT * INIT_THREAD_COUNT; i ++)
                    t0[i].join();

                cout << endl << "Length after init section: " << fTable.getCount() << endl << endl;


                // -----------------------------------------------------------------
                // MAIN SECTION

                // Start time
                s1 = chrono::high_resolution_clock::now();

                // Start up threads...
                for (unsigned int i = 0; i < THREAD_MULT * MAIN_THREAD_COUNT; i ++) {
                    if (i % 3 == 0)
                        t1[i] = thread(&fineTesting, (START + i*RANGE) / THREAD_MULT, (START + (i+1)*RANGE - 1) / THREAD_MULT, REMOVE);

                    else if (i % 3 == 1)
                        t1[i] = thread(&fineTesting, (START + i*RANGE) / THREAD_MULT, (START + (i+1)*RANGE - 1) / THREAD_MULT, SEARCH);

                    else if (i % 3 == 2)
                        t1[i] = thread(&fineTesting, (START + (i+1)*RANGE) / THREAD_MULT, (START + (i+2)*RANGE - 1) / THREAD_MULT, INSERT);
                }

                for(unsigned int i = 0; i < THREAD_MULT * MAIN_THREAD_COUNT; i ++)
                    t1[i].join();

                cout << endl << "Length after main section: " << fTable.getCount() << endl << endl;


                // -----------------------------------------------------------------
                // END SECTION

                // Wait for threads to finish...
                for (unsigned int i = 0; i < THREAD_MULT * END_THREAD_COUNT; i ++) 
                    t2[i] = thread(&fineTesting, (START + (i+1)*RANGE) / THREAD_MULT, (START + (i+2)*RANGE - 1) / THREAD_MULT, REMOVE);

                 for(unsigned int i = 0; i < THREAD_MULT * END_THREAD_COUNT; i ++)
                    t2[i].join();

                 cout << endl << "Length after end section: " << fTable.getCount() << endl << endl;

                // End time
                s2 = chrono::high_resolution_clock::now();

                cout << endl << "Run-time: " << chrono::duration_cast<chrono::milliseconds>(s2 - s1).count()
                     << " milliseconds" << endl << endl;

                delete [] t0;
                delete [] t1;
                delete [] t2;

                break;

// *****************************************************************
// quit, clear and default cases
            case 'd':
                cout << bars << endl << "Machine Statistics" << endl;

                cout << "\tHardware Cores: " << hwthd << endl;
                cout << "\tThread Count: " << THREAD_MULT * (INIT_THREAD_COUNT + MAIN_THREAD_COUNT + END_THREAD_COUNT) 
                    << endl;
                break;

            case 'n':
                cout << bars << endl << "Change start and range" << endl;

                cout << "Current start: " << START << endl
                    << "Enter new start > ";
                cin >> START;

                cout << "Current range: " << RANGE << endl
                    << "Enter new range > ";
                cin >> RANGE;
                break;

            case 'q':
                keepProcessing = false;
                cout << endl;
                break;

            default:
                cout << "Error, invalid selection. "
                     << "Please retry" << endl;
                break;
        }
    }

// *****************************************************************
// All done...

    return 0;
}
