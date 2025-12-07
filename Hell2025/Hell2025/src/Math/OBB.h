#pragma once
#include "AABB.h"
#include <glm/glm.hpp>
#include <vector>

struct OBB {
	OBB() = default;
	OBB(const AABB& localBounds, const glm::mat4& worldMatrix);

	void SetTransform(const glm::mat4& worldMatrix);
	void SetLocalBounds(const AABB& localBounds);

	const AABB& GetLocalBounds() const               { return m_localBounds; }
	const glm::mat4& GetWorldTransform() const       { return m_worldTransform; }
	const std::vector<glm::vec3>& GetCorners() const { return m_corners; }

private:
	void RecomputeCorners();

	AABB m_localBounds;
	glm::mat4 m_worldTransform = glm::mat4(1.0f);
	std::vector<glm::vec3> m_corners;
};