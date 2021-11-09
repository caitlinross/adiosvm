#include "reader.h"

#ifdef USE_CATALYST
#include "CatalystAdaptor.h"
#endif

#include <iostream>

Reader::Reader(const Settings &settings, adios2::IO io, int argc, char* argv[])
: settings(settings), io(io)
{
    var_u = io.InquireVariable<double>("U");

    var_v = io.InquireVariable<double>("V");

    var_step = io.InquireVariable<int>("step");

#ifdef USE_CATALYST
    CatalystAdaptor::Initialize(argc, argv);
#endif
}

void Reader::open(const std::string &fname)
{
    reader = io.Open(fname + "-read", adios2::Mode::Read);
}

void Reader::read(int step)
{
    reader.BeginStep();
    // Get calls to data
    double* u_out, *v_out;
    reader.Get(var_u, &u_out);
    reader.Get(var_v, &v_out);

#ifdef USE_CATALYST
    CatalystAdaptor::Execute(reader.CurrentStep(),
        step, settings,
        u_out, var_u.SelectionSize(),
        v_out, var_v.SelectionSize());
#endif

    reader.EndStep();
}

void Reader::close()
{
  reader.Close();
#ifdef USE_CATALYST
    CatalystAdaptor::Finalize();
#endif
}

void Reader::print_settings()
{
    std::cout << "Simulation reads data using engine type:              "
              << io.EngineType() << std::endl;
}
