#include "reader.h"

#ifdef USE_CATALYST
#include "CatalystAdaptor.h"
#endif

#ifdef USE_FIDES
#include "fides/DataSetReader.h"
#include <vtkm/rendering/Camera.h>
#include <vtkm/rendering/CanvasRayTracer.h>
#include <vtkm/rendering/MapperRayTracer.h>
#include <vtkm/rendering/Scene.h>
#include <vtkm/rendering/View3D.h>
#endif

#include <iostream>

Reader::Reader(const Settings &settings, adios2::IO io, int argc, char* argv[])
: settings(settings), io(io)
{
#ifdef USE_FIDES
    std::string jsonConfig = std::string(argv[2]);
    FidesReader.reset(new fides::io::DataSetReader(jsonConfig));
    fides::DataSourceParams params;
    params["engine_type"] = "Inline";
    params["writer_id"] = settings.output;
    FidesReader->SetDataSourceParameters("the-source", std::move(params));
    FidesReader->SetDataSourceIO("the-source", &this->io);

#else

    var_u = io.InquireVariable<double>("U");

    var_v = io.InquireVariable<double>("V");

    var_step = io.InquireVariable<int>("step");

#ifdef USE_CATALYST
    CatalystAdaptor::Initialize(argc, argv);
#endif

#endif
}

void Reader::open(const std::string &fname)
{
#ifdef USE_FIDES
    FidesPaths["the-source"] = fname;
#else
    reader = io.Open(fname + "-read", adios2::Mode::Read);
#endif
}

void Reader::read(int step)
{
#ifdef USE_FIDES
    FidesReader->PrepareNextStep(FidesPaths);
    auto metaData = FidesReader->ReadMetaData(FidesPaths);
    fides::metadata::MetaData selections;

    vtkm::cont::PartitionedDataSet outputds =
       FidesReader->ReadStep(FidesPaths, selections);
    vtkm::rendering::Scene scene;
    vtkm::rendering::MapperRayTracer mapper;
    vtkm::rendering::CanvasRayTracer canvas(1024, 1024);
    vtkm::rendering::Color bg(0.2f, 0.2f, 0.2f, 1.0f);
    vtkm::cont::ColorTable colorTable(
        vtkm::cont::ColorTable::Preset::CoolToWarm);

    // Render an image of the output
    for (vtkm::Id i = 0; i < outputds.GetNumberOfPartitions(); ++i)
    {
      auto& outputData = outputds.GetPartition(i);
      scene.AddActor(vtkm::rendering::Actor(outputData.GetCellSet(),
                                            outputData.GetCoordinateSystem(),
                                            outputData.GetField("U"),
                                            colorTable));
    }
    vtkm::rendering::View3D view(scene, mapper, canvas);
    auto& camera = view.GetCamera();
    camera.Azimuth(45.0);
    camera.Elevation(45.0);
    view.Paint();
    std::string filename = "fides-output/step-" + std::to_string(step) + ".png";
    std::cout << "Rendering image " << filename << std::endl;
    view.SaveAs(filename);
#else
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

#endif
}

void Reader::close()
{
#ifdef USE_FIDES
#else
  reader.Close();
#ifdef USE_CATALYST
    CatalystAdaptor::Finalize();
#endif

#endif
}

void Reader::print_settings()
{
    std::cout << "Simulation reads data using engine type:              "
              << io.EngineType() << std::endl;
}
