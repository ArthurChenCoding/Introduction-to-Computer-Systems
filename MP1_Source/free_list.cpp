/* 
    File: free_list.cpp

    Author: Arthur Chen
            Department of Computer Science
            Texas A&M University
    Date  : Aug 28th

    Modified: 

    This file contains the implementation of the class FreeList.

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <iostream>
#include "free_list.hpp"

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
/* FUNCTIONS FOR CLASS SegmentHeader */
/*--------------------------------------------------------------------------*/

SegmentHeader::SegmentHeader(size_t _length, bool _is_free) {
  length = _length;
  is_free = false;
  cookie = COOKIE_VALUE;
  // You may need to initialize more members here!
  previous = nullptr;
  next = nullptr;
}

SegmentHeader::~SegmentHeader() {
  // You may need to add code here.
}


void SegmentHeader::CheckValid() {
  if (cookie != COOKIE_VALUE) {
    cout << "INVALID SEGMENT HEADER!!" << endl;
    assert(false);
    // You will need to check with the debugger to see how we got into this
    // predicament.
  }
  // cout << "VALID SEGMENT HEADER!!" << endl;
}

SegmentHeader* SegmentHeader::Split(size_t _length) {
  // if (this->index == 1) { 
  //   //cout << "avoiding to split size 2 segment, returning this." << endl;
  //   return this;
  // }
  //cout << "calling split" << endl;
  SegmentHeader* seg_new = new((void*)((char*)this + _length))SegmentHeader(this->length - _length);
  // set index and type and inh
  seg_new->inh = this->type;
  this->index--;
  this->type = -1;
  // left inh doesn't change
  seg_new->index = this->index - 1;
  seg_new->type = 1;
  seg_new->CheckValid();
  this->length = _length;
  return seg_new;
}

bool SegmentHeader::isFree() {
  if (is_free){
    return true;
  }
  return false;
}

// size_t SegmentHeader::getLength() {
//   return length;
// }

// void SegmentHeader::changeLength(size_t _length) {
//   length = _length;
// }

void SegmentHeader::setFree(bool status) {
  if(status) {
    is_free = true;
  }else {
    is_free = false;
  }
}

// void SegmentHeader::PrintSeg() {
//   if (this == nullptr) {
//     cout << "invalid segment pointer or empty freelist." << endl;
//     return;
//   }
//   cout << "The length of this ";
//   if (this->isFree()) {
//     cout << "freed ";
//   }else {
//     cout << "not freed ";
//   }
//   cout << "segment is " << this->length;
//   cout << " bytes, including size of the SH which is " << sizeof(SegmentHeader);
//   cout << " bytes, and the address is " << this << ". The index, type, and inh are " 
//   << this->index << ", " << this->type << ", " << this ->inh << ".";
// }

void SegmentHeader::PrintSeg() {
    cout << this->length << "(" << (void*)((char*)this + sizeof(SegmentHeader)) << ")";
}
// SegmentHeader* SegmentHeader::getNext() {
//   return this->next;
// }

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS FreeList */
/*--------------------------------------------------------------------------*/

bool FreeList::isEmpty() {
  return (this->head == nullptr);
}

FreeList::FreeList() {
  head = nullptr;
  tail = nullptr;
}

FreeList::~FreeList() {
  // You may need to add code here.
}

bool FreeList::Add(SegmentHeader * _segment) {
  //cout << "calling Add" << endl;
  // if (_segment->isFree() == true) {
  //   cout << "attempting to add a freed segment. Why it is freed?" << endl;
  //   assert(false);
  // }
  _segment->setFree(true);
  if (head == nullptr) { // first SH
    head = _segment;
    tail = _segment;
    _segment->next = nullptr;
    _segment->previous = nullptr;
    //cout << "Adding first segment into freelist"<< endl;
    return true;
  }
  // cout << "in Add2" << endl;
  // always insert before the head
  _segment->next = head;
  _segment->previous = nullptr;
  // cout << "in Add3" << endl;
  head->previous = _segment;
  head = _segment;
  //cout << "checking if valid in Add" << endl;
  _segment->CheckValid();
  return true;
}

bool FreeList::Remove(SegmentHeader * _segment) {
  //cout << "calling Remove" << endl;
  //cout << "the length of the removed sgement is " << _segment->length << endl;
  // if (_segment->isFree() == false) {
  //   cout << "attempting to remove a occupied segment. Why is it occupied?" << endl;
  //   assert(false);
  // }
  _segment->setFree(false);
  if (_segment == nullptr) {
    cout << "remove fail cuz segment is null" << endl;
    assert(false);
  }
  if (_segment->previous == nullptr) { // removing first SH && only segment
    //cout << "Removing the first segment of the FreeList" << endl;
    head = _segment->next;
    _segment->next = nullptr;
    _segment->previous = nullptr;
    return true;
  }
  if (_segment->next == nullptr) { // removing last SH
    //cout << "Removing the last segment of the FreeList" << endl;
    tail = _segment->previous;
    _segment->next = nullptr;
    _segment->previous = nullptr;
    return true;
  }
  SegmentHeader* seg = _segment->previous;
  SegmentHeader* seg2 = _segment->next;
  seg->next = seg2;
  seg2->previous = seg;
  _segment->next = nullptr;
  _segment->previous = nullptr;
  //cout << "checking if valid in Remove" << endl;
  seg->CheckValid();
  return true;
}

SegmentHeader* FreeList::getHead(){
  return head;
}

// void FreeList::PrintFreeList(){
//   SegmentHeader* seg = this->head;
//   if (seg == nullptr) {
//     cout << "head is null, empty";
//     return;
//   }
//   seg->PrintSeg();
//   seg = seg->next;
//   // cout << "seg is " << seg << endl;
//   for (int i = 0 ; i < 10; i++) {
//     if ((seg != nullptr) && (seg->next != nullptr)){
//       seg->CheckValid();
//       // cout << "if loop entered" << endl;
//       seg->PrintSeg();
//       // cout << "seg is " << seg << endl;
//       seg = seg->next;
//       // cout << "test 2 and seg is " << seg << endl;
//     }
//   }
//   cout << "End of PrintFreeList" << endl << endl;
//   return;
// }

void FreeList::PrintFreeList() {
    SegmentHeader* seg = this->head;
    if(seg == nullptr){
        cout << "Empty" << endl;
        return;
    }
    cout << "Head: ";
    for (int i = 0; i < 10; i++) {
      if(seg){
          seg->PrintSeg();
          cout << " > ";
          seg = seg->next;
      }
    }
    cout << "\b\b\b :Tail" << endl;
}