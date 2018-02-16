#include <core/AceCore.h>

#ifdef ACE_GUI
   #include <ace/gui/Application.h>
#else
   #include <ace/console/Application.h>
#endif

#include "analyticfactory.h"
#include "datafactory.h"



using namespace std;






int main(int argc, char *argv[])
{
   // Make a new analytic factory
   unique_ptr<AnalyticFactory> analyticFactory(new AnalyticFactory);

   // Make a new data factory
   unique_ptr<DataFactory> dataFactory(new DataFactory);

   // Set ACE's global analytic factory to the one created
   EAbstractAnalyticFactory::setInstance(move(analyticFactory));

   // Set ACE's global data factory to the one created
   EAbstractDataFactory::setInstance(move(dataFactory));

   // Start ACE application and execute it
   EApplication application(argc,argv,"Example ACE Program","example");
   return application.exec();
}