#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/Maths.h"

#include <QDebug>
#include <QString>
#include <QVector>

#include <algorithm>
#include <cmath>

inline std::vector<int> GetHist16Bit( const ImageDataPtr & p_src )
{
    const auto hist_sz = static_cast<int>( std::pow( 2, 16 ) );

    std::vector<int> hist( hist_sz, 0 );
    if( p_src == nullptr )
    {
        return hist;
    }
    if( p_src->GetScalarType() != VTK_UNSIGNED_SHORT )
    {
        // TODO Add some error management instead of returning an 0 valued histogram
        return hist;
    }
    auto width = p_src->GetDimensions()[0];
    auto height = p_src->GetDimensions()[1];
    auto depth = p_src->GetDimensions()[2];
    for( auto z{ 0 }; z < depth; z++ )
    {
        for( auto y{ 0 }; y < height; y++ )
        {
            for( auto x{ 0 }; x < width; x++ )
            {
                auto pixValue = static_cast<unsigned short *>( p_src->GetScalarPointer( x, y, z ) );
                hist[static_cast<int>( pixValue[0] )]++;
            }
        }
    }
    return hist;
}

//inline void EqualizeHist16Bit( const cv::Mat & p_src, cv::Mat & p_dst )
//{
//    p_dst = p_src.clone();
//
//    auto hist = GetHist16Bit( p_src );
//    auto histSize = static_cast<int>( hist.size() );
//    QVector<int> lut( histSize );
//
//    auto i = 0;
//    while( hist[i] == 0 )
//    {
//        ++i;
//    }
//
//    auto total = static_cast<int>( p_src.total() );
//    if( hist[i] == total )
//    {
//        p_dst.setTo( i );
//        return;
//    }
//
//    double scale = static_cast<double>( histSize - 1 ) / static_cast<double>( total - hist[i] );
//    auto sum = 0;
//
//    for( lut[i++] = 0; i < histSize; ++i )
//    {
//        sum += hist[i];
//        lut[i] = cv::saturate_cast<ushort>( sum * scale );
//    }
//
//    for( int y = 0; y < p_src.rows; y++ )
//    {
//        for( int x = 0; x < p_src.cols; x++ )
//        {
//            p_dst.at<unsigned short int>( y, x ) = lut[(int)p_src.at<unsigned short int>( y, x )];
//        }
//    }
//}


inline void HistMatching16Bit( std::vector<int> p_expectedHistogram, ImageDataPtr p_matToAdapt )
{
    if( p_matToAdapt == nullptr )
    {
        qDebug() << "HistMatching16Bit: image to adapt is not ushort";
        return;
    }
    if( p_matToAdapt->GetScalarType() != VTK_UNSIGNED_SHORT )
    {
        qDebug() << "HistMatching16Bit: image to adapt is not ushort";
        return;
    }

    auto expectedHistSize = static_cast<int>( p_expectedHistogram.size() );

    auto originalHistogram = GetHist16Bit( p_matToAdapt );
    auto originalHistSize = static_cast<int>( originalHistogram.size() );
    if( originalHistSize != expectedHistSize )    // hist sizes mismatch
    {
        qDebug() << "HistMatching16Bit: hist sizes mismatch";
        return;
    }

    std::vector<ushort> lut( expectedHistSize, 0 );

    auto firstNonNullIndex = 0;
    while( firstNonNullIndex < expectedHistSize && p_expectedHistogram[firstNonNullIndex] == 0 )
    {
        ++firstNonNullIndex;
    }

    auto width = p_matToAdapt->GetDimensions()[0];
    auto height = p_matToAdapt->GetDimensions()[1];
    auto depth = p_matToAdapt->GetDimensions()[2];
    auto total = width * height;
    if( p_expectedHistogram[firstNonNullIndex] == total )    // histogram concentrated on one gray level -> output image is uniform
    {
        for( auto z{ 0 }; z < depth; z++ )
        {
            for( auto y{ 0 }; y < height; y++ )
            {
                for( auto x{ 0 }; x < width; x++ )
                {
                    auto pixValue = static_cast<unsigned short *>( p_matToAdapt->GetScalarPointer( x, y, z ) );
                    pixValue[0] = firstNonNullIndex;
                }
            }
        }
        return;
    }


    // Due to masking process, the 0 bin in histogram is misleading for the algorithm
    // Thue we remove them. This removal makes it compulsory to renormalize the cdf
    p_expectedHistogram[0] = 0;
    originalHistogram[0] = 0;
    lut[0] = static_cast<ushort>( 0 );

    std::vector<double> expectedCumulativeDistribution( expectedHistSize, 0 );
    std::vector<double> originalCumulativeDistribution( originalHistSize, 0 );

    // cdf computation
    expectedCumulativeDistribution[0] = static_cast<double>( p_expectedHistogram[0] );
    originalCumulativeDistribution[0] = static_cast<double>( originalHistogram[0] );
    for( auto graylevel{ 1 }; graylevel < originalHistSize; ++graylevel )
    {
        expectedCumulativeDistribution[graylevel] = expectedCumulativeDistribution[graylevel - 1] + static_cast<double>( p_expectedHistogram[graylevel] );
        originalCumulativeDistribution[graylevel] = originalCumulativeDistribution[graylevel - 1] + static_cast<double>( originalHistogram[graylevel] );
    }
    // normalizations
    for( auto graylevel{ 0 }; graylevel < originalHistSize; ++graylevel )
    {
        expectedCumulativeDistribution[graylevel] /= expectedCumulativeDistribution[originalHistSize - 1];
        originalCumulativeDistribution[graylevel] /= originalCumulativeDistribution[originalHistSize - 1];
    }

    for( auto graylevel = 0; graylevel < originalHistSize; ++graylevel )
    {
        auto originalAccumulatedValue = originalCumulativeDistribution[graylevel];
        // lookup of the closest value of originalAccumulatedValue in vector expectedCumulativeDistribution
        auto upper = std::upper_bound( expectedCumulativeDistribution.begin(), expectedCumulativeDistribution.end(), originalAccumulatedValue );

        if( upper == expectedCumulativeDistribution.end() )
        {
            lut[graylevel] = std::clamp( graylevel, 0, USHRT_MAX );    // Check it
        }
        else
        {
            // take just above and just under and take the closest
            // auto previousValue = *( upper - 1 );
            // auto nextValue = *upper;
            if( originalAccumulatedValue - *( upper - 1 ) < *upper - originalAccumulatedValue )
            {
                // closest value is the previous one
                lut[graylevel] = std::clamp( static_cast<int>( std::distance( expectedCumulativeDistribution.begin(), upper - 1 ) ), 0, USHRT_MAX );
            }
            else
            {
                // closest value is the next one
                lut[graylevel] = std::clamp( static_cast<int>( std::distance( expectedCumulativeDistribution.begin(), upper ) ), 0, USHRT_MAX );
            }
        }
    }

    for( auto z{ 0 }; z < depth; z++ )
    {
        for( auto y{ 0 }; y < height; y++ )
        {
            for( auto x{ 0 }; x < width; x++ )
            {
                auto pixValue = static_cast<unsigned short *>( p_matToAdapt->GetScalarPointer( x, y, z ) );
                pixValue[0] = lut[static_cast<int>( pixValue[0] )];
            }
        }
    }
}


inline void HistUniformization16Bit( ImageDataPtr p_matToAdapt )
{
    const auto hist_sz = static_cast<int>( std::pow( 2, 16 ) );
    std::vector<int> uniformHist( hist_sz, 1 );
    HistMatching16Bit( uniformHist, p_matToAdapt );
}


inline void HistHalfTop16Bit( ImageDataPtr p_matToAdapt )
{
    const auto hist_sz = static_cast<int>( std::pow( 2, 16 ) );
    std::vector<int> uniformHist( hist_sz, 1 );
    for( int greyLevel = 0; greyLevel < 1000; greyLevel++ )
    {
        uniformHist[greyLevel] *= 20;
    }
    HistMatching16Bit( uniformHist, p_matToAdapt );
}

//
//inline void HistMatching16Bit( const cv::Mat & p_expectedMat, cv::Mat & p_matToAdapt )
//{
//    HistMatching16Bit( GetHist16Bit( p_expectedMat ), p_matToAdapt );
//}
