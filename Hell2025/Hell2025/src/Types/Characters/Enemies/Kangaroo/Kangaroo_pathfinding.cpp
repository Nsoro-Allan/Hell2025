#include "Kangaroo.h"
#include "Core/Game.h"
#include "Pathfinding/AStarMap.h"

void Kangaroo::FindPathToTarget() {
    Player* player = Game::GetLocalPlayerByIndex(0);;
    glm::vec3 playerPosition = player->GetCameraPosition();

    glm::ivec2 start = AStarMap::GetCellCoordsFromWorldSpacePosition(m_position);
    glm::ivec2 end = AStarMap::GetCellCoordsFromWorldSpacePosition(playerPosition);
    m_aStar.InitSearch(start.x, start.y, end.x, end.y);
    m_aStar.FindPath();
}