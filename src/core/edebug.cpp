#include "edebug.h"
#include <string.h>
#include "eabstractanalytic_block.h"
#include "emetadata.h"
#include "eexception.h"



//



/*!
 */
QAtomicInteger<int> EDebug::_counter;
/*!
 */
int thread_local EDebug::_threadId {-1};
/*!
 */
int thread_local EDebug::_stackDepth {0};
/*!
 */
bool thread_local EDebug::_active {false};






/*!
 *
 * @param token  
 */
EDebug& EDebug::operator<<(Token token)
{
   switch(token)
   {
   case Quote:
      _quote = true;
      break;
   case NoQuote:
      _quote = false;
      break;
   }
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(bool value)
{
   _holder += value ? QStringLiteral("TRUE") : QStringLiteral("FALSE");
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(qint8 value)
{
   _holder += QString::number(value).toLocal8Bit();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(qint16 value)
{
   _holder += QString::number(value).toLocal8Bit();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(qint32 value)
{
   _holder += QString::number(value).toLocal8Bit();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(qint64 value)
{
   _holder += QString::number(value).toLocal8Bit();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(quint8 value)
{
   _holder += QString::number(value).toLocal8Bit();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(quint16 value)
{
   _holder += QString::number(value).toLocal8Bit();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(quint32 value)
{
   _holder += QString::number(value).toLocal8Bit();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(quint64 value)
{
   _holder += QString::number(value).toLocal8Bit();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(float value)
{
   _holder += QString::number(value).toLocal8Bit();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(double value)
{
   _holder += QString::number(value).toLocal8Bit();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(const char* value)
{
   if ( _quote )
   {
      _holder += QStringLiteral("\"");
   }
   _holder += value;
   if ( _quote )
   {
      _holder += QStringLiteral("\"");
   }
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(const void*const value)
{
   _holder += QStringLiteral("0x") + QString::number((quint64)value,16).toLocal8Bit();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(const QString& value)
{
   if ( _quote )
   {
      _holder += QStringLiteral("\"");
   }
   _holder += value.toLocal8Bit();
   if ( _quote )
   {
      _holder += QStringLiteral("\"");
   }
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(const QStringList& value)
{
   bool first {true};
   for (auto string: value)
   {
      if ( !first )
      {
         _holder += QStringLiteral(",");
      }
      else
      {
         first = false;
      }
      _holder += QStringLiteral("\"") + string.toLocal8Bit() + QStringLiteral("\"");
   }
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(const QObject*const value)
{
   *this << (void*)value;
   if ( value )
   {
      *this << NoQuote
            << QStringLiteral("(")
            << value->metaObject()->className()
            << QStringLiteral(")")
            << Quote;
   }
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(const QVariant& value)
{
   *this << value.toString();
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(const EMetadata& value)
{
   *this << NoQuote << EMetadata::typeName(value.type()) << Quote;
   switch (value.type())
   {
   case EMetadata::Bool:
      *this << NoQuote << QStringLiteral("(") << value.toBool() << QStringLiteral(")") << Quote;
      break;
   case EMetadata::Double:
      *this << NoQuote << QStringLiteral("(") << value.toDouble() << QStringLiteral(")") << Quote;
      break;
   case EMetadata::String:
      *this << NoQuote
            << QStringLiteral("(")
            << Quote
            << value.toString()
            << NoQuote
            << QStringLiteral(")")
            << Quote;
      break;
   default:
      break;
   }
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(const EAbstractAnalytic::Block*const value)
{
   *this << (void*)value;
   if ( value )
   {
      *this << NoQuote << QStringLiteral("(EAbstractAnalytic::Block,index=")
            << QString::number(value->index())
            << QStringLiteral(")")
            << Quote;
   }
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(EAbstractAnalytic::Input::Type value)
{
   switch (value)
   {
   case EAbstractAnalytic::Input::Boolean:
      *this << NoQuote << QStringLiteral("Boolean") << Quote;
      break;
   case EAbstractAnalytic::Input::Integer:
      *this << NoQuote << QStringLiteral("Integer") << Quote;
      break;
   case EAbstractAnalytic::Input::Double:
      *this << NoQuote << QStringLiteral("Double") << Quote;
      break;
   case EAbstractAnalytic::Input::String:
      *this << NoQuote << QStringLiteral("String") << Quote;
      break;
   case EAbstractAnalytic::Input::Selection:
      *this << NoQuote << QStringLiteral("Selection") << Quote;
      break;
   case EAbstractAnalytic::Input::FileIn:
      *this << NoQuote << QStringLiteral("FileIn") << Quote;
      break;
   case EAbstractAnalytic::Input::FileOut:
      *this << NoQuote << QStringLiteral("FileOut") << Quote;
      break;
   case EAbstractAnalytic::Input::DataIn:
      *this << NoQuote << QStringLiteral("DataIn") << Quote;
      break;
   case EAbstractAnalytic::Input::DataOut:
      *this << NoQuote << QStringLiteral("DataOut") << Quote;
      break;
   }
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(EAbstractAnalytic::Input::Role value)
{
   switch (value)
   {
   case EAbstractAnalytic::Input::CommandLineName:
      *this << NoQuote << QStringLiteral("CommandLineName") << Quote;
      break;
   case EAbstractAnalytic::Input::Title:
      *this << NoQuote << QStringLiteral("Title") << Quote;
      break;
   case EAbstractAnalytic::Input::WhatsThis:
      *this << NoQuote << QStringLiteral("WhatsThis") << Quote;
      break;
   case EAbstractAnalytic::Input::Default:
      *this << NoQuote << QStringLiteral("Default") << Quote;
      break;
   case EAbstractAnalytic::Input::Minimum:
      *this << NoQuote << QStringLiteral("Minimum") << Quote;
      break;
   case EAbstractAnalytic::Input::Maximum:
      *this << NoQuote << QStringLiteral("Maximum") << Quote;
      break;
   case EAbstractAnalytic::Input::Decimals:
      *this << NoQuote << QStringLiteral("Decimals") << Quote;
      break;
   case EAbstractAnalytic::Input::SelectionValues:
      *this << NoQuote << QStringLiteral("SelectionValues") << Quote;
      break;
   case EAbstractAnalytic::Input::FileFilters:
      *this << NoQuote << QStringLiteral("FileFilters") << Quote;
      break;
   case EAbstractAnalytic::Input::DataType:
      *this << NoQuote << QStringLiteral("DataType") << Quote;
      break;
   }
   return *this;
}






/*!
 *
 * @param value  
 */
EDebug& EDebug::operator<<(Ace::Analytic::AbstractMPI::Type value)
{
   switch (value)
   {
   case Ace::Analytic::AbstractMPI::Serial:
      *this << NoQuote << QStringLiteral("Serial") << Quote;
      break;
   case Ace::Analytic::AbstractMPI::OpenCL:
      *this << NoQuote << QStringLiteral("OpenCL") << Quote;
      break;
   }
   return *this;
}






/*!
 */
int EDebug::threadId()
{
   return _threadId;
}






/*!
 *
 * @param function  
 *
 * @param argumentNames  
 */
EDebug::EDebug(const char* function, const char* argumentNames):
   _function(function)
{
   if ( _threadId == -1 )
   {
      _threadId = _counter.fetchAndAddAcquire(1);
   }
   if ( _active )
   {
      return;
   }
   setupArguments(argumentNames);
   QTextStream out(stdout);
   out << "[" << _threadId << "]" << QString(_stackDepth*3,' ') << function << "\n";
   out << "[" << _threadId << "]" << QString(_stackDepth*3,' ') << "{\n";
   if ( _active )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(QObject::tr("Logic Error"));
      e.setDetails(QObject::tr("Cannot DEBUG TRACE methods used inside a EDebug operator."));
      throw e;
   }
   ++_stackDepth;
}






/*!
 */
EDebug::~EDebug()
{
   if ( _active )
   {
      return;
   }
   delete[] _argumentNames;
   --_stackDepth;
   if ( _stackDepth < 0 )
   {
      _stackDepth = 0;
   }
   QTextStream out(stdout);
   out << "[" << _threadId << "]" << QString(_stackDepth*3,' ') << "}\n";
}






/*!
 *
 * @param depth  
 */
void EDebug::setArgument(int depth)
{
   Q_UNUSED(depth)
}






/*!
 *
 * @param argumentNames  
 */
void EDebug::setupArguments(const char* argumentNames)
{
   size_t size {strlen(argumentNames) + 1};
   _argumentNames = new char[size];
   memcpy(_argumentNames,argumentNames,size);
   int last {0};
   for (size_t i = 0; i < size ;++i)
   {
      if ( argumentNames[i] == ',' || argumentNames[i] == '\0' )
      {
         _argumentNames[i] = '\0';
         _argumentIndexes << last;
         last = i + 1;
      }
      else
      {
         _argumentNames[i] = argumentNames[i];
      }
   }
   _argumentValues.resize(_argumentIndexes.size());
}
