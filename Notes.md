# Memory limitations

## Problem parameters

- #projections : 40
- projections dimension: 911x959 
- => #projection positions: 911x959x40 = 34 945 960
- For each projection position, its projection ray contains (in avg) 132.325643279 voxels
- Total number of combinations voxel position/projection position = 4 624 246 637 => number of elements of the Radon matrix
- Each element of RadonMatrix contains a int position (projection position for projection and voxel position for back projection) and a float weight -> 8 Bytes by radon matrix element
- Radon matrix should be 4 624 246 637 x 8 Bytes = 36 993 973 096 Bytes  -> ~37 GBytes 





