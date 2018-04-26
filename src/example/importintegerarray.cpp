#include "importintegerarray.h"
#include "importintegerarray_input.h"
#include "integerarray.h"
#include "datafactory.h"
//






/*!
 */
ImportIntegerArray::~ImportIntegerArray()
{
   delete _stream;
}






/*!
 * Implements the interface that returns the total number of blocks this analytic 
 * must process as steps or blocks of work. 
 *
 * @return Total number of blocks or steps that this analytic must work on. 
 */
int ImportIntegerArray::size() const
{
   return (_in->size()/_incrementSize) + 1;
}






/*!
 * Implements the interface that processes the given index with a possible block of 
 * results if this analytic produces work blocks. This analytic implementation has 
 * no work blocks. 
 *
 * @param result Pointer to the block of results that is read in. 
 */
void ImportIntegerArray::process(const EAbstractAnalytic::Block* result)
{
   while ( _in->pos() < ((result->index() + 1)*_incrementSize) )
   {
      int value;
      *_stream >> value;
      if ( _stream->status() != QTextStream::Ok )
      {
         break;
      }
      _out->_numbers << value;
   }
}






/*!
 * Implements the interface that makes a new input object and returns its pointer. 
 *
 * @return Pointer to new input object. 
 */
EAbstractAnalytic::Input* ImportIntegerArray::makeInput()
{
   return new Input(this);
}






/*!
 * Implements the interface that initializes this analytic. This implementation 
 * checks to make sure the input file and output data object has been set. 
 */
void ImportIntegerArray::initialize()
{
   if ( !_in || !_out )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Invalid Argument"));
      e.setDetails(tr("Did not get valid input and/or output arguments."));
      throw e;
   }
   _stream = new QTextStream(_in);
}
