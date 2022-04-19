// Your First C++ Program

#include <iostream>
#include <cstdlib>
#include <vtkTIFFReader.h>
#include <vtkSmartPointer.h>
//#include <vtkTIFFWriter.h>

int main() {
    std::cout << "Hello World!" << std::endl;

    auto listsEnvironments = {"PATH", "path", "Path"};
    for(auto && environementName : listsEnvironments) {
        auto environment = std::getenv(environementName);
        if(environment != nullptr )
        {
            std::cout << "Your " << environementName << " is: " << environment << std::endl;
        }
    }

    std::cout << "vtk test" << std::endl;
    auto fileName = "C:/dev/ADAM/source/personal/Kevernals/resources/testImage.tif";
    // Read the image
    auto tiffReader = vtkSmartPointer<vtkTIFFReader>::New();
    tiffReader->SetFileName(fileName);

    return 0;
}