#ifndef __FIDES_READER_H__
#define __FIDES_READER_H__

#include <adios2.h>
#include <mpi.h>

#include "gray-scott.h"
#include "settings.h"

#include <fides/DataSetReader.h>

#include <memory>

class FidesReader
{
public:
    FidesReader(const Settings &settings, adios2::IO io, int argc, char* argv[]);
    void open(const std::string &fname);
    void read(int step);
    void close();

    void print_settings();

protected:
    const Settings &settings;

    adios2::IO io;
    std::unique_ptr<fides::io::DataSetReader> Reader;
    std::unordered_map<std::string, std::string> FidesPaths;
};

#endif

