#include "Player.h"
#include "AssetManagement/AssetManager.h"
#include "Config/Config.h"
#include "Core/Debug.h"
#include "Editor/Editor.h"
#include "Util/Util.h"
#include "UI/TextBlitter.h"
#include "UI/UiBackend.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"
#include "Input/Input.h"

#include "API/OpenGL/Renderer/GL_renderer.h"
#include "UniqueID.h"

void Player::UpdateUI() {
    if (Editor::IsOpen()) return;

    const Viewport* viewport = ViewportManager::GetViewportByIndex(m_viewportIndex);
    if (!viewport->IsVisible()) return;

    const Resolutions& resolutions = Config::GetResolutions();
    int width = resolutions.ui.x * viewport->GetSize().x;
    int height = resolutions.ui.y * viewport->GetSize().y;
    int xLeft = resolutions.ui.x * viewport->GetPosition().x;
    int xRight = xLeft + width;
    int yTop = resolutions.ui.y * (1.0f - viewport->GetPosition().y - viewport->GetSize().y);
    int yBottom = yTop + height;
    int centerX = xLeft + (width / 2);
    int centerY = yTop + (height / 2);
    int ammoX = xRight - (width * 0.17f);
    int ammoY = yBottom - (height * 0.145f) - TextBlitter::GetFontSpriteSheet("AmmoFont")->m_charHeight - -TextBlitter::GetFontSpriteSheet("AmmoFont")->m_lineSpacing;

    // Info text
    int infoTextX = xLeft + (width * 0.1f);
    int infoTextY = ammoY;

    if (m_inventory.IsOpen()) {
        m_inventory.SubmitRenderItems();
    }


    //std::string text = "Locked...";
    //UIBackEnd::BlitText("[COL=0.839,0.784,0.635]" + text, "RobotoCondensed", 150, 1080 - 150, Alignment::TOP_LEFT, 2.0f, TextureFilter::LINEAR);



//   glm::ivec2 location = glm::ivec2(centerX, centerY);
//   location = glm::ivec2(64, yTop + 64);
//   //glm::ivec2 size = glm::ivec2(-1, -1);
//
//   std::string texName = "inv10";
//   if (Input::KeyDown(HELL_KEY_Y)) {
//       texName = "inv11";
//   }
//   Texture* texture = AssetManager::GetTextureByName(texName);
//   //glm::ivec2 size = glm::ivec2(-1, -1);
//   glm::ivec2 size = glm::ivec2(texture->GetWidth(0), texture->GetHeight(0));
//
//   UIBackEnd::BlitTexture(texName, location, Alignment::TOP_LEFT, WHITE, size, TextureFilter::NEAREST);
//
//   location = glm::ivec2(750, yTop + 64);
//   //UIBackEnd::BlitTexture("inv2", location, Alignment::TOP_LEFT, WHITE, size, TextureFilter::LINEAR);
//
    

    if (IsAlive()) {
        // Cross hair texture
        std::string crosshairTexture = "CrosshairDot";
        if (m_interactFound) {
            crosshairTexture = "CrosshairSquare";
        }

        UIBackEnd::BlitText(m_infoText, "StandardFont", infoTextX, infoTextY, Alignment::TOP_LEFT, 2.0f);
        UIBackEnd::BlitTexture(crosshairTexture, glm::ivec2(centerX, centerY), Alignment::CENTERED, WHITE, glm::ivec2(128, 128));

        // Ammo
        if (GetCurrentWeaponType() != WeaponType::MELEE) {
            float scale = 1.3f;
            float smallScale = scale * 0.8f;

            int slashPadding = 10;
            std::string clipText = std::to_string(GetCurrentWeaponMagAmmo());
            std::string totalText = std::to_string(GetCurrentWeaponTotalAmmo());

            // Mag ammo color
            if (GetCurrentWeaponMagAmmo() == 0) {
                clipText = "[COL=0.8,0.05,0.05,1]" + clipText;
            }
            else {
                clipText = "[COL=0.16,0.78,0.23,1]" + clipText;
            }

            UIBackEnd::BlitText(clipText, "AmmoFont", ammoX - slashPadding, ammoY, Alignment::TOP_RIGHT, scale, TextureFilter::LINEAR);
            UIBackEnd::BlitText("/", "AmmoFont", ammoX, ammoY, Alignment::CENTERED_HORIZONTAL, scale, TextureFilter::LINEAR);
            UIBackEnd::BlitText(totalText, "AmmoFont", ammoX + slashPadding, ammoY, Alignment::TOP_LEFT, smallScale, TextureFilter::LINEAR);

            // SPAS AUTO
            WeaponState* weaponState = GetCurrentWeaponState();
            WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
            if (weaponInfo->hasAutoSwitch) {
                Texture* texture = AssetManager::GetTextureByName("Weapon_Auto");
                if (GetCurrentWeaponType() == WeaponType::SHOTGUN && texture) {
                    int modifierPadding = 29;
                    int modifierX = TextBlitter::GetBlitTextSize(totalText, "AmmoFont", smallScale).x + modifierPadding;
                    int gridSize = 10;
                    modifierX = (modifierX / 10) * 10;
                    int modifierScaleX = texture->GetWidth() * smallScale;
                    int modifierScaleY = texture->GetHeight() * smallScale;
                    glm::vec4 unselectedColor = glm::vec4(0.541, 0.51, 0.392, 0.5f);
                    glm::vec4 colorAuto = weaponState->shotgunInAutoMode ? WHITE : unselectedColor;
                    glm::vec4 colorPump = weaponState->shotgunInAutoMode ? unselectedColor : WHITE;
                    float padding = 4;
                    float autoY = ammoY;
                    float pumpY = autoY + ((texture->GetHeight() + padding) * smallScale);

                    int negHack = -9 + 9;
                    int hack = 7 + 9;

                    std::string shellTextureName = "ShotgunShellRed";
                    if (weaponState->shotgunSlug) {
                        shellTextureName = "ShotgunShellGreen";
                    }
                    Texture* texture = AssetManager::GetTextureByName(shellTextureName);
                    int shellScaleX = texture->GetWidth() * smallScale;
                    int shellScaleY = texture->GetHeight() * smallScale * 1.1f;

                    UIBackEnd::BlitTexture(shellTextureName, glm::ivec2(ammoX + modifierX + negHack, autoY), Alignment::TOP_LEFT, WHITE, glm::ivec2(shellScaleX, shellScaleY), TextureFilter::LINEAR);
                    UIBackEnd::BlitTexture("Weapon_Auto", glm::ivec2(ammoX + modifierX + hack, autoY), Alignment::TOP_LEFT, colorAuto, glm::ivec2(modifierScaleX, modifierScaleY), TextureFilter::LINEAR);
                    UIBackEnd::BlitTexture("Weapon_Pump", glm::ivec2(ammoX + modifierX + hack, pumpY), Alignment::TOP_LEFT, colorPump, glm::ivec2(modifierScaleX, modifierScaleY), TextureFilter::LINEAR);
                }
            }
        }

        if (Debug::GetDebugTextMode() == DebugTextMode::PER_PLAYER || Debug::GetDebugRenderMode() == DebugRenderMode::BVH_CPU_PLAYER_RAYS) {

            std::string text = "";
            text += "Feet Pos: " + Util::Vec3ToString(GetFootPosition()) + "\n";
            text += "Cam Pos: " + Util::Vec3ToString(GetCameraPosition()) + "\n";
            text += "Cam Euler: " + Util::Vec3ToString(GetCameraRotation()) + "\n";

            // Kangaroos
            if (false) {
                if (World::GetKangaroos().size()) {
                    Kangaroo& kangaroo = World::GetKangaroos()[0];
                    text += kangaroo.GetDebugInfoString();
                }
            }

            // Inventory
            if (false) {
                text += "Inventory State: " + Util::InventoryStateToString(m_inventory.GetInventoryState()) + "\n";
                for (auto item : m_inventory.GetItems()) {
                    text += "- " + item.m_name;
                    text += " [" + std::to_string(item.m_gridLocation.x) +"][" + std::to_string(item.m_gridLocation.y) + "]";
                    text += "\n";
                }
            }

            // Movement
            if (false) {
                text += "IsMoving: " + Util::BoolToString(IsMoving()) + "\n";
            }

            // Weapons
            if (true) {
                text += "Weapon Action: " + Util::WeaponActionToString(GetCurrentWeaponAction()) + "\n";
            }

            // Interact
            //if (false) {
            //    text += "Interact object: " + Util::ObjectTypeToString(m_interactObjectId) + " " + std::to_string(m_interactObjectId) + "\n";
            //}

            // Rays
            //if (true) {
            //    text += "BVH ray: " + Util::ObjectTypeToString(UniqueID::GetType(m_bvhRayResult.objectId)) + " " + std::to_string(m_bvhRayResult.objectId) + "\n";
            //    text += "PhysX ray: " + Util::ObjectTypeToString(m_physXRayResult.userData.objectType) + " " + std::to_string(m_physXRayResult.userData.objectId) + " " + Util::PhysicsTypeToString(m_physXRayResult.userData.physicsType) + " " + std::to_string(m_physXRayResult.userData.physicsId) + "\n";
            //    text += "Ray hit found: " + Util::BoolToString(m_rayHitFound) + " " + Util::ObjectTypeToString(m_rayHitObjectType) + " " + std::to_string(m_rayhitObjectId) + "\n";
            //    text += "Feet above height field: " + Util::BoolToString(m_feetAboveHeightField) + "\n";
            //}

            // Movement
            if (false) {
                text += "Movement Dir: " + Util::Vec3ToString(m_movementDirection) + "\n";
                text += "Acceleration: " + std::to_string(m_acceleration) + "\n";
                text += "Y Velocity: " + std::to_string(m_yVelocity) + "\n";
            }

            // Lights
            if (false) {
                for (Light& Light : World::GetLights()) {
                    text += "Light: " + Util::BoolToString(Light.IsDirty()) + "\n";
                }
            }

            // Physx Object Count
            if (false) {
                text += "\n";
                text += Physics::GetObjectCountsAsString();
                text += "\n";
            }

            // Shark
            if (false) {
                for (Shark& shark : World::GetSharks()) {
                    text += shark.GetDebugInfoAsString();
                }
            }

            glm::vec3 rayOrigin = GetCameraPosition();
            glm::vec3 rayDir = GetCameraForward();
            float maxRayDistance = 100.0f;

            text += "\n";
            text += "Flip normal map Y: " + Util::BoolToString(OpenGLRenderer::ShouldFlipNormalMapY()) + "\n";

            // Override with BVH CPU RAYS if that render mode is set
            if (Debug::GetDebugRenderMode() == DebugRenderMode::BVH_CPU_PLAYER_RAYS) {
                text = "BVH ray hit: " + Util::BoolToString(m_bvhRayResult.hitFound) + "\n";

                if (m_bvhRayResult.hitFound) {
                    uint64_t hitId = m_bvhRayResult.objectId;
                    ObjectType hitType = UniqueID::GetType(hitId);
                    text += "- Hit pos: " + Util::Vec3ToString(m_bvhRayResult.hitPosition) + "\n";
                    text += "- Parent type: " + Util::ObjectTypeToString(hitType) + "\n";
                    text += "- Mesh name: " + AssetManager::GetMeshNameByMeshIndex(m_bvhRayResult.globalMeshIndex) + "\n";
                    text += "- Parent Id: " + std::to_string(UniqueID::GetLocal(m_bvhRayResult.objectId)) + "\n";
                    text += "- Openable Id: " + std::to_string(m_bvhRayResult.openableId) + "\n";
                    text += "- Custom Id: " + std::to_string(m_bvhRayResult.customId) + "\n";
                }
            }


            UIBackEnd::BlitText(text, "StandardFont", xLeft, yTop, Alignment::TOP_LEFT, 2.0f);
        }
    
    }
    
    // Press Start
    if (RespawnAllowed()) {
        static Texture* texture = AssetManager::GetTextureByName("PressStart");
        if (texture) {
            static int width = texture->GetWidth() * 2;
            static int height = texture->GetHeight() * 2;
            glm::ivec2 location = glm::ivec2(centerX, centerY);
            glm::ivec2 size = glm::ivec2(width, height);
            UIBackEnd::BlitTexture("PressStart", location, Alignment::CENTERED, WHITE, size, TextureFilter::LINEAR);
        }
    }
}