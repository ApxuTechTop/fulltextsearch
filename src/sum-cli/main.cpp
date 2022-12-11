#include <cxxopts.hpp>

#include <iostream>

#include <libmath/math.hpp>

int main(int argc, char **argv) {
	cxxopts::Options options("MySum", "Summing two numbers");
	options.add_options()("f,first", "First number", cxxopts::value<double>())(
		"s,second", "Second number", cxxopts::value<double>());
	auto result = options.parse(argc, argv);
	double left = result["first"].as<double>();
	double right = result["second"].as<double>();
	double answer = math::sum(left, right);
	std::cout << answer << std::endl;
	return 0;
}