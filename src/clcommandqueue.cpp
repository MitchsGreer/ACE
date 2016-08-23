#include "clcommandqueue.h"
namespace AccelCompEng
{



CLCommandQueue::~CLCommandQueue()
{
   if (_initd)
   {
      clReleaseCommandQueue(_id);
   }
}



void CLCommandQueue::init(cl_context cid, cl_device_id did)
{
   static const char* failInit = "Cannot create OpenCL command queue";
   cl_int err;
   _id = clCreateCommandQueue(cid,did,0x0,&err);
   classert<CannotCreate>(err,__LINE__,failInit);
   _initd = true;
}



CLEvent CLCommandQueue::add_task(CLKernel& kernel)
{
   static const char* notInit = "OpenCL command queue not initialized";
   static const char* nullKern = "Cannot use OpenCL kernel that is null";
   static const char* failEnqueue = "Failed adding OpenCL kernel to command queue";
   assert<NotInitialized>(_initd,__LINE__,notInit);
   assert<NullKernelUsed>(kernel._isAlive,__LINE__,nullKern);
   cl_event ret;
   cl_int err;
   err = clEnqueueTask(_id,kernel._id,0,NULL,&ret);
   classert<CannotAddTask>(err,__LINE__,failEnqueue);
   return CLEvent(ret);
}



CLEvent CLCommandQueue::add_swarm(CLKernel& kernel)
{
   static const char* notInit = "OpenCL command queue not initialized";
   static const char* nullKern = "Cannot use OpenCL kernel that is null";
   static const char* failEnqueue = "Failed adding OpenCL kernel swarm to command queue";
   assert<NotInitialized>(_initd,__LINE__,notInit);
   assert<NullKernelUsed>(kernel._isAlive,__LINE__,nullKern);
   cl_event ret;
   const size_t offsets[kernel._dims] = {0};
   cl_int err;
   err = clEnqueueNDRangeKernel(_id,kernel._id,kernel._dims,offsets,
                                kernel._gSizes,kernel._lSizes,
                                0,NULL,&ret);
   classert<CannotAddSwarm>(err,__LINE__,failEnqueue);
   return CLEvent(ret);
}



}
