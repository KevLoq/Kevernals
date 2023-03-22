#include "modules/geometry/Dim3.h"
#include "test_utils/GenericMethods.h" 
#include "test_utils/TestInitializer.h"

#include <array>
#include <string>

constexpr int nbRepetitions = 100;
 
int main( int p_argc, char ** p_argv )
{
    return LaunchTest(p_argc, p_argv);
}

template<typename T>
void TestConstructorIntegralType()
{ 
    std::cout << "TestConstructorIntegralType for type " << typeid(T).name() << std::endl; 
    auto expectedX = Random<T>();
    auto expectedY = Random<T>();
    auto expectedZ = Random<T>();
    Dim3<T> point(expectedX,expectedY,expectedZ); 
    EXPECT_EQ(point.x,expectedX);
    EXPECT_EQ(point.y,expectedY);
    EXPECT_EQ(point.z,expectedZ);
}

template<typename T>
void TestConstructorFloatingType()
{ 
    std::cout << "TestConstructorFloatingType for type " << typeid(T).name() << std::endl; 
    auto expectedX = Random<T>();
    auto expectedY = Random<T>();
    auto expectedZ = Random<T>();
    Dim3<T> point(expectedX,expectedY,expectedZ); 
    EXPECT_DOUBLE_EQ(point.x,expectedX);
    EXPECT_DOUBLE_EQ(point.y,expectedY);
    EXPECT_DOUBLE_EQ(point.z,expectedZ);
}

template<typename T>
void TestFromStringArrayIntegralType()
{
    std::cout << "TestFromStringArrayIntegralType for type " << typeid(T).name() << std::endl; 
    auto expectedX = Random<T>();
    auto expectedY = Random<T>();
    auto expectedZ = Random<T>();
    std::array<std::string,3> stringValues{ToStdString<T>(expectedX),ToStdString<T>(expectedY),ToStdString<T>(expectedZ)};
    auto point = Dim3<T>::FromStringArray(stringValues) ;
    ASSERT_TRUE(point.has_value());
    EXPECT_EQ(point.value().x,expectedX);
    EXPECT_EQ(point.value().y,expectedY);
}

template<typename T>
void TestFromStringArrayFloatingType()
{ 
    std::cout << "TestFromStringArrayFloatingType for type " << typeid(T).name() << std::endl; 
    auto expectedX = Random<T>();
    auto expectedY = Random<T>();
    auto expectedZ = Random<T>();
    std::array<std::string,3> stringValues{ToStdString<T>(expectedX),ToStdString<T>(expectedY),ToStdString<T>(expectedZ)};
    auto point = Dim3<T>::FromStringArray(stringValues);
    ASSERT_TRUE(point.has_value());
    EXPECT_DOUBLE_EQ(point.value().x,expectedX);
    EXPECT_DOUBLE_EQ(point.value().y,expectedY);
}

template<typename T>
void TestFromStringIntegralType()
{
    std::cout << "TestFromStringIntegralType for type " << typeid(T).name() << std::endl; 
    auto expectedX = Random<T>();
    auto expectedY = Random<T>();
    auto expectedZ = Random<T>();
    auto stringValues = ToStdString<T>(expectedX) + " " + ToStdString<T>(expectedY) + " " + ToStdString<T>(expectedZ);
    auto point = Dim3<T>::FromString(stringValues) ;
    ASSERT_TRUE(point.has_value());
    EXPECT_EQ(point.value().x,expectedX);
    EXPECT_EQ(point.value().y,expectedY);
    EXPECT_EQ(point.value().z,expectedZ);
}

template<typename T>
void TestFromStringFloatingType()
{ 
    std::cout << "TestFromStringFloatingType for type " << typeid(T).name() << std::endl; 
    auto expectedX = Random<T>();
    auto expectedY = Random<T>();
    auto expectedZ = Random<T>();
    auto stringValues = ToStdString<T>(expectedX) + " " + ToStdString<T>(expectedY) + " " + ToStdString<T>(expectedZ);
    auto point = Dim3<T>::FromString(stringValues);
    ASSERT_TRUE(point.has_value());
    EXPECT_DOUBLE_EQ(point.value().x, expectedX);
    EXPECT_DOUBLE_EQ(point.value().y, expectedY);
    EXPECT_DOUBLE_EQ(point.value().z, expectedZ);
}

template<typename T>
void TestComparison()
{ 
    std::cout << "TestComparison for type " << typeid(T).name() << std::endl; 
    auto originalX = Random<T>();
    auto originalY = Random<T>();
    auto originalZ = Random<T>();
    Dim3<T> point1(originalX,originalY,originalZ); 
    Dim3<T> point2(originalX,originalY,originalZ);
    EXPECT_TRUE(point1 == point2); 
    EXPECT_FALSE(point1 != point2);
    auto differentX = originalX + Random<T>();
    auto differentY = originalY + Random<T>();
    auto differentZ = originalZ + Random<T>();
    while( differentX == originalX && differentY == originalY && differentZ == originalZ)
    {
        differentX = originalX + Random<T>();
        differentY = originalY + Random<T>();
        differentZ = originalZ + Random<T>();
    }
    Dim3<T> point3(differentX,differentY,differentZ);
    EXPECT_TRUE(point1 != point3); 
    EXPECT_FALSE(point1 == point3);
}

template<typename T>
void TestMultiplicationByScalar()
{ 
    std::cout << "TestMultiplicationByScalar for type " << typeid(T).name() << std::endl; 
    Dim3<T> point(Random<T>(),Random<T>(),Random<T>()); 
    auto scalar = Random<T>();
    Dim3<T> expectedMultiplication(scalar * point.x, scalar * point.y, scalar * point.z);
    EXPECT_EQ( scalar * point , expectedMultiplication );
    EXPECT_EQ( point * scalar , expectedMultiplication );
}

TEST( Dim3Test, ConstructorsTests )
{  
    TestConstructorIntegralType<short>();
    TestConstructorIntegralType<int>();
    TestConstructorIntegralType<long>();  
    TestConstructorIntegralType<unsigned short>();
    TestConstructorIntegralType<unsigned int>();
    for(int i = 0; i < nbRepetitions ; i++) // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestConstructorFloatingType<float>();
    }
    for(int i = 0; i < nbRepetitions ; i++) // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestConstructorFloatingType<double>();
    }
}

TEST( Dim3Test, FromStringArraysTests )
{  
    TestFromStringArrayIntegralType<short>();
    TestFromStringArrayIntegralType<int>();
    TestFromStringArrayIntegralType<long>();  
    TestFromStringArrayIntegralType<unsigned short>();
    TestFromStringArrayIntegralType<unsigned int>();
    for(int i = 0; i < nbRepetitions ; i++) // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestFromStringArrayFloatingType<float>();  
    }
    for(int i = 0; i < nbRepetitions ; i++) // Repeatability and thus robustness is important for floating point arithmetic
    { 
        TestFromStringArrayFloatingType<double>();
    }
}

TEST( Dim3Test, FromStringTests )
{
    TestFromStringIntegralType<short>();
    TestFromStringIntegralType<int>();
    TestFromStringIntegralType<long>();
    TestFromStringIntegralType<unsigned short>();
    TestFromStringIntegralType<unsigned int>();
    for(int i = 0; i < nbRepetitions ; i++) // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestFromStringFloatingType<float>();  
    }
    for(int i = 0; i < nbRepetitions ; i++) // Repeatability and thus robustness is important for floating point arithmetic
    { 
        TestFromStringFloatingType<double>();
    }
}


TEST( Dim3Test, ComparatorsTests )
{  
    TestComparison<short>();
    TestComparison<int>();
    TestComparison<long>();  
    TestComparison<unsigned short>();
    TestComparison<unsigned int>();
    for(int i = 0; i < nbRepetitions ; i++) // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestComparison<float>();  
    }
    for(int i = 0; i < nbRepetitions ; i++) // Repeatability and thus robustness is important for floating point arithmetic
    { 
        TestComparison<double>(); 
    }
}

TEST( Dim3Test, MultiplicationByScalarTests )
{ 
    TestMultiplicationByScalar<short>();
    TestMultiplicationByScalar<int>();
    TestMultiplicationByScalar<long>();
    TestMultiplicationByScalar<unsigned short>();
    TestMultiplicationByScalar<unsigned int>();
    for(int i = 0; i < nbRepetitions ; i++) // Repeatability and thus robustness is important for floating point arithmetic
    {
        TestMultiplicationByScalar<float>(); 
    }
    for(int i = 0; i < nbRepetitions ; i++) // Repeatability and thus robustness is important for floating point arithmetic
    { 
        TestMultiplicationByScalar<double>();
    }
}
 
TEST( Dim3Test, FromStringArrayIntFromFloatString )
{ 
    auto expectedX = Random<float>();
    auto expectedY = Random<float>();
    auto expectedZ = Random<float>();
    std::array<std::string,3> stringValues{ToStdString<float>(expectedX),ToStdString<float>(expectedY),ToStdString<float>(expectedZ)};
    auto point = Dim3<int>::FromStringArray(stringValues) ;
    ASSERT_TRUE(point.has_value());
    EXPECT_EQ(point.value().x, static_cast<int>(expectedX));
    EXPECT_EQ(point.value().y,static_cast<int>(expectedY));
    EXPECT_EQ(point.value().z,static_cast<int>(expectedZ));
}

TEST( Dim3Test, FromStringArrayWithWrongValues)
{  
    std::array<std::string,3> stringValues{"rrr","TT","dfsf"};
    auto intPoint = Dim3<int>::FromStringArray(stringValues) ;
    EXPECT_FALSE(intPoint.has_value());
    auto floatPoint = Dim3<float>::FromStringArray(stringValues) ;
    EXPECT_FALSE(floatPoint.has_value());
}

TEST( Dim3Test, FromStringIntFromFloatString )
{ 
    auto expectedX = Random<float>();
    auto expectedY = Random<float>();
    auto expectedZ = Random<float>();
    auto stringValues = ToStdString<float>(expectedX) + " " + ToStdString<float>(expectedY) + " " + ToStdString<float>(expectedZ);
    auto point = Dim3<int>::FromString(stringValues) ;
    ASSERT_TRUE(point.has_value());
    EXPECT_EQ(point.value().x, static_cast<int>(expectedX));
    EXPECT_EQ(point.value().y,static_cast<int>(expectedY));
}

TEST( Dim3Test, FromStringWithWrongValues)
{
    std::string stringValues{"rrr TT YOYUY"};
    auto intPoint = Dim3<int>::FromString(stringValues) ;
    EXPECT_FALSE(intPoint.has_value()); 
    auto floatPoint = Dim3<float>::FromString(stringValues) ;
    EXPECT_FALSE(floatPoint.has_value());

    auto testX = Random<float>();
    auto testY = Random<float>();
    
    stringValues = ToStdString<float>(testX) + " " + ToStdString<float>(testY) ;
   
    intPoint = Dim3<int>::FromString(stringValues) ;
    EXPECT_FALSE(intPoint.has_value()); 
    floatPoint = Dim3<float>::FromString(stringValues) ;
    EXPECT_FALSE(floatPoint.has_value());

    auto testZ = Random<float>();
    auto testU = Random<float>();
    stringValues = ToStdString<float>(testX) + " " + ToStdString<float>(testY) + " " + ToStdString<float>(testZ) + " " + ToStdString<float>(testU);
   
    intPoint = Dim3<int>::FromString(stringValues) ;
    EXPECT_FALSE(intPoint.has_value()); 
    floatPoint = Dim3<float>::FromString(stringValues) ;
    EXPECT_FALSE(floatPoint.has_value());
}
 
