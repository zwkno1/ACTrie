#include <boost/python.hpp>
#include "actrie.h"
//#ifdef __cplusplus
extern "C"
{

BOOST_PYTHON_MODULE(pyactrie)
{
	boost::python::class_<actrie<>, boost::noncopyable>("actrie", boost::python::init<>())
		.def("insert", &actrie<>::insert)
		.def("build", &actrie<>::build)
		.def("process", &actrie<>::py_process)
		;
}

}
