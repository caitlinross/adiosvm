#ifndef __READER_H__
#define __READER_H__

#include <adios2.h>
#include <mpi.h>

#include "gray-scott.h"
#include "settings.h"

#ifdef USE_FIDES
#include <fides/DataSetReader.h>
#endif

#include <memory>

class Reader
{
public:
    Reader(const Settings &settings, adios2::IO io, int argc, char* argv[]);
    void open(const std::string &fname);
    void read(int step);
    void close();

    void print_settings();

protected:
    const Settings &settings;

    adios2::IO io;
#if USE_FIDES
    std::unique_ptr<fides::io::DataSetReader> FidesReader;
    std::unordered_map<std::string, std::string> FidesPaths;
#else
    adios2::Engine reader;
    adios2::Variable<double> var_u;
    adios2::Variable<double> var_v;
    adios2::Variable<int> var_step;
#endif
};

#endif
