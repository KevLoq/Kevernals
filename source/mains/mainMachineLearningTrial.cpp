#include <iostream>
#include <torch/torch.h>

int main()
{
    torch::Tensor tensor = torch::rand( { 2, 3 } );
    std::cout << tensor << std::endl;


    std::cout << " ---  DONE  ---  ;)" << std::endl;


    int a{ 0 };
    std::cin >> a;
}