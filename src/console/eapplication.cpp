#include "eapplication.h"
#include <QTextStream>
#include <core/eexception.h>
#include <core/ace_dataobject.h>
#include "ace_run.h"
//






/*!
 *
 * @param argc  
 *
 * @param argv  
 */
EApplication::EApplication(int& argc, char** argv):
   QCoreApplication(argc,argv),
   _options(argc,argv),
   _command(argc,argv)
{}






/*!
 */
int EApplication::exec()
{
   try
   {
      enum {Unknown = -1,Run,Dump,Inject};
      if ( _command.size() < 1 )
      {
         E_MAKE_EXCEPTION(e);
         e.setTitle(tr("No Arguments"));
         e.setDetails(tr("No arguments given, exiting..."));
         throw e;
      }
      QStringList commands {"run","dump","inject"};
      QString command {_command.first()};
      switch (_command.pop(commands))
      {
      case Run:
         {
            Ace::Run* run {new Ace::Run(_command,_options)};
            connect(run,&Ace::Run::destroyed,this,&QCoreApplication::quit);
            return QCoreApplication::exec();
         }
      case Dump:
         dump();
         return 0;
      case Inject:
         inject();
         return 0;
      case Unknown:
         {
            E_MAKE_EXCEPTION(e);
            e.setTitle(tr("Invalid argument"));
            e.setDetails(tr("Unknown command '%1'.").arg(command));
            throw e;
         }
      }
   }
   catch (EException e)
   {
      showException(e);
   }
   catch (std::exception e)
   {
      qDebug() << tr("STD exception %1 caught!\n").arg(e.what());
   }
   catch (...)
   {
      qDebug() << tr("Unknown exception caught!\n");
   }
   return -1;
}






/*!
 */
void EApplication::dump()
{
   if ( _command.size() < 1 )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Invalid Argument"));
      e.setDetails(tr("No file path given for data object to dump, exiting..."));
      throw e;
   }
   QTextStream stream(stdout);
   Ace::DataObject data(_command.first());
   QJsonDocument system(data.systemMeta().toJson().toObject());
   QJsonDocument user(data.userMeta().toJson().toObject());
   stream << "SYSTEM METADATA\n" << system.toJson() << "USER METADATA\n" << user.toJson();
}






/*!
 */
void EApplication::inject()
{
   if ( _command.size() < 2 )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Invalid Argument"));
      e.setDetails(tr("Missing required arguments for inject command, exiting..."));
      throw e;
   }
   inject(getJson());
}






/*!
 */
QJsonDocument EApplication::getJson()
{
   QFile jsonFile(_command.at(1));
   if ( !jsonFile.open(QIODevice::ReadOnly) )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("System Error"));
      e.setDetails(tr("Failed opening JSON file %1: %2")
                   .arg(_command.at(1))
                   .arg(jsonFile.errorString()));
      throw e;
   }
   return QJsonDocument::fromJson(jsonFile.readAll());
}






/*!
 *
 * @param document  
 */
void EApplication::inject(const QJsonDocument& document)
{
   if ( !document.isObject() )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Read Error"));
      e.setDetails(tr("Failed reading in JSON file %1: It must be a JSON object.")
                   .arg(_command.at(1)));
      throw e;
   }
   Ace::DataObject data(_command.at(0));
   data.setUserMeta(EMetadata(document.object()));
}






/*!
 *
 * @param exception  
 */
void EApplication::showException(const EException& exception)
{
   QTextStream stream(stdout);
   qDebug().noquote().nospace() << exception.fileName() << ":" << exception.line();
   qDebug().noquote() << exception.functionName();
   stream << exception.title().toUpper() << "\n" << exception.details() << "\n";
}
