#ifndef __READER_H__
#define __READER_H__

#include <adios2.h>
#include <mpi.h>

#include "gray-scott.h"

class Reader
{
public:
    Reader(adios2::IO io);
    void open(const std::string &fname);
    void read();
    void close();

    void print_settings();

protected:
    adios2::IO io;
    adios2::Engine reader;
    adios2::Variable<double> var_u;
    adios2::Variable<double> var_v;
    adios2::Variable<int> var_step;
};

#endif
