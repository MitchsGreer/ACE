#ifndef EXPORTINTEGERARRAY_H
#define EXPORTINTEGERARRAY_H
#include <core/core.h>
#include "global.h"
//



/*!
 */
class ExportIntegerArray : public EAbstractAnalytic
{
   Q_OBJECT
public:
   class Input;
   virtual ~ExportIntegerArray() override final;
   virtual int size() const override final;
   virtual void process(const EAbstractAnalytic::Block* result) override final;
   virtual EAbstractAnalytic::Input* makeInput() override final;
   virtual void initialize();
private:
   /*!
    */
   constexpr static const int _incrementSize {16};
   /*!
    */
   IntegerArray* _in {nullptr};
   /*!
    */
   QTextStream* _stream {nullptr};
   /*!
    */
   QFile* _out {nullptr};
};



#endif
