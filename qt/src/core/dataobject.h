#ifndef ACE_DATAOBJECT_H
#define ACE_DATAOBJECT_H
#include <QtCore>

#include "abstractdata.h"
#include "datastream.h"
#include "metadata.h"



namespace Ace
{
   class DataObject : public QObject
   {
      Q_OBJECT
   public:
      explicit DataObject(const QString& path);
      ~DataObject() noexcept;
      void open();
      bool seek(quint64 offset) noexcept;
      bool allocate(quint64 size) noexcept;
      void clear(quint16 newType);
      bool isNew() const noexcept;
      quint64 size() const noexcept;
      EAbstractData& data();
      EMetadata& getMeta() { return _metaRoot; }
      void writeMeta();
      quint16 getType() { return _type; }
      QString getPath() { return _path; }
      operator bool() const noexcept;
   signals:
      void cleared();
   private:
      constexpr static int _mininumFileSize {12};
      constexpr static quint64 _specialValue {584};
      std::unique_ptr<QFile> _file {nullptr};
      std::unique_ptr<EDataStream> _stream {nullptr};
      std::unique_ptr<EAbstractData> _data {nullptr};
      quint64 _headerOffset {0};
      bool _isNew {true};
      bool _invalid {false};
      EMetadata _metaRoot;
      quint16 _type {0};
      QString _path;
   };
}



#endif
// 64special 16inttype qstring
