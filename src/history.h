#ifndef ACCELCOMPENG_HISTORY_H
#define ACCELCOMPENG_HISTORY_H
#include <string>
#include "histitem.h"
#include "exception.h"
namespace AccelCompEng
{



/// @brief Provides history of file.
///
/// Provides entire history for one file, including all children of inputs that
/// were involved in the creation of this file.
///
/// @author Josh Burns
/// @date 25 March 2016
class History : private HistItem
{
public:
   // *
   // * DECLERATIONS
   // *
   class Iterator;
   using FPtr = FileMem::Ptr;
   using HistItem::sync;
   using HistItem::timeStamp;
   using HistItem::fileName;
   using HistItem::object;
   using HistItem::command;
   using HistItem::addr;
   // *
   // * BASIC METHODS
   // *
   History(FileMem&,FPtr = FileMem::nullPtr);
   // *
   // * COPY METHODS
   // *
   History(const History&) = delete;
   History& operator=(const History&) = delete;
   // *
   // * MOVE METHODS
   // *
   History(History&&) = delete;
   History& operator=(History&&) = delete;
   // *
   // * FUNCTIONS
   // *
   void add_child(const History&);
   bool has_child() const;
   Iterator begin();
   Iterator end();
};



/// @brief Iterates through all children of history.
///
/// Iterates through all children of history file, also allowing iteration
/// through all children of each child recursively until end of all children
/// reached.
///
/// @author Josh Burns
/// @date 25 March 2016
class History::Iterator
{
public:
   // *
   // * DECLERATIONS
   // *
   friend class History;
   using FPtr = History::FPtr;
   // *
   // * FUNCTIONS
   // *
   Iterator child();
   bool has_child() const;
   // *
   // * OPERATORS
   // *
   HistItem load();
   void operator++();
   bool operator!=(const Iterator&);
private:
   using Skim = HistItemData::Skim;
   // *
   // * BASIC METHODS
   // *
   Iterator(FileMem*,FPtr = FileMem::nullPtr);
   // *
   // * VARIABLES
   // *
   /// Pointer to file memory object.
   FileMem* _mem;
   /// Compact version of history item chunk for navigating.
   Skim _skim;
};



}
#endif
