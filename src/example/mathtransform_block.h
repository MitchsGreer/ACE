#ifndef MATHTRANSFORM_BLOCK_H
#define MATHTRANSFORM_BLOCK_H
#include "mathtransform.h"
//



/*!
 * This implements both work and result blocks for its parent math transform 
 * analytic class. Most real analytic classes will have to implement separate work 
 * and result blocks. 
 */
class MathTransform::Block : public EAbstractAnalytic::Block
{
   Q_OBJECT
public:
   /*!
    * Constructs a new block in an uninitialized null state. 
    */
   explicit Block() = default;
   explicit Block(int index);
   explicit Block(int index, qint32 number);
   /*!
    * The integer for this block that can either be a work block which is used for 
    * input or a result block which is used for output. 
    */
   qint32 _number;
protected:
   virtual void write(QDataStream& stream) const override final;
   virtual void read(QDataStream& stream) override final;
};



#endif
