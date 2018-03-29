#ifndef EMETADATA_H
#define EMETADATA_H
#include "global.h"



class QString;
class QByteArray;
//



/*!
 * This holds a single metadata value. It is designed as a variant class which 
 * means it can hold any possible value type supported by the metadata system. 
 * Because this can be an array or object it can possibly hold additional metadata 
 * objects as children. Care should be taken to not attempt to convert a metadata 
 * object to a type that it is not. If the object attempts to convert itself to a 
 * type it is not an exception will be thrown. 
 */
class EMetadata
{
public:
   /*!
    * This holds all possible types a metadata object can contain. 
    */
   enum Type
   {
      /*!
       * This means the object is empty and contains no data. 
       */
      Null
      /*!
       * This means the object is a Boolean. 
       */
      ,Bool
      /*!
       * This means the object is a real number. 
       */
      ,Double
      /*!
       * This means the object is a string. 
       */
      ,String
      /*!
       * This means the object is an array of bytes. The QByteArray type is used to store 
       * this type. 
       */
      ,Bytes
      /*!
       * This means the object is an array of additional metadata objects. 
       */
      ,Array
      /*!
       * This means the object is a mapping of additional metadata objects. The mapping 
       * is done using strings as keywords. 
       */
      ,Object
   };
   EMetadata(Type type = Null);
   EMetadata(const EMetadata& object);
   EMetadata(EMetadata&& object);
   EMetadata& operator=(const EMetadata& object);
   EMetadata& operator=(EMetadata&& object);
   ~EMetadata();
   bool isNull() const;
   bool isBool() const;
   bool isDouble() const;
   bool isString() const;
   bool isBytes() const;
   bool isArray() const;
   bool isObject() const;
   bool& toBool() const;
   double& toDouble() const;
   QString& toString() const;
   QByteArray& toBytes() const;
   EMetaArray& toArray() const;
   EMetaObject& toObject() const;
   bool& toBool();
   double& toDouble();
   QString& toString();
   QByteArray& toBytes();
   EMetaArray& toArray();
   EMetaObject& toObject();
   EMetadata::Type type() const;
   static QString typeName(Type type);
private:
   void checkType(Type type) const;
   void clear();
   void copy(const void* data);
   /*!
    * This stores what type this object contains. 
    */
   Type _type;
   /*!
    * This is a pointer to the actual data, if any, that this object contains. 
    */
   void* _data {nullptr};
};



#endif