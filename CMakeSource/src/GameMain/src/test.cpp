#include "pch.h"
//#include <Rice/Tests/HelloTriangle.hpp>
#include <Rice/Tests/QuadTest.hpp>
#include <Rice/Tests/CoreTest.hpp>

int main() {
    //Rice::Tests::HelloTriangle::runTest();
	Rice::Tests::QuadTest::runTest();
    Rice::Tests::CoreTest::runTest();
	return 0;
}
