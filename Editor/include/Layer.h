#pragma once

#include "imgui.h"

namespace KGR
{
    namespace Editor
    {
        class Layer
        {
        public:
            // Call once, right after ImGui::CreateContext() and before the first frame.
            static void ApplyStyle();

            // Call at the very beginning of each frame, before any panel Render().
            // It creates an invisible fullscreen window that acts as the docking host,
            // then builds the default dock layout the first time it runs.
            static void BeginDockspace();

            // Call after all panels have been rendered.
            static void EndDockspace();

        private:
            static void BuildDefaultLayout(ImGuiID dockspaceId);

            static bool m_layoutBuilt;
        };
    }
}
