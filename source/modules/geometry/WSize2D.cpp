#include "modules/geometry/WSize2D.h"

#include "modules/geometry/Size2D.h"
#include "modules/geometry/PixelSpacing.h"


WSize2D::WSize2D( const PixelSpacing & p_pixelSpacing, const Size2D & p_size )
 : Dim2<float>(static_cast<float>( p_size.x ) * p_pixelSpacing.x,static_cast<float>( p_size.y ) * p_pixelSpacing.y ) 
{} 
// Why constructor is located here and not in header file.
// Perfect forwrding prevents from include cycles betwee WSize2D, Size2D and VoxelSpacing classes.
// But does not allow for the use of objects of the classes Size2D and VoxelSpacing in header file.