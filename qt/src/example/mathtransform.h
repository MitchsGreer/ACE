#ifndef ANALYTIC_H
#define ANALYTIC_H
#include <core/AceCore.h>



class IntegerArray;



// Analytic type that implements math transform from analytic factory
//
// This analytic takes one integer array, does a mathematical operation on each value, and stores
// the new values to a new integer array.
class MathTransform : public EAbstractAnalytic
{
   Q_OBJECT
public:
   enum Arguments
   {
      InputData = 0
      ,OutputData
      ,OperationType
      ,Amount
      ,Total
   };
   int getArgumentCount() override final;
   ArgumentType getArgumentData(int argument) override final;
   QVariant getArgumentData(int argument, Role role) override final;
   void setArgument(int argument, QVariant value) override final;
   void setArgument(int argument, QFile* file) override final;
   void setArgument(int argument, EAbstractData* data) override final;
   bool initialize() override final;
   int getBlockSize() override final;
   bool runBlock(int block) override final;
   void finish() override final;
private:
   // Enumerated types to define all possible operations
   enum class OperType
   {
      Addition
      ,Subtraction
      ,Multiplication
      ,Division
   };
   // Integer array input
   IntegerArray* _in {nullptr};
   // Integer array output
   IntegerArray* _out {nullptr};
   // Type of operation that will be done
   OperType _type {OperType::Addition};
   // Value that will be used for operation
   int _amount {0};
};



#endif
