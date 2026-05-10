#ifdef PIO_UNIT_TESTING

#include <gtest/gtest.h>
#include "ClutchPaddleTest.h"
#include "CanControllerTest.h"
#include "MainTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
