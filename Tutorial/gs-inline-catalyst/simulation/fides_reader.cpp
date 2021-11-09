#include "fides_reader.h"

#include <vtkm/rendering/Camera.h>
#include <vtkm/rendering/CanvasRayTracer.h>
#include <vtkm/rendering/MapperRayTracer.h>
#include <vtkm/rendering/Scene.h>
#include <vtkm/rendering/View3D.h>

#include <iostream>

FidesReader::FidesReader(const Settings &settings, adios2::IO io, int argc, char* argv[])
: settings(settings), io(io)
{
    std::string jsonConfig = std::string(argv[2]);
    Reader.reset(new fides::io::DataSetReader(jsonConfig));
    fides::DataSourceParams params;
    params["engine_type"] = "Inline";
    params["writer_id"] = settings.output;
    Reader->SetDataSourceParameters("the-source", std::move(params));
    Reader->SetDataSourceIO("the-source", &this->io);
}

void FidesReader::open(const std::string &fname)
{
    FidesPaths["the-source"] = fname;
}

void FidesReader::read(int step)
{
    Reader->PrepareNextStep(FidesPaths);
    auto metaData = Reader->ReadMetaData(FidesPaths);
    fides::metadata::MetaData selections;

    vtkm::cont::PartitionedDataSet outputds =
       Reader->ReadStep(FidesPaths, selections);
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
}

void FidesReader::close()
{
}

void FidesReader::print_settings()
{
    std::cout << "Simulation reads data using engine type:              "
              << io.EngineType() << std::endl;
}
