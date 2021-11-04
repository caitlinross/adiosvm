from paraview.simple import *
from paraview import catalyst

# Greeting to ensure that ctest knows this script is being imported
print("executing catalyst_pipeline")

view = CreateRenderView()

# registrationName must match the channel name used in the
# 'CatalystAdaptor'.
producer = TrivialProducer(registrationName="grid")
display = Show(producer)
ColorBy(display, ('POINTS', 'u'))
display.RescaleTransferFunctionToDataRange(True, False)
#display.SetScalarBarVisibility(view, True)
uLUT = GetColorTransferFunction('u')
uPWF = GetOpacityTransferFunction('u')

# change representation type
display.SetRepresentationType('Surface')

clip = Clip(registrationName="clip1", Input=producer)
clip.ClipType = 'Plane'
clip.Scalars = ['POINTS', 'u']
clip.ClipType.Origin = [1.5, 1.5, 1.5]
clipDisplay = Show(clip, view, 'UnstructuredGridRepresentation')

Hide(producer, view)

view.CameraPosition = [11.232435731717569, 2.3712243169047245, 3.799137821440938]
view.CameraFocalPoint = [1.5, 1.5, 1.5]
view.CameraViewUp = [-0.04050591825835369, 0.9790535372862995, -0.19953305919880143]
view.CameraParallelScale = 2.598076211353316

options = catalyst.Options()
options.EnableCatalystLive = 1
SaveExtractsUsingCatalystOptions(options)

def catalyst_execute(info):
    global producer
    producer.UpdatePipeline()

    fname = "catalyst-output/img-%d.png" % info.timestep
    ResetCamera()
    SaveScreenshot(fname)

    print("-----------------------------------")

    print("executing (cycle={}, time={})".format(info.cycle, info.time))
    print("bounds:", producer.GetDataInformation().GetBounds())
    print("u-range:", producer.PointData["u"].GetRange(0))
    print("v-range:", producer.PointData["v"].GetRange(0))

    print("bounds:", clip.GetDataInformation().GetBounds())
    print("u-range:", clip.PointData["u"].GetRange(0))
    print("v-range:", clip.PointData["v"].GetRange(0))

