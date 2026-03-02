#include "Core/Frenet.h"
#include <stdexcept>

namespace Frenet
{
    CurveFrame TransportFrame(const CurveFrame& previousFrame,
        const glm::vec3& from,
        const glm::vec3& to,
        const glm::vec3& nextForward)
    {
        // Reflection 1: slide the frame along the segment from -> to
        glm::vec3 segmentDir = to - from;
        float     segmentSqLen = glm::dot(segmentDir, segmentDir); // squared length, used to normalize

        // Reflection formula -> forward and up across the plane perpendicular to segmentDir.
        glm::vec3 reflForward = previousFrame.forward - (2.0f / segmentSqLen) * glm::dot(segmentDir, previousFrame.forward) * segmentDir;
        glm::vec3 reflUp = previousFrame.up - (2.0f / segmentSqLen) * glm::dot(segmentDir, previousFrame.up) * segmentDir;

        // Reflection 2: align reflForward onto the actual next forward direction
        glm::vec3 correctionAxis = nextForward - reflForward;
        float     correctionSqLen = glm::dot(correctionAxis, correctionAxis);

        // If correctionSqLen is near zero, reflForward already matches nextForward,
        // so no correction is needed (and we avoid a division by zero).
        glm::vec3 transportedUp = (correctionSqLen < 1e-10f)
            ? reflUp
            : reflUp - (2.0f / correctionSqLen) * glm::dot(correctionAxis, reflUp) * correctionAxis;

        CurveFrame nextFrame;
        nextFrame.forward = nextForward;
        nextFrame.up = glm::normalize(transportedUp);
        nextFrame.right = glm::normalize(glm::cross(nextFrame.forward, nextFrame.up));
        return nextFrame;
    }

    std::vector<glm::vec3> EstimateForwardDirs(const std::vector<glm::vec3>& points)
    {
        const std::size_t pointCount = points.size();
        if (pointCount < 2)
            throw std::invalid_argument("Frenet::EstimateForwardDirs : at least 2 points required");

        std::vector<glm::vec3> forwardDirs(pointCount);

        // End points only have one neighbour, so we use a simple one-sided difference.
        forwardDirs[0] = glm::normalize(points[1] - points[0]);
        forwardDirs[pointCount - 1] = glm::normalize(points[pointCount - 1] - points[pointCount - 2]);

        // Middle points use a centered difference (looks at both neighbours, more accurate).
        for (std::size_t i = 1; i < pointCount - 1; ++i)
            forwardDirs[i] = glm::normalize(points[i + 1] - points[i - 1]);

        return forwardDirs;
    }

    std::vector<CurveFrame> BuildFrames(const std::vector<glm::vec3>& points,
        const std::vector<glm::vec3>& forwardDirs)
    {
        const std::size_t pointCount = points.size();
        if (pointCount < 2 || forwardDirs.size() != pointCount)
            throw std::invalid_argument("Frenet::BuildFrames : points and forwardDirs must have the same size (>= 2)");

        std::vector<CurveFrame> frames(pointCount);

        // First frame: we know forward, but up cannot be deduced from the curve yet.
        // We pick a world axis and make it perpendicular to forward using Gram-Schmidt:
        // subtract the component that is parallel to forward, leaving only the perpendicular part.
        {
            const glm::vec3& firstForward = forwardDirs[0];

            // Avoid the X axis if forward is nearly parallel to it,
            // because cross(X, X) would produce a near-zero vector.
            glm::vec3 worldAxis = (std::abs(firstForward.x) <= 0.9f) ? glm::vec3(1, 0, 0)
                : glm::vec3(0, 1, 0);

            // Gram-Schmidt: remove the part of worldAxis that points in the same direction as forward.
            glm::vec3 firstUp = glm::normalize(worldAxis - glm::dot(worldAxis, firstForward) * firstForward);

            frames[0].forward = firstForward;
            frames[0].up = firstUp;
            frames[0].right = glm::normalize(glm::cross(firstForward, firstUp));
        }

        // All other frames: transport each frame to the next point using double reflection
        for (std::size_t i = 0; i < pointCount - 1; ++i)
            frames[i + 1] = TransportFrame(frames[i], points[i], points[i + 1], forwardDirs[i + 1]);

        return frames;
    }
}