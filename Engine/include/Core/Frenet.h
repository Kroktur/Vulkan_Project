#pragma once

#include <vector>
#include <glm/glm.hpp>

struct CurveFrame
{
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;
};

namespace Frenet
{
    // Takes the frame at point 'from' and transports it to point 'to'.
    // Uses two reflections to rotate the frame as little as possible (Wang et al. 2008).
    CurveFrame TransportFrame(const CurveFrame& previousFrame,
        const glm::vec3& from,
        const glm::vec3& to,
        const glm::vec3& nextForward);

    // Builds one CurveFrame for every point along the curve.
    // points and forwardDirs must have the same size (>= 2).
    std::vector<CurveFrame> BuildFrames(const std::vector<glm::vec3>& points,
        const std::vector<glm::vec3>& forwardDirs);

    // Estimates the forward direction at each point using finite differences.
    // End points use a one-sided difference; middle points use a centered difference.
    std::vector<glm::vec3> EstimateForwardDirs(const std::vector<glm::vec3>& points);
}