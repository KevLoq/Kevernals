#include "modules/dataHandling/PhantomMaker.h"
#include "modules/geometry/TomoGeometry.h"

#include <QDebug>
#include <vtkImageCast.h>
#include <vtkTIFFWriter.h>

#include <chrono>
#include <optional>
#include <ratio>    // for std::milli

#define writeTestImages_

QTextStream qtin( stdin );

int main( int argc, char * argv[] )
{
    Q_UNUSED( argc );
    Q_UNUSED( argv );

    QString geometryXmlFilePath{ "C:/Users/loquin.k/Documents/tomosynthesis_data/tomos_geometry_test.xml" };
    auto tomoGeometry = std::make_unique<TomoGeometry>( geometryXmlFilePath );

    if( !tomoGeometry->IsValid() )
    {
        qCritical() << "invalid geometry pasring";
        qtin.readLine();
        return 1;
    }

    qInfo() << "geometry parsed";

    ImageDataPtr phantom;
    qInfo() << "phantom computation";
    PhantomMaker phantomMaker( tomoGeometry->GetVolume() );
    Pave paveToAdd( Position3D( -25.f, -50.f, 207.f ), WSize3D( 20.f, 40.f, 30.f ) );
    Sphere sphereToAdd( Position3D( 60.f, -100.f, 220.f ), 15.f );
    phantomMaker.AddPave( paveToAdd, 100.f );
    phantomMaker.AddSphere( sphereToAdd, 300.f );
    phantom = phantomMaker.GetPhantom();

    if( phantom == nullptr )
    {
        qCritical() << "projection is nullptr";
        qtin.readLine();
        return 1;
    }
    auto tiffWriterGlobal = vtkSmartPointer<vtkTIFFWriter>::New();
    QString sliceFilename{ "../phantom.tiff" };
    tiffWriterGlobal->SetFileName( sliceFilename.toStdString().c_str() );
    tiffWriterGlobal->SetInputData( phantom );
    tiffWriterGlobal->Write();
    qInfo() << "phantom done";


    qInfo() << " ---  DONE  ---  ;)";
    qtin.readLine();
    return 0;
}
