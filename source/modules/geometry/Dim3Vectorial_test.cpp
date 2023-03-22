#include "modules/geometry/Dim3Vectorial.h"

#include "test_utils/GenericMethods.h"
#include "test_utils/TestInitializer.h"

#include <array>
#include <string>

constexpr int nbRepetitions = 100;

int main( int p_argc, char ** p_argv )
{
    return LaunchTest( p_argc, p_argv );
}

template<typename T>
void TestConstructorIntegralType()
{
    std::cout << "TestConstructorIntegralType for type " << typeid( T ).name() << std::endl;
    auto expectedX = Random<T>();
    auto expectedY = Random<T>();
    auto expectedZ = Random<T>();
    Dim3Vectorial<T> point( expectedX, expectedY, expectedZ );
    EXPECT_EQ( point.x, expectedX );
    EXPECT_EQ( point.y, expectedY );
    EXPECT_EQ( point.z, expectedZ );
}

template<typename T>
void TestConstructorFloatingType()
{
    std::cout << "TestConstructorFloatingType for type " << typeid( T ).name() << std::endl;
    auto expectedX = Random<T>();
    auto expectedY = Random<T>();
    auto expectedZ = Random<T>();
    Dim3Vectorial<T> point( expectedX, expectedY, expectedZ );
    EXPECT_DOUBLE_EQ( point.x, expectedX );
    EXPECT_DOUBLE_EQ( point.y, expectedY );
    EXPECT_DOUBLE_EQ( point.z, expectedZ );
}

template<typename T>
void TestFromStringIntegralType()
{
    std::cout << "TestFromStringIntegralType for type " << typeid( T ).name() << std::endl;
    auto expectedX = Random<T>();
    auto expectedY = Random<T>();
    auto expectedZ = Random<T>();
    std::string stringValues = ToStdString<T>( expectedX ) + " " + ToStdString<T>( expectedY ) + " " + ToStdString<T>( expectedZ );
    auto pointOptional = Dim3Vectorial<T>::FromString( stringValues );
    ASSERT_TRUE( pointOptional.has_value() );
    EXPECT_EQ( pointOptional.value().x, expectedX );
    EXPECT_EQ( pointOptional.value().y, expectedY );
    EXPECT_EQ( pointOptional.value().z, expectedZ );
}

template<typename T>
void TestFromStringFloatingType()
{
    std::cout << "TestFromStringFloatingType for type " << typeid( T ).name() << std::endl;
    auto expectedX = Random<T>();
    auto expectedY = Random<T>();
    auto expectedZ = Random<T>();
    std::string stringValues = ToStdString<T>( expectedX ) + " " + ToStdString<T>( expectedY ) + " " + ToStdString<T>( expectedZ );
    auto pointOptional = Dim3Vectorial<T>::FromString( stringValues );
    ASSERT_TRUE( pointOptional.has_value() );
    EXPECT_DOUBLE_EQ( pointOptional.value().x, expectedX );
    EXPECT_DOUBLE_EQ( pointOptional.value().y, expectedY );
    EXPECT_DOUBLE_EQ( pointOptional.value().z, expectedZ );
}

template<typename T>
void TestComparison()
{
    std::cout << "TestComparison for type " << typeid( T ).name() << std::endl;
    auto originalX = Random<T>();
    auto originalY = Random<T>();
    auto originalZ = Random<T>();
    Dim3Vectorial<T> point1( originalX, originalY, originalZ );
    Dim3Vectorial<T> point2( originalX, originalY, originalZ );
    EXPECT_TRUE( point1 == point2 );
    EXPECT_FALSE( point1 != point2 );
    auto differentX = originalX + Random<T>();
    auto differentY = originalY + Random<T>();
    auto differentZ = originalZ + Random<T>();
    while( differentX == originalX && differentY == originalY && differentZ == originalZ )
    {
        differentX = originalX + Random<T>();
        differentY = originalY + Random<T>();
        differentZ = originalZ + Random<T>();
    }
    Dim3Vectorial<T> point3( differentX, differentY, differentZ );
    EXPECT_TRUE( point1 != point3 );
    EXPECT_FALSE( point1 == point3 );
}

template<typename T>
void TestAddition()
{
    std::cout << "TestAddition for type " << typeid( T ).name() << std::endl;
    Dim3Vectorial<T> left( Random<T>(), Random<T>(), Random<T>() );
    Dim3Vectorial<T> right( Random<T>(), Random<T>(), Random<T>() );
    Dim3Vectorial<T> expectedSum( left.x + right.x, left.y + right.y, left.z + right.z );
    EXPECT_EQ( left + right, expectedSum );
}

template<typename T>
void TestSubtraction()
{
    std::cout << "TestSubtraction for type " << typeid( T ).name() << std::endl;
    Dim3Vectorial<T> left( Random<T>(), Random<T>(), Random<T>() );
    Dim3Vectorial<T> right( Random<T>(), Random<T>(), Random<T>() );
    Dim3Vectorial<T> expectedSubtraction( left.x - right.x, left.y - right.y, left.z - right.z );
    EXPECT_EQ( left - right, expectedSubtraction );
}

template<typename T>
void TestMultiplicationByScalar()
{
    std::cout << "TestMultiplicationByScalar for type " << typeid( T ).name() << std::endl;
    Dim3Vectorial<T> point( Random<T>(), Random<T>(), Random<T>() );
    auto scalar = Random<T>();
    Dim3Vectorial<T> expectedMultiplication( scalar * point.x, scalar * point.y, scalar * point.z );
    EXPECT_EQ( scalar * point, expectedMultiplication );
    EXPECT_EQ( point * scalar, expectedMultiplication );
}

TEST( Dim3VectorialTest, ConstructorsTests )
{
    TestConstructorIntegralType<short>();
    TestConstructorIntegralType<int>();
    TestConstructorIntegralType<long>();
    TestConstructorIntegralType<unsigned short>();
    TestConstructorIntegralType<unsigned int>();
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestConstructorFloatingType<float>();
    }
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestConstructorFloatingType<double>();
    }
}

TEST( Dim3VectorialTest, FromStringsTests )
{
    TestFromStringIntegralType<short>();
    TestFromStringIntegralType<int>();
    TestFromStringIntegralType<long>();
    TestFromStringIntegralType<unsigned short>();
    TestFromStringIntegralType<unsigned int>();
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestFromStringFloatingType<float>();
    }
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestFromStringFloatingType<double>();
    }
}

TEST( Dim3VectorialTest, ComparatorsTests )
{
    TestComparison<short>();
    TestComparison<int>();
    TestComparison<long>();
    TestComparison<unsigned short>();
    TestComparison<unsigned int>();
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestComparison<float>();
    }
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestComparison<double>();
    }
}

TEST( Dim3VectorialTest, AdditionTests )
{
    TestAddition<short>();
    TestAddition<int>();
    TestAddition<long>();
    TestAddition<unsigned short>();
    TestAddition<unsigned int>();
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestAddition<float>();
    }
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestAddition<double>();
    }
}

TEST( Dim3VectorialTest, SubtractionTests )
{
    TestSubtraction<short>();
    TestSubtraction<int>();
    TestSubtraction<long>();
    TestSubtraction<unsigned short>();
    TestSubtraction<unsigned int>();
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestSubtraction<float>();
    }
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestSubtraction<double>();
    }
}

TEST( Dim3VectorialTest, MultiplicationByScalarTests )
{
    TestMultiplicationByScalar<short>();
    TestMultiplicationByScalar<int>();
    TestMultiplicationByScalar<long>();
    TestMultiplicationByScalar<unsigned short>();
    TestMultiplicationByScalar<unsigned int>();
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestMultiplicationByScalar<float>();
    }
    for( int i = 0; i < nbRepetitions; i++ )    // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestMultiplicationByScalar<double>();
    }
}

TEST( Dim3VectorialTest, FromStringIntFromFloatString )
{
    auto expectedX = Random<float>();
    auto expectedY = Random<float>();
    auto expectedZ = Random<float>();
    auto stringValues = ToStdString<float>( expectedX ) + " " + ToStdString<float>( expectedY ) + " " + ToStdString<float>( expectedZ );
    auto pointOptional = Dim3Vectorial<int>::FromString( stringValues );
    ASSERT_TRUE( pointOptional.has_value() );
    EXPECT_EQ( pointOptional.value().x, static_cast<int>( expectedX ) );
    EXPECT_EQ( pointOptional.value().y, static_cast<int>( expectedY ) );
    EXPECT_EQ( pointOptional.value().z, static_cast<int>( expectedZ ) );
}

TEST( Dim3VectorialTest, FromStringWithWrongValues )
{
    std::string stringValues{ "rrr TT dfsf" };
    bool result = true;
    auto intPointOptional = Dim3Vectorial<int>::FromString( stringValues );
    EXPECT_FALSE( intPointOptional.has_value() );
    EXPECT_EQ( intPointOptional.value().x, 0 );
    EXPECT_EQ( intPointOptional.value().y, 0 );
    EXPECT_EQ( intPointOptional.value().z, 0 );
    auto floatPointOptional = Dim3Vectorial<float>::FromString( stringValues );
    EXPECT_FALSE( result );
    EXPECT_FLOAT_EQ( floatPointOptional.value().x, 0.F );
    EXPECT_FLOAT_EQ( floatPointOptional.value().y, 0.F );
    EXPECT_FLOAT_EQ( floatPointOptional.value().z, 0.F );
}
