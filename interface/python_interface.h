#ifndef PYTHON_INTERFACE_H
#define PYTHON_INTERFACE_H

#include <string>
#include <pybind11/pybind11.h>

std::string call_execute_algorithm(const std::string &arg1, const std::string &arg2);


#endif // PYTHON_INTERFACE_H
