#include "modules/dataHandling/PhantomMaker.h"
#include "modules/geometry/TomoGeometry.h"
#include "commons/GlobalUtils.h"
 
#include <vtkImageCast.h>
#include <vtkTIFFWriter.h>

#include <iostream>
#include <string>
#include <chrono>
#include <optional>
#include <ratio>    // for std::milli

#define writeTestImages_
 

int main( int argc, char * argv[] )
{
    (void) argc ;
    (void) argv ;

    std::string geometryXmlFilePath{ "C:/Users/loquin.k/Documents/tomosynthesis_data/tomos_geometry_test.xml" };
    auto tomoGeometry = std::make_unique<TomoGeometry>( geometryXmlFilePath );

    if( !tomoGeometry->IsValid() )
    {
        std::cout << "invalid geometry parsing" << std::endl ;
        glob::WaitForKeyTyping();
        return 1;
    }

    std::cout << "geometry parsed" << std::endl;

    ImageDataPtr phantom;
    std::cout << "phantom computation" << std::endl;
    PhantomMaker phantomMaker( tomoGeometry->GetVolume() );
    Pave paveToAdd( Position3D( -25.f, -50.f, 207.f ), WSize3D( 20.f, 40.f, 30.f ) );
    Sphere sphereToAdd( Position3D( 60.f, -100.f, 220.f ), 15.f );
    phantomMaker.AddPave( paveToAdd, 100.f );
    phantomMaker.AddSphere( sphereToAdd, 300.f );
    phantom = phantomMaker.GetPhantom();

    if( phantom == nullptr )
    {
        std::cout << "projection is nullptr" << std::endl;
        glob::WaitForKeyTyping();
        return 1;
    }
    auto tiffWriterGlobal = vtkSmartPointer<vtkTIFFWriter>::New();
    std::string sliceFilename{ "../phantom.tiff" };
    tiffWriterGlobal->SetFileName( sliceFilename.c_str() );
    tiffWriterGlobal->SetInputData( phantom );
    tiffWriterGlobal->Write();
    std::cout << "phantom done" << std::endl;


    std::cout << " ---  DONE  ---  ;)" << std::endl;
    glob::WaitForKeyTyping();
    return 0;
}
