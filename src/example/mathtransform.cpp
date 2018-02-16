#include <limits>
#include <QThread>

#include "mathtransform.h"
#include "integerarray.h"
#include "datafactory.h"

#define ADDITION "Addition"
#define SUBTRACTION "Subtraction"
#define MULTIPLICATION "Multiplication"
#define DIVISION "Division"



using namespace std;






int MathTransform::getArgumentCount()
{
   // Return total number of arguments
   return Total;
}






EAbstractAnalytic::ArgumentType MathTransform::getArgumentData(int argument)
{
   using Type = EAbstractAnalytic::ArgumentType;

   // determine which argument is being queried
   switch (argument)
   {
   case InputData:
      // This is input data, return DataIn type
      return Type::DataIn;
   case OutputData:
      // This is output data, return DataOut type
      return Type::DataOut;
   case OperationType:
      // This is operation type, return combo box type
      return Type::Combo;
   case Amount:
      // This is amount, return integer type
      return Type::Integer;
   default:
      // Unknown argument, return base bool type
      return Type::Bool;
   }
}






QVariant MathTransform::getArgumentData(int argument, EAbstractAnalytic::Role role)
{
   using Role = EAbstractAnalytic::Role;

   // determine which argument role is being queried
   switch (role)
   {
   case Role::CommandLineName:
      // determine which argument is being queried
      switch (argument)
      {
      case InputData:
         // This is input data, return string
         return QString("in");
      case OutputData:
         // This is output data, return string
         return QString("out");
      case OperationType:
         // This is operation type, return string
         return QString("type");
      case Amount:
         // This is amount, return string
         return QString("amount");
      default:
         // Unknown argument, return empty string
         return QString();
      }
   case Role::Title:
      // determine which argument is being queried
      switch (argument)
      {
      case InputData:
         // This is input data, return title
         return QString("Input:");
      case OutputData:
         // This is output data, return title
         return QString("Output:");
      case OperationType:
         // This is operation type, return title
         return QString("Operation Type:");
      case Amount:
         // This is amount, return title
         return QString("Amount:");
      default:
         return QString();
      }
   case Role::WhatsThis:
      // determine which argument is being queried
      switch (argument)
      {
      case InputData:
         // This is input data, return help
         return QString("Input data object of type Integer Array.");
      case OutputData:
         // This is output data, return help
         return QString("Output data object of type Integer Array.");
      case OperationType:
         // This is operation type, return help
         return QString("The type of operation to be done to output values.");
      case Amount:
         // This is amount, return help
         return QString("The number used for the given type of operation.");
      default:
         // Unknown argument, return empty string
         return QString();
      }
   case Role::Minimum:
      // determine which argument is being queried
      switch (argument)
      {
      case Amount:
         return numeric_limits<int>::min();
      default:
         return QVariant();
      }
   case Role::Maximum:
      // determine which argument is being queried
      switch (argument)
      {
      case Amount:
         return numeric_limits<int>::max();
      default:
         return QVariant();
      }
   case Role::ComboValues:
      // determine which argument is being queried
      switch (argument)
      {
      case OperationType:
         // This is operation type, return list of combo box options
         return QStringList() << ADDITION << SUBTRACTION << MULTIPLICATION << DIVISION;
      default:
         // Any other argument is not combo box so return empty list
         return QStringList();
      }
      break;
   case Role::DataType:
      // determine which argument is being queried
      switch (argument)
      {
      case InputData:
      case OutputData:
         return DataFactory::IntegerArrayType;
      default:
         return QVariant();
      }
   default:
      // unknown role is being queried simply return empty variant
      return QVariant();
   }
}






void MathTransform::setArgument(int argument, QVariant value)
{
   // determine which argument is being queried
   switch (argument)
   {
   case OperationType:
      // This is operation type, set operation type to new value
      {
         QString argument = value.toString();
         if ( argument == ADDITION )
         {
            _type = OperType::Addition;
         }
         else if ( argument == SUBTRACTION )
         {
            _type = OperType::Subtraction;
         }
         else if ( argument == MULTIPLICATION )
         {
            _type = OperType::Multiplication;
         }
         else if ( argument == DIVISION )
         {
            _type = OperType::Division;
         }
         break;
      }
   case Amount:
      // This is amount, set amount to new value
      _amount = value.toInt();
      break;
   }
}






void MathTransform::setArgument(int /*argument*/, QFile* /*file*/)
{
   // This analytic has no file arguments so this virtual function does nothing
}






void MathTransform::setArgument(int argument, EAbstractData* data)
{
   switch (argument)
   {
   case InputData:
      // This is input data, set reference
      _in = dynamic_cast<IntegerArray*>(data);
      break;
   case OutputData:
      // This is output data, set reference
      _out = dynamic_cast<IntegerArray*>(data);
      break;
   }
}






bool MathTransform::initialize()
{
   // make sure we have valid inputs and outputs
   if ( !_in || !_out )
   {
      // If failure occured create exception to report failure
      E_MAKE_EXCEPTION(e);
      e.setTitle(QObject::tr("Argument Error"));
      e.setDetails(QObject::tr("Did not get valid input and/or output arguments."));
      throw e;
   }

   // Clear any numbers the output data may hold
   _out->_numbers.clear();

   // Reserve the amount of numbers output data will hold
   _out->_numbers.reserve(_in->_numbers.size());
   _out->_futureSize = _in->_numbers.size();

   // Return true so the output data object pre-allocates file space
   return true;
}






void MathTransform::runSerial()
{
   // Get total amount of numbers to process
   int total {_in->_numbers.size()};

   // Initialize variables for keeping track of progress
   int done {0};
   int lastPercent {0};

   // Increment through all numbers of input data
   for (auto i = _in->_numbers.begin(); i != _in->_numbers.end() ;++i)
   {
      // Do mathematical operation with input data value and push to end of output data
      switch (_type)
      {
      case OperType::Addition:
         _out->_numbers.push_back((*i)+_amount);
         break;
      case OperType::Subtraction:
         _out->_numbers.push_back((*i)-_amount);
         break;
      case OperType::Multiplication:
         _out->_numbers.push_back((*i)*_amount);
         break;
      case OperType::Division:
         _out->_numbers.push_back((*i)/_amount);
         break;
      }

      // Increment done counter and calculate new percentage complete
      ++done;
      int newPercent = 100*done/total;

      // If percentage complete has changed emit progressed(int) signal
      if ( newPercent != lastPercent )
      {
         lastPercent = newPercent;
         emit progressed(newPercent);
      }

      // sleep to make this appear to take a lot time
      QThread::msleep(100);
   }
}