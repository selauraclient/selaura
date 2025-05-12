#pragma once

#include "../../core/math/vec3.hpp"

namespace selaura::bedrock {
	struct StateVectorComponent {
		Vec3 mPos;
		Vec3 mPosPrev;
		Vec3 mPosDelta;
	};
};
