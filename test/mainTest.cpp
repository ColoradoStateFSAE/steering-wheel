#ifdef NATIVE

#include <gtest/gtest.h>
#include "AnalogInputTest.h"
#include "ButtonTest.h"
#include "CanTest.h"
#include "MainTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
