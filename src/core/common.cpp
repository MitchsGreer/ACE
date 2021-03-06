#include "common.h"
#include "edatastream.h"
#include "emetadata.h"
#include "emetaarray.h"
#include "emetaobject.h"
#include "edebug.h"
#include "eabstractanalytic_block.h"



//






/*!
 * This reads in a metadata value from a data stream object. If the metadata is an 
 * array or object this recursively reads in all metadata contained therein. 
 *
 * @param stream The data stream to be read from. 
 *
 * @param meta The metadata object that will be overwritten with the metadata read 
 *             from the data stream. 
 *
 * @return Constant reference to data stream object. 
 */
const EDataStream& operator>>(const EDataStream& stream, EMetadata& meta)
{
   EDEBUG_FUNC(&stream,&meta)

   // Read in the metadata type and overwrite the given metadata with a new object of 
   // that type. 
   quint8 type;
   stream >> type;
   meta = EMetadata(static_cast<EMetadata::Type>(type));

   // If the type is an array go to step 4, else if the type is an object go to step 
   // 5, else go to step 3. 
   switch (meta.type())
   {

   // Read in the value of the metadata to the given object. Return a reference to 
   // the data stream. 
   case EMetadata::Bool:
   {
      quint8 val;
      stream >> val;
      meta.toBool() = val;
      break;
   }
   case EMetadata::Double:
      stream >> meta.toDouble();
      break;
   case EMetadata::String:
      stream >> meta.toString();
      break;
   case EMetadata::Bytes:
      stream >> meta.toBytes();
      break;

   // Read in the size of metadata objects the array contains in the data stream. 
   // Read in all metadata objects the array contains, inserting each one into the 
   // given metadata array object. Return a reference to the data stream. 
   case EMetadata::Array:
      {
         quint32 size;
         stream >> size;
         for (quint32 i = 0; i < size ;++i)
         {
            EMetadata child;
            stream >> child;
            meta.toArray().append(child);
         }
         break;
      }

   // Read in the size of key and metadata value pairs the object contains in the 
   // data stream. Read in all keys and metadata objects the object contains, 
   // inserting each key and metadata value pair into the object. Return a reference 
   // to the data stream. 
   case EMetadata::Object:
      {
         quint32 size;
         stream >> size;
         for (quint32 i = 0; i < size ;++i)
         {
            QString key;
            EMetadata child;
            stream >> key >> child;
            meta.toObject().insert(key,child);
         }
         break;
      }
   default: break;
   }
   return stream;
}






/*!
 * This writes out a metadata value to the given data stream from the metadata 
 * object given. If the given metadata object is an array or object this 
 * recursively writes out all metadata contained therein. 
 *
 * @param stream The data stream object to be written to. 
 *
 * @param meta The metadata object whose value will be written to the data stream. 
 *
 * @return Reference to data stream object. 
 */
EDataStream& operator<<(EDataStream& stream, const EMetadata& meta)
{
   EDEBUG_FUNC(&stream,&meta)

   // Write out the metadata type from the given object to the data stream. 
   quint8 type {static_cast<quint8>(meta.type())};
   stream << type;

   // If the type is an array go to step 4, else if the type is an object go to step 
   // 5, else go to step 3. 
   switch (meta.type())
   {

   // Write out the value of the given metadata object to the data stream. 
   case EMetadata::Bool:
      stream << meta.toBool();
      break;
   case EMetadata::Double:
      stream << meta.toDouble();
      break;
   case EMetadata::String:
      stream << meta.toString();
      break;
   case EMetadata::Bytes:
      stream << meta.toBytes();
      break;

   // Write out the number of metadata objects the array contains. Write out all 
   // metadata values the array contains to the data stream. Return a reference to 
   // the data stream. 
   case EMetadata::Array:
      stream << static_cast<quint32>(meta.toArray().size());
      for (auto child : meta.toArray())
      {
         stream << child;
      }
      break;

   // Write out the number of key and metadata value pairs the object contains. Write 
   // out all key and metadata value pairs the object contains to the data stream. 
   // Return a reference to the data stream. 
   case EMetadata::Object:
      stream << static_cast<quint32>(meta.toObject().size());
      for (auto i = meta.toObject().begin(); i != meta.toObject().end() ;++i)
      {
         stream << i.key() << *i;
      }
      break;
   default: break;
   }
   return stream;
}






/*!
 * Writes a metadata object to the debug stream for debugging output. 
 *
 * @param debug The debug object which has the given metadata object value streamed 
 *              to it. 
 *
 * @param meta The metadata value that is streamed to the given debug object. 
 *
 * @return Reference to the debug object. 
 */
EDebug& operator<<(EDebug& debug, const EMetadata*const meta)
{
   // Disable quoting of strings. 
   debug << EDebug::NoQuote;

   // Append the fact that this is metadata and its type. 
   switch (meta->_type)
   {
   case EMetadata::Null:
      debug << QStringLiteral("EMetadata(null");
      break;
   case EMetadata::Bool:
      debug << QStringLiteral("EMetadata(bool,");
      break;
   case EMetadata::Double:
      debug << QStringLiteral("EMetadata(double,");
      break;
   case EMetadata::String:
      debug << QStringLiteral("EMetadata(string,\"");
      break;
   case EMetadata::Bytes:
      debug << QStringLiteral("EMetadata(bytes");
      break;
   case EMetadata::Array:
      debug << QStringLiteral("EMetadata(array");
      break;
   case EMetadata::Object:
      debug << QStringLiteral("EMetadata(object");
      break;
   }

   // Check to see if this metadata object has data. 
   if ( meta->_data )
   {
      // Append the value of this metadata's data as a string. 
      switch (meta->_type)
      {
      case EMetadata::Bool:
         debug << ( *reinterpret_cast<bool*>(meta->_data) ? QStringLiteral("TRUE") : QStringLiteral("FALSE") )
               << QStringLiteral(")");
         break;
      case EMetadata::Double:
         debug << *reinterpret_cast<double*>(meta->_data)
               << QStringLiteral(")");
         break;
      case EMetadata::String:
         debug << *reinterpret_cast<QString*>(meta->_data)
               << QStringLiteral("\")");
         break;
      default:
         debug << QStringLiteral(")");
         break;
      }
   }

   // Else this must be a null type so just add a closing parenthesis. 
   else
   {
      debug << QStringLiteral(")");
   }

   // Enable quoting of strings and return a reference to the debug object. 
   debug << EDebug::Quote;
   return debug;
}






/*!
 * Writes a abstract analytic block object to the debug stream for debugging 
 * output. 
 *
 * @param debug The debug object which has the given abstract analytic block value 
 *              streamed to it. 
 *
 * @param value The abstract analytic object that is streamed to the given debug 
 *              object. 
 *
 * @return Reference to the debug object. 
 */
EDebug& operator<<(EDebug& debug, const EAbstractAnalytic::Block*const value)
{
   // Append the raw pointer value to the given debug stream. 
   debug << (void*)value;

   // If the given pointer is not null then append the fact it is a block and give 
   // its index value. 
   if ( value )
   {
      debug << EDebug::NoQuote << QStringLiteral("(EAbstractAnalytic::Block,index=")
            << QString::number(value->_index)
            << QStringLiteral(")")
            << EDebug::Quote;
   }

   // Return a reference to the debug object. 
   return debug;
}
