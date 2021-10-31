/* 
 File: my_allocator.cpp
 
 Author: Arthur Chen
 Department of Computer Science
 Texas A&M University
 Date  : Aug 28th
 
 Modified:
 
 This file contains the implementation of the class MyAllocator.
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cstdlib>
#include "my_allocator.hpp"
#include <assert.h>
#include <iostream>
#include "free_list.hpp"
#include <math.h>

/*--------------------------------------------------------------------------*/
/* NAME SPACES */ 
/*--------------------------------------------------------------------------*/

using namespace std;
/* I know, it's a bad habit, but this is a tiny program anyway... */

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* UTILITY FUNCTIONS */
/*--------------------------------------------------------------------------*/

int GetFib(int index){
    int a = 1; 
    int b = 2;
    for(int i=1; i<= index; i++){
        int c = a + b;
        a = b;
        b = c;
    }
    return a;
}

int FibRoundLow(int n) {
    int index=0; 
    while(true) {
        if (GetFib(index) <= n) {
            index++;
            continue;
        }
        return GetFib(index-1);
    }
}

int FibRoundUp(int n) {
    int index=0; 
    while(true) {
        if (GetFib(index) <= n) {
            index++;
            continue;
        }
        return GetFib(index);
    }
}

int inverseFib(int n) {
    for (int i = 0; i < 100; i++){
        if (n == GetFib(i)){
            return i;
        }
    }
    return -1;
}

// void MyAllocator::PrintAll() {
//     for (int i = FibIndex; i >= 0 ; i--) {
//         cout << "The " << i << "th row of fl is: ";
//         fl[i].PrintFreeList();
//         cout << endl;
//     }
// }

void MyAllocator::PrintAll(){
    for(int i = FibIndex; i >= 0; i--){
        cout << "Row " << i << ": ";
        fl[i].PrintFreeList();
    }
    cout << endl;
}

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS MyAllocator */
/*--------------------------------------------------------------------------*/

MyAllocator::MyAllocator(size_t _basic_block_size, size_t _size) {
    cout << "MyAllocator constuctor called" << endl;
    // cout << "_size is " << _size << endl;
    start = std::malloc(_size);
    block_size = _basic_block_size;
    int temp = _size/block_size; // assume it is integer
    FibNum = FibRoundLow(temp);
    FibIndex = inverseFib(FibNum);
    // cout << "temp size was " << temp << " and is rounded to " << FibNum << endl;
    SegmentHeader* seg1 = new(start)SegmentHeader(FibNum * block_size);
    // construct a array of freelist then loop through it
    fl = new FreeList[FibIndex];
    fl[FibIndex].Add(seg1); // add to biggest FL
    seg1->index = FibIndex;
    seg1->type = 0;
    seg1->inh = 0;
    // cout << "FreeLists after Constructor:" << endl;
    // PrintAll();
}

MyAllocator::~MyAllocator() {
    // Same here...
}

// void* MyAllocator::Malloc(size_t _length) {
//     // This empty implementation just uses C standard library malloc
//     if (_length > remaining) {
//         return current_start; 
//     }
//     void* result = current_start;
//     // char* current_start;
//     // current_start = current_start + sizeof(_length);
//     for (size_t i=0;i<_length;i++){
//         current_start++;
//     }
//     remaining -= _length;
//     cout << "MyAllocator::Malloc called with length = " << _length << endl;
//     return result;
// }

void* MyAllocator::Malloc(size_t _length) {
    //cout << "calling Malloc" << endl;
    int multiple = ceil((_length + sizeof(SegmentHeader)) / block_size);
    // multiple++;
    multiple = FibRoundUp(multiple);
    size_t length = block_size * multiple;
    cout << "The rounded memory is "<< length << "." << endl;
    while (true) {
        if (length == block_size && (fl[1].isEmpty() != true) && (fl[0].isEmpty() == true)){
            // Special case !! Give out an size 2 
            //cout << "Special case !! Give out an size 2 " << endl;
            SegmentHeader* segSP = fl[1].getHead();segSP->CheckValid();
            fl[1].Remove(segSP);
            segSP->setFree(false);
            void* ptr = (void*)((char*)segSP + sizeof(SegmentHeader));
            if (ptr == nullptr){cout << " ptr is not valid" << endl;}else {cout << "the Malloc returns " << ptr << endl;}
            // cout << "FreeLists after Malloc:" << endl;
            // PrintAll();
            return ptr;
        }
        int iterator = 0;
        while(iterator <= FibIndex && (fl[iterator].isEmpty() || GetFib(iterator)*block_size < length)) {
            iterator++;
        }
        if (iterator > FibIndex) {cout << "iterator went out of index, the requested memory is too large." << endl;assert(false);} // error reporting
        SegmentHeader* seg = fl[iterator].getHead();seg->CheckValid();
        fl[iterator].Remove(seg);
        if (seg->length == length) {
            // success!!
            seg->setFree(false);
            void* ptr = (void*)((char*)seg + sizeof(SegmentHeader));
            if (ptr == nullptr){
                cout << " ptr is not valid" << endl;
            }else {
                // cout << "the Malloc returns " << ptr << endl;
            }
            // cout << "FreeLists after Malloc:" << endl;
            // PrintAll();
            return ptr;
        }
        /* else seg too long */ //cout << "this free segment is too large so start spliting." << endl;
        SegmentHeader* seg2 = seg->Split(GetFib(iterator-1)*block_size);//cout << "Spliting done" << endl;seg2->CheckValid();
        // change fl of segs
        fl[iterator-1].Add(seg);
        fl[iterator-2].Add(seg2);
        // cout << "FreeLists after Malloc:" << endl;
        // PrintAll();
    }
}


// bool MyAllocator::Free(void* _a) {
//     // This empty implementation just uses C standard library free
//     cout << "MyAllocator::Free called but does nothing" << endl;
//     // std::free(_a);
//     return true;
// }

bool MyAllocator::Free(void* _a) {
    //cout << "MyAllocator::Free called" << endl;
    SegmentHeader* seg2;
    SegmentHeader* seg = (SegmentHeader*)((char*)_a - sizeof(SegmentHeader));//cout << "checking if valid in Free" << endl;seg->CheckValid();
    // step 1
    while(true) {
        int temp = 0;
        if (seg->type == -1) {
            temp = seg2->index+1;
            seg2 = (SegmentHeader*)((char*)seg + GetFib(seg->index)*block_size);
        }else if(seg->type == 1) {
            temp = seg2->index-1;
            seg2 = (SegmentHeader*)((char*)seg - GetFib(seg->index+1)*block_size);
        }else { // biggest segment
            fl[FibIndex].Add(seg);
            return true;
        }
        // step 2
        if ((temp != seg->index) || seg2->isFree() != true) {
            // cannot coalesce
            fl[seg->index].Add(seg);
            // cout << "FreeList after Freed" << endl;
            // PrintAll();
            return true;
        }
        // step 3
        SegmentHeader* segB;
        SegmentHeader* segS;
        if (seg->type == -1) {
            segB = seg;
            segS = seg2;
        }else {
            segB = seg2;
            segS = seg;
        }
        SegmentHeader* segM = segB;
        segM->index = segB->index+1;
        segM->type = segS->inh;
        segM->inh = segB->inh;
        seg = segM;
    }
}

