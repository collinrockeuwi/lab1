#include "unity.h"
#include "calc.h"

void setUp(void) {}
void tearDown(void) {}

void test_add(void) {
    TEST_ASSERT_EQUAL(12, add(10, 2));
    TEST_ASSERT_EQUAL(300, add(200, 100)); // catch 8-bit overflow
}

void test_is_equal(void) {
    TEST_ASSERT_EQUAL(1, add(1, 0) == 1);
}

void test_power(void) {
    TEST_ASSERT_EQUAL(100, power(10, 2));
    TEST_ASSERT_EQUAL(1000, power(10, 3));
}

void test_magnitude(void) {
    TEST_ASSERT_EQUAL(5, magnitude(-5));
    TEST_ASSERT_EQUAL(5, magnitude(5));
    TEST_ASSERT_EQUAL(0, magnitude(0));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    RUN_TEST(test_is_equal);
    RUN_TEST(test_power);
    RUN_TEST(test_magnitude);
    return UNITY_END();
}
