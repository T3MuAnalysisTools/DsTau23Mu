#include <boost/python.hpp>
#include "DsTau23Mu/T3MNtuple/interface/DataMCType.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(class_ext)
{
   class_<DataMCType>("DataMCType")
   .def("GetType", static_cast<unsigned int(DataMCType::*)(TString)>(&DataMCType::GetType))
   .def("isSignalParticle", static_cast<bool(DataMCType::*)(int)> (&DataMCType::isSignalParticle))
   ;
}
