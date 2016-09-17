/*
 * See GPL.txt for copyright information.
 *
 * Author: Joshua Burns
 *
 */
#include <sstream>
#include "cldevice.h"
namespace AccelCompEng
{



CLDevice::CLDevice(int pinc, int dinc, cl_platform_id pid, cl_device_id did):
   _pinc(pinc),
   _dinc(dinc),
   _pid(pid),
   _did(did)
{}



cl_platform_id CLDevice::platform()
{
   return _pid;
}



cl_device_id CLDevice::device()
{
   return _did;
}



std::string CLDevice::info(CLInfo which) const
{
   std::ostringstream buffer;
   switch (which)
   {
   case CLInfo::ident:
      buffer << _pinc << ":" << _dinc;
      break;
   case CLInfo::name:
      buffer << get_info<string>(CL_DEVICE_NAME);
      break;
   case CLInfo::pname:
      break;
   case CLInfo::type:
      switch (get_info<cl_device_type>(CL_DEVICE_TYPE))
      {
      case CL_DEVICE_TYPE_CPU:
         buffer << "CPU";
         break;
      case CL_DEVICE_TYPE_GPU:
         buffer << "GPU";
         break;
      case CL_DEVICE_TYPE_ACCELERATOR:
         buffer << "ACCEL";
         break;
      case CL_DEVICE_TYPE_DEFAULT:
         buffer << "DEFAULT";
         break;
      default:
         buffer << "UNKNOWN";
         break;
      }
      break;
   case CLInfo::online:
      yes_no(buffer,get_info<cl_bool>(CL_DEVICE_AVAILABLE)&&
                    get_info<cl_bool>(CL_DEVICE_COMPILER_AVAILABLE));
      break;
   case CLInfo::unified_mem:
      yes_no(buffer,get_info<cl_bool>(CL_DEVICE_HOST_UNIFIED_MEMORY));
      break;
   case CLInfo::addr_space:
      buffer << get_info<cl_uint>(CL_DEVICE_ADDRESS_BITS);
      break;
   case CLInfo::clock:
      buffer << get_info<cl_uint>(CL_DEVICE_MAX_CLOCK_FREQUENCY);
      break;
   case CLInfo::compute_units:
      buffer << get_info<cl_uint>(CL_DEVICE_MAX_COMPUTE_UNITS);
      break;
   case CLInfo::work_size:
      buffer << get_info<size_t>(CL_DEVICE_MAX_WORK_GROUP_SIZE);
      break;
   case CLInfo::global_mem:
      size_it(buffer,get_info<cl_ulong>(CL_DEVICE_GLOBAL_MEM_SIZE));
      break;
   case CLInfo::local_mem:
      size_it(buffer,get_info<cl_ulong>(CL_DEVICE_LOCAL_MEM_SIZE));
      break;
   case CLInfo::extensions:
      buffer << get_info<string>(CL_DEVICE_EXTENSIONS);
      break;
   }
   return buffer.str();
}



bool CLDevice::operator==(const CLDevice& cmp)
{
   return ( _pid == cmp._pid && _did == cmp._did );
}



/// @brief Converts boolean value into string representation.
///
/// Takes boolean value and outputs textual yes if boolean is true and no if
/// boolean is false.
///
/// @param buffer Where textual yes/no will be written to.
/// @param test Value that will be read.
void CLDevice::yes_no(std::ostringstream& buffer, bool test) const
{
   const char* ans[] = {"yes","no"};
   buffer << ans[(test ? 0 : 1)];
}



/// @brief Converts integer to memory size.
///
/// Takes integer value as bytes and outputs formatted output to buffer with the
/// appropriate label. Maximum size is 1024 terrabytes.
///
/// @param buffer Where formatted output is written.
/// @param size Sizein bytes to be reported.
void CLDevice::size_it(std::ostringstream& buffer, long size) const
{
   constexpr double kilobit = 1024.0f;
   constexpr int max = 4;
   const char* sizes[] = {"B","KB","MB","GB","TB"};
   int count = 0;
   double fsize = size;
   // Keep dividing size until it is less than 1024, keeping track of how many dividions occured.
   while ( fsize > kilobit && count < max )
   {
      fsize /= kilobit;
      count++;
   }
   // Write number with specific precision along with size prefix.
   buffer.setf(std::ios::fixed,std::ios::floatfield);
   buffer.precision(_fPrecision);
   buffer << fsize << sizes[count];
   buffer.unsetf(std::ios::floatfield);
}



}
