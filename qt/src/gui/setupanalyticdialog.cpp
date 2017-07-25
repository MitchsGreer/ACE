#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSignalMapper>
#include <QFormLayout>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QSpinBox>

#include "setupanalyticdialog.h"
#include "abstractanalytic.h"
#include "abstractdatafactory.h"
#include "exception.h"






Ace::SetupAnalyticDialog::SetupAnalyticDialog(EAbstractAnalytic* analytic, QWidget* parent):
   QDialog(parent),
   _analytic(analytic)
{
   _cancelButton = new QPushButton(tr("&Cancel"),this);
   _executeButton = new QPushButton(tr("&Execute"),this);
   connect(_cancelButton,SIGNAL(clicked(bool)),this,SLOT(cancel()));
   connect(_executeButton,SIGNAL(clicked(bool)),this,SLOT(executeButton()));
   QVBoxLayout* layout = new QVBoxLayout;
   layout->addLayout(createInputs());
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(_executeButton);
   buttonsLayout->addStretch();
   buttonsLayout->addWidget(_cancelButton);
   layout->addLayout(buttonsLayout);
   setLayout(layout);
}






void Ace::SetupAnalyticDialog::findFile(int argument)
{
   using Type = EAbstractAnalytic::ArgumentType;
   using Role = EAbstractAnalytic::Role;
   QFileDialog dialog(nullptr,tr("Select File"));
   EAbstractDataFactory& factory = EAbstractDataFactory::getInstance();
   switch (_analytic->getArgumentData(argument))
   {
   case Type::FileIn:
   case Type::DataIn:
      dialog.setAcceptMode(QFileDialog::AcceptOpen);
      break;
   case Type::FileOut:
   case Type::DataOut:
      dialog.setAcceptMode(QFileDialog::AcceptSave);
      break;
   default:
      return;
   }
   switch (_analytic->getArgumentData(argument))
   {
   case Type::FileIn:
   case Type::FileOut:
      dialog.setNameFilters(_analytic->getArgumentData(argument,Role::FileFilters).toStringList());
      break;
   case Type::DataIn:
   case Type::DataOut:
   {
      quint16 type = _analytic->getArgumentData(argument,Role::DataType).toUInt();
      if ( type >= factory.getCount() )
      {
         E_MAKE_EXCEPTION(e);
         e.setLevel(EException::Critical);
         e.setTitle(tr("Setup Analytic Dialog"));
         e.setDetails(tr("Analytic requesting data type %1 that does not exist(%2 is max).")
                      .arg(type).arg(factory.getCount()-1));
         throw e;
      }
      QString filter = tr("%1 data object (*.%2)").arg(factory.getName(type))
            .arg(factory.getFileExtension(type));
      dialog.setNameFilter(filter);
      break;
   }
   default:
      return;
   }
   if ( dialog.exec() )
   {
      QDir pwd(QDir::currentPath());
      QStringList files = dialog.selectedFiles();
      QLineEdit* edit = qobject_cast<QLineEdit*>(_inputs.at(argument));
      edit->setText(pwd.relativeFilePath(files.at(0)));
   }
}






void Ace::SetupAnalyticDialog::cancel()
{
   reject();
}






void Ace::SetupAnalyticDialog::executeButton()
{
   using Type = EAbstractAnalytic::ArgumentType;
   //using Role = EAbstractAnalytic::Role;
   for (int i = 0; i < _analytic->getArgumentCount() ;++i)
   {
      switch (_analytic->getArgumentData(i))
      {
      case Type::Bool:
      {
         QCheckBox* edit = qobject_cast<QCheckBox*>(_inputs.at(i));
         _analytic->setArgument(i,QVariant(edit->isChecked()));
         break;
      }
      case Type::Integer:
      {
         QSpinBox* edit = qobject_cast<QSpinBox*>(_inputs.at(i));
         _analytic->setArgument(i,QVariant(edit->value()));
         break;
      }
      case Type::Double:
      {
         QLineEdit* edit = qobject_cast<QLineEdit*>(_inputs.at(i));
         _analytic->setArgument(i,QVariant(edit->text().toDouble()));
         break;
      }
      case Type::String:
      {
         QLineEdit* edit = qobject_cast<QLineEdit*>(_inputs.at(i));
         _analytic->setArgument(i,QVariant(edit->text()));
         break;
      }
      case Type::Combo:
      {
         QComboBox* edit = qobject_cast<QComboBox*>(_inputs.at(i));
         _analytic->setArgument(i,QVariant(edit->currentText()));
         break;
      }
      case Type::FileIn:
      case Type::FileOut:
      case Type::DataIn:
      case Type::DataOut:
         break;
      }
   }
   accept();
}






QFormLayout* Ace::SetupAnalyticDialog::createInputs()
{
   using Type = EAbstractAnalytic::ArgumentType;
   using Role = EAbstractAnalytic::Role;
   QSignalMapper* mapper = new QSignalMapper(this);
   connect(mapper,SIGNAL(mapped(int)),this,SLOT(findFile(int)));
   QFormLayout* formLayout = new QFormLayout;
   for (int i = 0; i < _analytic->getArgumentCount() ;++i)
   {
      QLabel* label = new QLabel(_analytic->getArgumentData(i,Role::Title).toString());
      label->setWhatsThis(_analytic->getArgumentData(i,Role::WhatsThis).toString());
      QWidget* field;
      switch (_analytic->getArgumentData(i))
      {
      case Type::Bool:
      {
         QCheckBox* edit = new QCheckBox;
         edit->setChecked(_analytic->getArgumentData(i,Role::DefaultValue).toBool());
         _inputs.append(edit);
         field = edit;
         break;
      }
      break;
      case Type::Integer:
         {
            QSpinBox* edit = new QSpinBox;
            edit->setValue(_analytic->getArgumentData(i,Role::DefaultValue).toInt());
            edit->setMinimum(_analytic->getArgumentData(i,Role::Minimum).toInt());
            edit->setMaximum(_analytic->getArgumentData(i,Role::Minimum).toInt());
            _inputs.append(edit);
            field = edit;
         }
         break;
      case Type::Double:
         {
            QLineEdit* edit = new QLineEdit;
            edit->setText(QString::number(
                             _analytic->getArgumentData(i,Role::DefaultValue).toDouble()));
            double bottom {_analytic->getArgumentData(i,Role::Minimum).toDouble()};
            double top {_analytic->getArgumentData(i,Role::Minimum).toDouble()};
            int decimals {_analytic->getArgumentData(i,Role::Decimals).toInt()};
            edit->setValidator(new QDoubleValidator(bottom,top,decimals,this));
            _inputs.append(edit);
            field = edit;
         }
         break;
      case Type::String:
      {
         QLineEdit* edit = new QLineEdit;
         edit->setText(_analytic->getArgumentData(i,Role::DefaultValue).toString());
         _inputs.append(edit);
         field = edit;
         break;
      }
      case Type::Combo:
      {
         QComboBox* combo = new QComboBox;
         QStringList options = _analytic->getArgumentData(i,Role::ComboValues).toStringList();
         for (auto i = options.constBegin(); i != options.constEnd() ;++i)
         {
            combo->addItem(*i);
         }
         _inputs.append(combo);
         field = combo;
         break;
      }
      case Type::FileIn:
      case Type::FileOut:
      case Type::DataIn:
      case Type::DataOut:
      {
         field = new QWidget;
         QHBoxLayout* fieldLayout = new QHBoxLayout;
         QLineEdit* edit = new QLineEdit;
         edit->setReadOnly(true);
         _inputs.append(edit);
         fieldLayout->addWidget(_inputs.back());
         QPushButton* findFileButton = new QPushButton(tr("Browse"));
         fieldLayout->addWidget(findFileButton);
         connect(findFileButton,SIGNAL(clicked(bool)),mapper,SLOT(map()));
         field->setLayout(fieldLayout);
         mapper->setMapping(findFileButton,i);
         break;
      }
      }
      formLayout->addRow(label,field);
   }
   return formLayout;
}
