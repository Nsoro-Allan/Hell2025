#include "Shark.h"
#include "Util.h"

std::string Shark::GetDebugInfoAsString() {
    std::string result = "\nShark\n";
    result += "- Movement State: " + Util::SharkMovementStateToString(m_movementState) + "\n";
    result += "- Hunting State: " + Util::SharkHuntingStateToString(m_huntingState) + "\n";
    result += "- Hunted player ID: " + std::to_string(m_huntedPlayerId) + "\n";
    result += "- Forward: " + Util::Vec3ToString(m_forward) + "\n";
    result += "- Left: " + Util::Vec3ToString(m_left) + "\n";
    result += "- Right: " + Util::Vec3ToString(m_right) + "\n";
    result += "- Target Position: " + Util::Vec3ToString(m_targetPosition) + "\n";
    result += "- Last Known Target Position: " + Util::Vec3ToString(m_lastKnownTargetPosition) + "\n";
    result += "- Path size: " + std::to_string(m_path.size()) + "\n";
    result += "- Next path point index: " + std::to_string(m_nextPathPointIndex) + "\n";
    return result;
}

SharkHuntingState m_huntingState = SharkHuntingState::UNDEFINED;
SharkMovementState m_movementState = SharkMovementState::FOLLOWING_PATH;
