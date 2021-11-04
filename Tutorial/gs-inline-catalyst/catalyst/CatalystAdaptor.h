#ifndef CatalystAdaptor_h
#define CatalystAdaptor_h

#include <catalyst.hpp>

#include <iostream>
#include <string>

#include "settings.h"

namespace CatalystAdaptor
{

/**
 * In this example, we show how we can use Catalysts's C++
 * wrapper around conduit's C API to create Conduit nodes.
 * This is not required. A C++ adaptor can just as
 * conveniently use the Conduit C API to setup the
 * `conduit_node`. However, this example shows that one can
 * indeed use Catalyst's C++ API, if the developer so chooses.
 */
void Initialize(int argc, char* argv[])
{
  conduit_cpp::Node node;
  for (int cc = 1; cc < argc; ++cc)
  {
    if (strcmp(argv[cc], "--script") == 0 && (cc + 1) < argc)
    {
      node["catalyst/scripts/script" + std::to_string(cc - 1)].set_string(argv[cc + 1]);
      ++cc;
    }
    else if (strcmp(argv[cc], "--output") == 0 && (cc + 1) < argc)
    {
      node["catalyst/pipelines/0/type"].set("io");
      node["catalyst/pipelines/0/filename"].set(argv[cc + 1]);
      node["catalyst/pipelines/0/channel"].set("grid");
      ++cc;
    }

  }
  node["catalyst_load/implementation"] = "paraview";
  node["catalyst_load/search_paths/paraview"] = PARAVIEW_IMPL_DIR;
  catalyst_status err = catalyst_initialize(conduit_cpp::c_node(&node));
  if (err != catalyst_status_ok)
  {
    std::cerr << "Failed to initialize Catalyst: " << err << std::endl;
  }
}

void Execute(int step, double time, const Settings &settings, double *u, int size_u, double *v, int size_v)
{
  conduit_cpp::Node exec_params;

  // add time/step information
  auto state = exec_params["catalyst/state"];
  state["timestep"].set(step);
  state["time"].set(time);

  // Add channels.
  // We only have 1 channel here. Let's name it 'grid'.
  auto channel = exec_params["catalyst/channels/grid"];

  // Since this example is using Conduit Mesh Blueprint to define the mesh,
  // we set the channel's type to "mesh".
  channel["type"].set("mesh");

  // now create the mesh.
  auto mesh = channel["data"];

  // start with coordsets (of course, the sequence is not important, just make
  // it easier to think in this order).
  mesh["coordsets/coords/type"].set("uniform");

  // TODO is this supposed to be global or local dims?
  mesh["coordsets/coords/dims/i"].set(settings.L);
  mesh["coordsets/coords/dims/j"].set(settings.L);
  mesh["coordsets/coords/dims/k"].set(settings.L);

  // TODO correctly figure out local origin for each rank
  double localOrigin[3] = {0.0, 0.0, 0.0};
  mesh["coordsets/coords/origin/x"].set(localOrigin[0]);
  mesh["coordsets/coords/origin/y"].set(localOrigin[1]);
  mesh["coordsets/coords/origin/z"].set(localOrigin[2]);

  mesh["coordsets/coords/spacing/x"].set(0.1);
  mesh["coordsets/coords/spacing/y"].set(0.1);
  mesh["coordsets/coords/spacing/z"].set(0.1);

  // Next, add topology
  mesh["topologies/mesh/type"].set("uniform");
  mesh["topologies/mesh/coordset"].set("coords");

  // Finally, add fields.
  auto fields = mesh["fields"];
  fields["u/association"].set("vertex");
  fields["u/topology"].set("mesh");
  fields["u/volume_dependent"].set("false");
  fields["u/values"].set_external(u, size_u);

  fields["v/association"].set("vertex");
  fields["v/topology"].set("mesh");
  fields["v/volume_dependent"].set("false");
  fields["v/values"].set_external(v, size_v);

  catalyst_status err = catalyst_execute(conduit_cpp::c_node(&exec_params));
  if (err != catalyst_status_ok)
  {
    std::cerr << "Failed to execute Catalyst: " << err << std::endl;
  }
}

void Finalize()
{
  conduit_cpp::Node node;
  catalyst_status err = catalyst_finalize(conduit_cpp::c_node(&node));
  if (err != catalyst_status_ok)
  {
    std::cerr << "Failed to finalize Catalyst: " << err << std::endl;
  }
}
}

#endif
