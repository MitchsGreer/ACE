#include "eapplication.h"
#include <QTextStream>
#include "../core/ace_settings.h"
#include "../core/ace_dataobject.h"
#include "../core/eabstractdatafactory.h"
#include "../core/eabstractanalyticfactory.h"
#include "../core/eexception.h"
#include "../core/edebug.h"
#include "../core/ace_logserver.h"
#include "../core/ace_qmpi.h"
#include "ace_run.h"
#include "ace_settingsrun.h"



//






/*!
 * Constructs a new application object, initializing the ACE system with the given 
 * organization, application, version numbers, data factory, and analytic factory. 
 * This also parses the command line arguments with the given command line 
 * arguments. 
 *
 * @param organization The organization name of the application using ACE. 
 *
 * @param application The name of the application using ACE. 
 *
 * @param majorVersion The major version of the application using ACE. 
 *
 * @param minorVersion The minor version of the application using ACE. 
 *
 * @param revision The revision of the application using ACE. 
 *
 * @param data The data factory of the application using ACE. 
 *
 * @param analytic The analytic factory of the application using ACE. 
 *
 * @param argc The command line argument size passed from the main function. 
 *
 * @param argv The command line arguments passed from the main function. 
 */
EApplication::EApplication(const QString& organization, const QString& application, int majorVersion, int minorVersion, int revision, std::unique_ptr<EAbstractDataFactory>&& data, std::unique_ptr<EAbstractAnalyticFactory>&& analytic, int& argc, char** argv):
   QCoreApplication(argc,argv),
   _options(argc,argv),
   _command(argc,argv)
{
   try
   {
      // Initialize the ACE system through the global settings instance and set the data 
      // and analytic factories to the ones given. 
      Ace::Settings::initialize(organization,application,majorVersion,minorVersion,revision);
      EAbstractDataFactory::setInstance(std::move(data));
      EAbstractAnalyticFactory::setInstance(std::move(analytic));
   }

   // If any exception is caught then report it to the user and forcefully exit the 
   // application. 
   catch (EException e)
   {
      showException(e);
      ::exit(-1);
   }
   catch (std::exception e)
   {
      qDebug() << tr("STD exception %1 caught!\n").arg(e.what());
      ::exit(-1);
   }
   catch (...)
   {
      qDebug() << tr("Unknown exception caught!\n");
      ::exit(-1);
   }
}






/*!
 * Implements the qt interface that handles all events being passed for handling. 
 * This implementation simply added an exception handler to qt so it can support 
 * exceptions being thrown. 
 *
 * @param receiver The qt object that is receiving this event. 
 *
 * @param event The event itself. 
 *
 * @return True if the event has been handled or false otherwise. 
 */
bool EApplication::notify(QObject* receiver, QEvent* event)
{
   // Call the qt implementation of the notify interface. If any exception is thrown 
   // then report the exception to the command line and forcefully exit the program. 
   try
   {
      return QCoreApplication::notify(receiver,event);
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
   std::exit(-1);
   return false;
}






/*!
 * Executes the console command based off the command line arguments given to this 
 * program, returning when ACE and this program has finished its execution and is 
 * ready to quit. 
 *
 * @return The code that should be returned in the main function. 
 */
int EApplication::exec()
{
   EDEBUG_FUNC(this)

   // For all following steps if an exception is thrown then report it to the command 
   // line and exit with a failure code. 
   enum {Unknown = -1,Settings,Run,ChunkRun,Merge,Dump,Inject};
   try
   {

      // If the command argument size is empty then throw an exception, else go to the 
      // next step. 
      if ( _command.size() < 1 )
      {
         E_MAKE_EXCEPTION(e);
         e.setTitle(tr("No Arguments"));
         e.setDetails(tr("No arguments given, exiting..."));
         throw e;
      }

      // Determine which primary command is given. If the command is settings then 
      // create a new settings run object, passing it the commands and calling its exec 
      // method. If the command is run, chunk run, or merge create a new run instance, 
      // passing it the commands and options and connecting its destroyed signal to this 
      // application's quit slot and calling this qt application's exec method for event 
      // processing. If the command is dump or inject then call this object's 
      // appropriate method to handle it. All commands excluding run, chunk run, or 
      // merge will have the first command popped from the list of command arguments. If 
      // the command is not recognized then throw an exception. 
      QStringList commands {"settings","run","chunkrun","merge","dump","inject"};
      switch (_command.peek(commands))
      {
      case Settings:
         {
            _command.pop();
            Ace::SettingsRun settings(_command);
            settings.execute();
            break;
         }
      case Run:
      case ChunkRun:
      case Merge:
         {
            // Initialize the run object. 
            Ace::Run* run {new Ace::Run(_command,_options)};
            connect(run,&Ace::Run::destroyed,this,&QCoreApplication::quit);

            // Check to see if logging is enabled. 
            if ( Ace::Settings::instance().loggingEnabled() )
            {
               // Get the logging port setting and initialize the log server. 
               QTextStream out(stdout);
               int port {Ace::Settings::instance().loggingPort() + Ace::QMPI::instance().localRank()};
               Ace::LogServer::initialize(port);

               // Inform the user that the application is waiting for an ACE log client to signal 
               // starting. 
               out << tr("Log server listening on %1:%2, waiting for connection...\n").arg(Ace::LogServer::host()).arg(port);
               out.flush();
               Ace::LogServer::log()->wait();
            }

            // Start the analytic run. 
            return QCoreApplication::exec();
         }
      case Dump:
         _command.pop();
         dump();
         return 0;
      case Inject:
         _command.pop();
         inject();
         return 0;
      case Unknown:
         {
            E_MAKE_EXCEPTION(e);
            e.setTitle(tr("Invalid argument"));
            e.setDetails(tr("Unknown command '%1'.").arg(commands.first()));
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
 * Gets JSON formatted text from the file with the given path. If the given file is 
 * not valid JSON or opening the file for reading fails then an exception is 
 * thrown. 
 *
 * @param path The path of the text file which is read in and returned as JSON. 
 *
 * @return The JSON of the text file with the given path. 
 */
QJsonDocument EApplication::getJson(const QString& path)
{
   EDEBUG_FUNC(path)

   // Open the text file at the given path and read in the JSON, returning the JSON 
   // document. If opening the file fails or reading it in as JSON fails then throw 
   // an exception. 
   QFile file(path);
   if ( !file.open(QIODevice::ReadOnly) )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("System Error"));
      e.setDetails(tr("Failed opening JSON file %1: %2").arg(path).arg(file.errorString()));
      throw e;
   }
   QJsonDocument ret {QJsonDocument::fromJson(file.readAll())};
   if ( !ret.isObject() )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Read Error"));
      e.setDetails(tr("Failed reading in JSON file %1: It must be a JSON object.").arg(path));
      throw e;
   }
   return ret;
}






/*!
 * Injects the given JSON document into the given data object's user metadata, 
 * converting the JSON to metadata. It is assumed the given JSON is an object else 
 * the behavior of this method is undefined. 
 *
 * @param path The path to the data object file that has metadata injected into it. 
 *
 * @param document The JSON document that is converted to metadata and injected 
 *                 into the given data object. 
 */
void EApplication::inject(const QString& path, const QJsonDocument& document)
{
   EDEBUG_FUNC(path,&document)

   // Open the data object from the given path and set its user metadata to the given 
   // JSON, converting it to ACE metadata. 
   Ace::DataObject data(path);
   data.setUserMeta(EMetadata(document.object()));
}






/*!
 * Prints out the title and details of an ACE exception to standard output. If 
 * debugging is enabled it also prints out the file, line number, and function 
 * where the exception was thrown. 
 *
 * @param exception  
 */
void EApplication::showException(const EException& exception)
{
   // If debugging is enabled print out the file name, line number, and function 
   // where the given exception was thrown. 
   QTextStream stream(stdout);
   qDebug().noquote().nospace() << exception.fileName() << ":" << exception.line();
   qDebug().noquote() << exception.functionName();

   // Print out the title, in all caps, and details of the given exception. 
   stream << exception.title().toUpper() << "\n" << exception.details() << "\n";
}






/*!
 * Executes the dump command, dumping the system or user metadata, depending on 
 * which one is specified to dump, of a given data object to standard output. 
 */
void EApplication::dump()
{
   EDEBUG_FUNC(this)

   // If the command argument size is less than one then throw an exception, else go 
   // to the next step. 
   enum {Unknown=-1,System,User};
   if ( _command.size() < 2 )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Invalid Argument"));
      e.setDetails(tr("Missing required arguments for dump command, exiting..."));
      throw e;
   }

   // Open the data object file located at the path given by the first command 
   // argument, dumping its system or user metadata as JSON to standard output, 
   // depending on whether the system or user command is given as the second command 
   // argument respectively. If the command given is not recognized then throw an 
   // exception. 
   QTextStream stream(stdout);
   Ace::DataObject data(_command.pop());
   QJsonDocument document;
   QStringList commands {"system","user"};
   switch (_command.peek(commands))
   {
   case System:
      document.setObject(data.systemMeta().toJson().toObject());
      break;
   case User:
      document.setObject(data.userMeta().toJson().toObject());
      break;
   case Unknown:
      {
         E_MAKE_EXCEPTION(e);
         e.setTitle(tr("Invalid Argument"));
         e.setDetails(tr("Unknown dump command '%1', exiting...").arg(_command.first()));
         throw e;
      }
   }
   stream << document.toJson();
}






/*!
 * Executes the inject command, injecting the JSON loaded from a given path and 
 * overwriting a given data object's user metadata with it. 
 */
void EApplication::inject()
{
   EDEBUG_FUNC(this)

   // If the command argument size is less than 2 then throw an exception, else go to 
   // the next step. 
   if ( _command.size() < 2 )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Invalid Argument"));
      e.setDetails(tr("Missing required arguments for inject command, exiting..."));
      throw e;
   }

   // Load the JSON from the path given by the second command argument and inject it 
   // into a data object whose path is given by the first command argument. 
   inject(_command.at(0),getJson(_command.at(1)));
}
