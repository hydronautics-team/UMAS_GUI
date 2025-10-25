#include "python_interface.h"
#include <pybind11/embed.h>

namespace py = pybind11;
using namespace py::literals;

std::string call_execute_algorithm(const std::string &arg1, const std::string &arg2) {
    py::scoped_interpreter guard{};
    try {
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("append")(".");

        py::module execute_algorithm = py::module::import("execute_algorithm");
        py::object result = execute_algorithm.attr("execute_algorithm")(arg1, arg2);
        return result.cast<std::string>();
    } catch (const std::exception &e) {
        return std::string("Error occurred: ") + e.what();
    }
}
