#ifndef ACCELCOMPENG_ACE_H
#define ACCELCOMPENG_ACE_H
#include "analytic.h"
#include "clbuffer.h"
#include "clcommandqueue.h"
#include "clcontext.h"
#include "cldevice.h"
#include "cldevlist.h"
#include "clevent.h"
#include "clkernel.h"
#include "clprogram.h"
#include "console.h"
#include "data.h"
#include "datamap.h"
#include "exception.h"
#include "fstring.h"
#include "getopts.h"
#include "history.h"
#include "file.h"
#include "linuxterm.h"
#include "terminal.h"
#include "factory.h"
namespace AccelCompEng
{



int run(const char*,Factory&,int,char*[]);



}
#endif
