#include "UIBackEnd.h"
#include "Mesh2D.h"
#include "TextBlitter.h"
#include "../AssetManagement/AssetManager.h"
#include "../BackEnd/BackEnd.h"
#include "../Core/Debug.h"
#include "../Config/Config.h"

#include "Renderer/Renderer.h"
#include "HellLogging.h"

namespace UIBackEnd {

    Mesh2D g_uiMesh;
    std::vector<Vertex2D> g_vertices;
    std::vector<uint32_t> g_indices;
    std::vector<UIRenderItem> g_renderItems;

    void Init() {
        // Export fonts, aka create spritesheets from single char files, no need to do every init but YOLO ¯\_(ツ)_/¯
        std::string name = "StandardFont";
        std::string characters = R"(!"#$%&'*+,-./0123456789:;<=>?_ABCDEFGHIJKLMNOPQRSTUVWXYZ\^_`abcdefghijklmnopqrstuvwxyz [])";
        std::string textureSourcePath = "res/fonts/raw_images/standard_font/";
        std::string outputPath = "res/fonts/";
        FontSpriteSheetPacker::Export(name, characters, 0, 1, textureSourcePath, outputPath);

        name = "AmmoFont";
        characters = "0123456789/";
        textureSourcePath = "res/fonts/raw_images/ammo_font/";
        FontSpriteSheetPacker::Export(name, characters, 0, 1, textureSourcePath, outputPath);

        name = "BebasNeue";
        characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789’,. ";
        textureSourcePath = "res/fonts/raw_images/bebas_neue/";
        FontSpriteSheetPacker::Export(name, characters, 2, 1, textureSourcePath, outputPath);

        name = "RobotoCondensed";
        characters = R"(!"#$%&'*+,-./0123456789:;<=>?_ABCDEFGHIJKLMNOPQRSTUVWXYZ\^_`abcdefghijklmnopqrstuvwxyz )";
        textureSourcePath = "res/fonts/raw_images/roboto_condensed/";
        FontSpriteSheetPacker::Export(name, characters, 1, 1, textureSourcePath, outputPath);

        // Import fonts
        FontSpriteSheet standardFont = FontSpriteSheetPacker::Import("res/fonts/StandardFont.json");
        FontSpriteSheet ammoFont = FontSpriteSheetPacker::Import("res/fonts/AmmoFont.json");
        FontSpriteSheet bebasNeue = FontSpriteSheetPacker::Import("res/fonts/BebasNeue.json");
        FontSpriteSheet robotoCondensed = FontSpriteSheetPacker::Import("res/fonts/RobotoCondensed.json");
        TextBlitter::AddFont(standardFont);
        TextBlitter::AddFont(ammoFont);
        TextBlitter::AddFont(bebasNeue);
        TextBlitter::AddFont(robotoCondensed);
    }

    void InventoryTest() {

        UIBackEnd::BlitTexture("inv2", { 745, 100 }, Alignment::TOP_LEFT, WHITE, { -1, -1 }, TextureFilter::LINEAR);

        std::string name = "GLOCK 22";
        std::string description= R"(Australian police issue. Matte and boxy, a cold
little companion. It does the paperwork duty 
without drama. Dependable at short range, 
underwhelming at a distance. A proper piece 
of shit.)";



        name = "9 X 19MM";
        description = R"(Born for Lugers, adopted by everyone. NATO's 
common tongue, cheap to feed and easy to 
stack.
)";

        name = "7.62 X 25MM";
        description = R"(Long, bottleneck case; hot, flat, and loud. 
Punches deep, too deep, through coats, doors, 
and sometimes sense.
)";

        name = "AKS74U";
        description = R"(Krinkov to some, a headache to anyone nearby. 
Built for tank hatches, stairwells, and mowing 
yer enemy like meat.
)";
    
        name = "SHOTGUN SHELLS";
        description = R"(12 gauge, plastic hulls with brass rims. They 
pattern wide and peel flesh. Heavy on the 
shoulder, honest in the work.
)";

        name = "RED DOT SIGHT";
        description = R"(A little window with a floating promise. Turns
aim into point, and point into a bloody mess.
)";

        name = "SUPPRESSOR";
        /*description = R"(A metal hush that threads onto bad intentions.
You'll hear the brass breathe before it even hits 
the floor.
)";*/
        description = R"(You'll hear the brass breathe before it even 
hits the floor.
)";




        BlitText("[COL=0.839,0.784,0.635]" + name, "BebasNeue", 800, 438, Alignment::TOP_LEFT, 1.0f, TextureFilter::LINEAR);
        BlitText("[COL=0.839,0.784,0.635]" + description, "RobotoCondensed", 800, 486, Alignment::TOP_LEFT, 1.0f, TextureFilter::LINEAR);


        UIBackEnd::BlitTexture("inventory_green_button", glm::ivec2(850, 630), Alignment::CENTERED, WHITE, glm::ivec2(-1, -1), TextureFilter::LINEAR);
        BlitText("[COL=0.839,0.784,0.635]M", "RobotoCondensed", 850, 631, Alignment::CENTERED, 0.75f, TextureFilter::LINEAR);
        BlitText("[COL=0.839,0.784,0.635]Move", "RobotoCondensed", 870, 631, Alignment::CENTERED_VERTICAL, 1.0f, TextureFilter::LINEAR);

        UIBackEnd::BlitTexture("inventory_green_button", glm::ivec2(850, 660), Alignment::CENTERED, WHITE, glm::ivec2(-1, -1), TextureFilter::LINEAR);
        BlitText("[COL=0.839,0.784,0.635]C", "RobotoCondensed", 850, 661, Alignment::CENTERED, 0.75f, TextureFilter::LINEAR);
        BlitText("[COL=0.839,0.784,0.635]Combine", "RobotoCondensed", 870, 661, Alignment::CENTERED_VERTICAL, 1.0f, TextureFilter::LINEAR);
    }

    void Update() {
        
        if (Debug::GetDebugTextMode() == DebugTextMode::GLOBAL) {
            //std::string text = "Global debug text\n... apparently it's broken right now.";
            std::string text = Debug::GetText();
            BlitText(text, "StandardFont", 0, 0, Alignment::TOP_LEFT, 2.0f);
        }

        else if (Debug::GetDebugTextMode() == DebugTextMode::PROFILING) {
            float scale = 2.0f;
            int margin = 35;

            std::string names = "\n" + Renderer::GetZoneNames();
            std::string timingsGPU = "GPU\n" + Renderer::GetZoneGPUTimings();
            std::string timingsCPU = "CPU\n" + Renderer::GetZoneCPUTimings();
            glm::ivec2 namesSize = TextBlitter::GetBlitTextSize(names, "StandardFont", scale);
            glm::ivec2 timingsGPUSize = TextBlitter::GetBlitTextSize(timingsGPU, "StandardFont", scale);
            glm::ivec2 timingsCPUSize = TextBlitter::GetBlitTextSize(timingsCPU, "StandardFont", scale);

            if (names.length() != 0) {
                timingsGPU += "\nTotal GPU: " + Renderer::GetTotalGPUTime();
                BlitText(timingsGPU, "StandardFont", 0, 0, Alignment::TOP_LEFT, scale);
                BlitText(timingsCPU, "StandardFont", timingsGPUSize.x + margin, 0, Alignment::TOP_LEFT, scale);
                BlitText(names, "StandardFont", timingsGPUSize.x + margin + timingsCPUSize.x + margin, 0, Alignment::TOP_LEFT, scale);
            }
        }

        if (BackEnd::GetAPI() == API::OPENGL) {
            g_uiMesh.GetGLMesh2D().UpdateVertexBuffer(g_vertices, g_indices);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            Logging::ToDo() << "UIBackEnd: Update()";
        }
        g_vertices.clear();
        g_indices.clear();
    }

    void EndFrame() {
        g_renderItems.clear();
    }

    void BlitText(const std::string& text, const std::string& fontName, glm::ivec2 location, Alignment alignment, float scale, TextureFilter textureFilter) {
        BlitText(text, fontName, location.x, location.y, alignment, scale, textureFilter);
    }

    void BlitText(const std::string& text, const std::string& fontName, int originX, int originY, Alignment alignment, float scale, TextureFilter textureFilter) {
        FontSpriteSheet* fontSpriteSheet = TextBlitter::GetFontSpriteSheet(fontName);
        if (!fontSpriteSheet) {
            std::cout << "UIBackEnd::BlitText() failed to find " << fontName << "\n";
            return;
        }
        int baseVertex = g_vertices.size();

        const Resolutions& resolutions = Config::GetResolutions();

        MeshData2D meshData = TextBlitter::BlitText(text, fontName, originX, originY, resolutions.ui, alignment, scale, baseVertex);
        g_vertices.insert(std::end(g_vertices), std::begin(meshData.vertices), std::end(meshData.vertices));
        g_indices.insert(std::end(g_indices), std::begin(meshData.indices), std::end(meshData.indices));

        UIRenderItem& renderItem = g_renderItems.emplace_back();
        renderItem.baseVertex = 0;
        renderItem.baseIndex = g_indices.size() - meshData.indices.size();
        renderItem.indexCount = meshData.indices.size();
        renderItem.textureIndex = AssetManager::GetTextureIndexByName(fontName); 
        renderItem.filter = (textureFilter == TextureFilter::NEAREST) ? 1 : 0;
        renderItem.clipMinX = 0;
        renderItem.clipMinY = 0;
        renderItem.clipMaxX = resolutions.ui.x;
        renderItem.clipMaxY = resolutions.ui.y;
    }


    void BlitTexture(BlitTextureInfo info) {
        BlitTexture(info.textureName, info.location, info.alignment, info.colorTint, info.size, info.textureFilter, info.rotation, info.clipMinX, info.clipMinY, info.clipMaxX, info.clipMaxY);
    }

    void BlitTexture(const std::string& textureName, glm::ivec2 location, Alignment alignment, glm::vec4 colorTint, glm::ivec2 size, TextureFilter textureFilter, float rotation, int clipMinX, int clipMinY, int clipMaxX, int clipMaxY) {
        // Bail if texture not found
        int textureIndex = AssetManager::GetTextureIndexByName(textureName);
        if (textureIndex == -1) {
            std::cout << "BlitTexture() failed. Could not find texture '" << textureName << "'\n";
            return;
        }
        // Get texture dimensions
        Texture* texture = AssetManager::GetTextureByIndex(textureIndex);
        float w = (size.x != -1) ? size.x : texture->GetWidth();
        float h = (size.y != -1) ? size.y : texture->GetHeight();

        glm::vec2 positions[4];
        glm::vec2 uvs[4] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        };

        // Alignment
        switch (alignment) {
            case Alignment::TOP_LEFT:
                positions[0] = { 0, 0 }; positions[1] = { w, 0 };
                positions[2] = { w, h }; positions[3] = { 0, h };
                break;
            case Alignment::TOP_RIGHT:
                positions[0] = { -w, 0 }; positions[1] = { 0, 0 };
                positions[2] = { 0, h }; positions[3] = { -w, h };
                break;
            case Alignment::BOTTOM_LEFT:
                positions[0] = { 0, -h }; positions[1] = { w, -h };
                positions[2] = { w, 0 }; positions[3] = { 0, 0 };
                break;
            case Alignment::BOTTOM_RIGHT:
                positions[0] = { -w, -h }; positions[1] = { 0, -h };
                positions[2] = { 0, 0 }; positions[3] = { -w, 0 };
                break;
            case Alignment::CENTERED:
                positions[0] = { -w * 0.5f, -h * 0.5f }; positions[1] = { w * 0.5f, -h * 0.5f };
                positions[2] = { w * 0.5f,  h * 0.5f }; positions[3] = { -w * 0.5f,  h * 0.5f };
                break;
            case Alignment::CENTERED_HORIZONTAL:
                positions[0] = { -w * 0.5f, 0 }; positions[1] = { w * 0.5f, 0 };
                positions[2] = { w * 0.5f,  h }; positions[3] = { -w * 0.5f,  h };
                break;
            case Alignment::CENTERED_VERTICAL:
                positions[0] = { 0, -h * 0.5f }; positions[1] = { w, -h * 0.5f };
                positions[2] = { w, h * 0.5f }; positions[3] = { 0, h * 0.5f };
                break;
            default:
                return;
        };

        // Rotation
        float s = sin(rotation);
        float c = cos(rotation);
        for (int i = 0; i < 4; ++i) {
            float newX = positions[i].x * c - positions[i].y * s;
            float newY = positions[i].x * s + positions[i].y * c;
            positions[i] = { newX, newY };
        }
        
        // Snap to integer pixels
        glm::vec2 anchor = glm::round(glm::vec2(location)); 

        // Convert the final screen position to NDC
        const Resolutions& resolutions = Config::GetResolutions();
        glm::vec2 finalVertices[4];
        for (int i = 0; i < 4; ++i) {
            glm::vec2 screenPos = glm::vec2(anchor) + positions[i];
            finalVertices[i].x = (screenPos.x / static_cast<float>(resolutions.ui.x)) * 2.0f - 1.0f;
            finalVertices[i].y = 1.0f - (screenPos.y / static_cast<float>(resolutions.ui.y)) * 2.0f;
        }

        int baseVertex = g_vertices.size();
        g_vertices.reserve(baseVertex + 4);
        g_vertices.push_back({ { finalVertices[0].x, finalVertices[0].y }, uvs[0], colorTint, textureIndex });
        g_vertices.push_back({ { finalVertices[1].x, finalVertices[1].y }, uvs[1], colorTint, textureIndex });
        g_vertices.push_back({ { finalVertices[2].x, finalVertices[2].y }, uvs[2], colorTint, textureIndex });
        g_vertices.push_back({ { finalVertices[3].x, finalVertices[3].y }, uvs[3], colorTint, textureIndex });

        int baseIndex = g_indices.size();
        g_indices.reserve(baseIndex + 6);
        g_indices.push_back(baseVertex + 0);
        g_indices.push_back(baseVertex + 1);
        g_indices.push_back(baseVertex + 2);
        g_indices.push_back(baseVertex + 0);
        g_indices.push_back(baseVertex + 2);
        g_indices.push_back(baseVertex + 3);

        UIRenderItem& renderItem = g_renderItems.emplace_back();
        renderItem.baseVertex = 0;
        renderItem.baseIndex = baseIndex;
        renderItem.indexCount = 6;
        renderItem.textureIndex = textureIndex;
        renderItem.filter = (textureFilter == TextureFilter::NEAREST) ? 1 : 0;
        renderItem.clipMinX = clipMinX;
        renderItem.clipMinY = clipMinY;
        renderItem.clipMaxX = clipMaxX;
        renderItem.clipMaxY = clipMaxY;

        // Maybe tidy this up later
        const int W = BackEnd::GetCurrentWindowWidth();
        const int H = BackEnd::GetCurrentWindowHeight();

        renderItem.clipMinX = (clipMinX >= 0) ? clipMinX : 0;
        renderItem.clipMinY = (clipMinY >= 0) ? clipMinY : 0;
        renderItem.clipMaxX = (clipMaxX >= 0) ? clipMaxX : W;
        renderItem.clipMaxY = (clipMaxY >= 0) ? clipMaxY : H;
    }

    Mesh2D& GetUIMesh() {
        return g_uiMesh;
    }

    std::vector<UIRenderItem>& GetRenderItems() {
        return g_renderItems;
    }
}