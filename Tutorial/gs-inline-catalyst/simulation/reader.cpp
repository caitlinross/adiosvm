#include "reader.h"

#include <iostream>

Reader::Reader(adios2::IO io)
: io(io)
{
    var_u = io.InquireVariable<double>("U");

    var_v = io.InquireVariable<double>("V");

    var_step = io.InquireVariable<int>("step");

    // TODO catalyst init
}

void Reader::open(const std::string &fname)
{
    reader = io.Open(fname + "-read", adios2::Mode::Read);
}

void Reader::read()
{
    reader.BeginStep();
    // Get calls to data
    double* u_out, *v_out;
    reader.Get(var_u, &u_out);
    reader.Get(var_v, &v_out);

    // TODO catalyst execute call
    reader.EndStep();
}

void Reader::close()
{
  reader.Close();
  // TODO catalyst finalize
}

void Reader::print_settings()
{
    std::cout << "Simulation reads data using engine type:              "
              << io.EngineType() << std::endl;
}
