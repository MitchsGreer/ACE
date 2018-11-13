# Summary

This document introduces the framework of the ACE library by providing a basic tutorial mimicking the classic hello world of programming introductions. It starts by creating a hello world data class, followed by a hello world analytic class, and finishes by wrapping it all together and including it in the ACE plugin factory object.

# Prerequisite Knowledge

In order to use ACE one should be well versed in C++ up to the C++11 standard and have basic knowledge with Qt. While general knowledge of both is required there are specific subjects that ACE uses frequently.

For C++ ACE uses abstract classes and the abstract factory design pattern for interfacing with an application's data and analytic classes. C++ Stream operators are also used heavily with data classe's data object's file IO.

For Qt the most important topics to understand is using its basic string classes QString and QByteArray.

- C++11
    - [Abstract Classes](https://www.tutorialspoint.com/cplusplus/cpp_interfaces.htm)
    - [Abstract Factory Design Pattern](https://www.codeproject.com/Articles/331304/Understanding-and-Implementing-Abstract-Factory)
    - [Streaming Operators](https://www.cprogramming.com/tutorial/c++-iostreams.html)
- [Qt Core](http://doc.qt.io/qt-5/qtcore-index.html)
    - [OString](https://doc.qt.io/qt-5/qstring.html)
    - [QByteArray](https://doc.qt.io/qt-5/qbytearray.html)

# Data Class

Creating a data class is the first thing you should do when implementing a new idea when using the ACE library since analytic objects depend on and create new data objects. Our HelloWorld data object will be responsible with holding the value of a 32 bit integer.

First we will create a skeleton class which will compile, but does absolutely nothing useful.

```
#include <ace/core/core.h>



class HelloWorld : public EAbstractData
{
public:
   virtual qint64 dataEnd() const override final;
   virtual void readData() override final;
   virtual void writeNewData() override final;
   virtual QAbstractTableModel* model() override final;
   virtual void finish() override final;
   qint32 size();
   qint32 get(int index);
   void add(quint32 value);
private:
   qint32 _size {0};
}; 
```

This skeleton inherits the required abstract data class from ACE which is the abstract class that defines a data object. The virtual functions that must be implemented from this class are dataEnd(), readData(), writeNewData(), and model(). The finish() function is optional and does not need to be overriden.

The dataEnd() method is called to determine the end of the data within this data object's file stream. The readData() method is called once when a data object file is opened and should read in its data from its data object file stream. The writeNewData() method is called once to initialize a new and empty data object file and should initialize its data object file stream. The model() method is called to get a pointer to a qt abstract table model that represents the data of its data object. The optional finish() method is called once for a new data object file once its analytic is finished running and is about to close.

A data class should have additional methods used by analytics for accessing and setting its data. These methods are not needed for the ACE platform to do its job though, and is left to the data class declaring as regular methods that its fellow analytics should be aware of. In our example data object these methods are size(), get(), and add().

## Protected Methods

To facilitate access to a data object's underlying file and metadata system a number of protected methods is provided for a data class implementation to use.

```
class EAbstractData : public QObject
{
...
protected:
   EMetadata systemMeta() const;
   EMetadata meta() const;
   void setMeta(const EMetadata& newMeta);
   const EDataStream& stream() const;
   EDataStream& stream();
   void seek(qint64 index) const;
   void allocate(qint64 size);
...
};
```

A detailed description of these methods can be read within the code documentation itself.  The systemMeta(), meta(), and setMeta() methods deal with interacting with the metadata system of an ACE data object. The stream() methods deal with the streaming object used for reading/writing to the data portion of a data object file. The seek() and allocate() methods deal with self-explanatory actions that can be done to the data object file.

## Data Interface Methods

Now we will define all interface methods that ACE uses to interact with your data class.

### dataEnd()

This interface informs ACE where in the data object file your data ends. This is used to safely write part of the metadata to the file after your data so nothing is overwritten. Another way to think about the number returned is it is effectively the size of your data. For our example class that is the number of integers it contains plus one for the integer size header.

```
qint64 HelloWorld::dataEnd() const;
{
   return sizeof(qint32)*(1 + _size);
}
```

### readData()

This interface is called when opening an existing data object file. This allows a data class to initialize itself by reading in whatever information it keeps in memory instead of the file. It is assumed the majority of data for a data object is kept in the file because ACE is intended for very large data. In our example class we simply read in the header that informs about the number of integer values the data object holds in the file.

```
void HelloWorld::readData()
{
   seek(0);
   stream() >> _size;
}
```

### writeNewData()

This interface is called when opening a new data object file. This allows a data class to initialize a new data object file to a null state. In our example class we set the header to zero to make it empty with zero integer values contained within in.

```
void HelloWorld::writeNewData()
{
   seek(0);
   stream() << (qint32)0;
}
```

### model()

This interface is called to get a pointer to the abstract table model that represents the data of a data object instance. To understand how to properly use this model please look at Qt documentation, specifically the QAbstractTableModel class. In our example class we simply return a null pointer.

```
QAbstractTableModel* HelloWorld::model()
{
   return nullptr;
}
```

### finish()

This interface is called once with new data object files once the analytic has finished execution. This allows a data class to write any final values to the object file that can only be calculated once the analytic is finished adding data to it. This is an optional interface. In our example class we write the number of integers the new data object contains to its header location within the file.

```
void HelloWorld::finish()
{
   seek(0);
   stream() << _size;
}
```

## Additional Methods

Now we will define the additional methods unique to our specific data class. These methods primary job our to access and write data it is responsible for holding in its data object file. Because ACE is designed for large data, reading and writing data should be done directly with the file object IO.

### get()

This method is used to get an integer value stored by its data object with the given index. In a real world implementation this method should also check for valid index ranges.

```
qint32 HelloWorld::get(int index)
{
   qint32 ret;
   seek(sizeof(qint32)*(1 + index));
   stream() >> ret;
   return ret;
}
```

### add()

This method is used to append new integer values to its data object. Notice that while the new integer value is immediately written to disk, the header size value is not. This is recommended to greatly increase efficiency of file IO. The header information can be written at the end in the finish() interface.

```
void HelloWorld::add(qint32 value)
{
   seek(sizeof(qint32)*(1 + _size);
   stream() << value;
   ++_size;
}
```

## Metadata

The metadata system for data objects is split into two regions.

The first is read only and is called the system metadata. This is generated by the ACE system and cannot be modified. It is accessed with the systemMeta() method. The information contained deals mainly with information about the inputs used to generate the metadata's data object.

The second is read and write and is called the user metadata. If the data object class does nothing with it a new data object will have a blank user metadata. It can be accessed with meta() and written to with setMeta(). The user metadata can only be written to at the very end of a new data object inside the finish() interface. It is entirely up to the data object class to decide what should be put into this metadata. The user can also manipulate this metadata through the GUI interface.

# Analytic Class