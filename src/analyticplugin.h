#ifndef ANALYTICPLUGIN_H
#define ANALYTICPLUGIN_H
#include "analytic.h"
#include "clcontext.h"
namespace AccelCompEng
{



class AnalyticPlugin : public Analytic, public CLContext
{
public:
   void execute(GetOpts& ops, Terminal& tm);
};



}
#endif
