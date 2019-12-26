#include "catch.hpp"
#include "../exread/bigint.hpp"

using namespace exread;

TEST_CASE( "operator ==,!=", "[BigInt]" ) {

    SECTION( "default" ) {
        BigInt i;
        REQUIRE(i ==  0);
        REQUIRE(i !=  1);
        REQUIRE(i != -1);

        REQUIRE( !(i !=  0) );
        REQUIRE( !(i ==  1) );
    }

    SECTION( "from unsigned int" ) {
        BigInt i(10u);
        REQUIRE(i ==  10);
        REQUIRE(i != -10);
        REQUIRE(i !=   0);
        REQUIRE(i !=   1);
        REQUIRE(i !=  -1);
    }

    SECTION( "from long long int" ) {
        const long long int n = 4536546846465102347627863786783823567;
        const BigInt i(n);
        REQUIRE(i ==  n);
        REQUIRE(i != -n);
        REQUIRE(i !=  0);
        REQUIRE(i !=  1);
        REQUIRE(i != -1);
    }

}

TEST_CASE( "operator >=", "[BigInt]" ) {

    const BigInt i1( 100);
    const BigInt i2(  99);
    const BigInt i3(-100);
    const BigInt i4(- 99);

    // + >= +
    REQUIRE(i1 >= i2);

    // + >= -
    REQUIRE(i1 >= i3);
    REQUIRE(i1 >= i4);
    REQUIRE(i2 >= i3);
    REQUIRE(i2 >= i4);

    // - >= -
    REQUIRE(i4 >= i3);

    // not - >= +
    REQUIRE( !(i3 >= i2) );

}

TEST_CASE( "unary arithmetic operators", "[BigInt]" ) {

    const long long int n = -89368532769232346342343406;
    const BigInt i(n);

    SECTION( "operator +") {
        REQUIRE( (+i) == i );
        REQUIRE( (+i) == n );
    }

    SECTION( "operator -") {
        REQUIRE( (-i) == (-i) );
        REQUIRE( (-i) != (+i) );
        REQUIRE( (-i) == (-n) );
    }

}

TEST_CASE( "binary arithmetic operators", "[BigInt]" ) {

    const long long int n1 = 69232346342343406;
    const BigInt i1(n1);

    const long long int n2 =            812345;
    const BigInt i2(n2);

    const long long int n3 = 69232346343155751;
    const BigInt i3(n3);

    const long long int n4 =            512310;
    const BigInt i4(n4);

    const long long int n5 =             65535;
    const BigInt i5(n5);

    const long long int n6 = 69232346343090216;
    const BigInt i6(n6);

    const BigInt i7 = BigInt(45371418175)*100000000000 + 94417305560;

    SECTION( "operator +") {
        REQUIRE( i1 + i2 == i3 );
        REQUIRE( i2 + i1 == i3 );
        REQUIRE( i5 + i6 == i3 );
        REQUIRE( i6 + i5 == i3 );
        REQUIRE( i5 + i5 == 2*n5 );
    }

    SECTION( "operator -") {
        REQUIRE( i1 - i1 == 0 );
        REQUIRE( i3 - (i2 + i1) == 0 );
        REQUIRE( i3 - i1 - i2 == 0 );
        REQUIRE( i2 - i4 == 300035 );
        REQUIRE( i3 - i2 - i1 == 0 );
        REQUIRE( i3 - i2 == i1 );
        REQUIRE( i3 - i1 == i2 );
        REQUIRE( i2 - i3 == -i1 );
        REQUIRE( i5 - i3 == -i6 );
        REQUIRE( i6 - i3 == -i5 );
        REQUIRE(-i5 + i3 ==  i6 );
        REQUIRE(-i3 + i5 == -i6 );
    }

    SECTION( "operator *") {
        REQUIRE(   i5  *   i6  ==  i7 );
        REQUIRE(   i6  *   i5  ==  i7 );
        REQUIRE( (-i6) *   i5  == -i7 );
        REQUIRE(   i6  * (-i5) == -i7 );
        REQUIRE( (-i6) * (-i5) ==  i7 );

        REQUIRE(   BigInt(0) * i7        == 0 );
        REQUIRE(   i7        * BigInt(0) == 0 );
        REQUIRE( (-i7)       * BigInt(0) == 0 );
    }

}

}
