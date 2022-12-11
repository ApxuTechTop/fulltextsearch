#include <gtest/gtest.h>
#include <libmath/math.hpp>

TEST(MathTests, SumTest) {
	const double a = 1.0;
	const double b = 13.0 / 7;
	ASSERT_EQ(math::sum(a, b), a + b);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
