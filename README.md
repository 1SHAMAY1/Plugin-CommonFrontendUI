# Common Frontend UI for Unreal Engine 5

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.3%2B%20|%205.8%2B-blue?logo=unrealengine&logoColor=white)](https://www.unrealengine.com/)
[![Pre-Compiled](https://img.shields.io/badge/Binaries-Win64%20Included-brightgreen)](README.md)
[![User Guide](https://img.shields.io/badge/Documentation-User%20Guide%20Available-gold)](USER_GUIDE.md)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Author](https://img.shields.io/badge/Author-Shankharaj%20Datta-blue?logo=linkedin)](https://www.linkedin.com/in/shankharaj-datta-45921a28a/)
[![Dependency](https://img.shields.io/badge/Plugin-CommonUI%20Enabled-orange)](README.md)

**Common Frontend UI** is a modular, production-ready frontend framework for Unreal Engine 5 (UE 5.3+ / 5.8+). Built on Epic's **CommonUI** and **Enhanced Input** systems, it delivers a decoupled, multi-platform UI architecture featuring a unified Settings Registry, interactive Key Remapping, Activatable Widget Stacks, and a dedicated Loading Screen subsystem.

> [!TIP]
> **Blueprint Ready**: Pre-compiled Win64 editor binaries (`.dll` + `.modules`) are included. Simply clone or copy `CommonFrontendUI` into your project's `Plugins/` directory.

---

## 📖 Documentation & User Guide

For step-by-step setup instructions, Blueprint node walkthroughs, settings registry examples, and architecture references:

👉 **[Read the Complete User Guide & Developer Manual](USER_GUIDE.md)**

### What's Covered in the Guide:
1. **[Architecture Overview](USER_GUIDE.md#1-architecture-overview)**: Subsystems, primary layouts, and stack-based flow.
2. **[5-Step Quick Start](USER_GUIDE.md#3-quick-start-5-step-setup)**: Getting a complete frontend running in under 5 minutes.
3. **[Gameplay Tags Reference](USER_GUIDE.md#4-gameplay-tag-system)**: Standardized tags for HUD, Menu, Frontend, and Modal layers.
4. **[Settings Registry Deep Dive](USER_GUIDE.md#5-settings-registry-system)**: Configuring audio, video, graphics sliders, toggles, and dropdowns with edit conditions.
5. **[Key Remapping](USER_GUIDE.md#6-key-remapping-system)**: Dynamic Enhanced Input rebinding for Gamepad and Keyboard.
6. **[Modal Choice Dialogues](USER_GUIDE.md#7-modal-choice-dialogues)**: Asynchronous confirmation and choice popups.
7. **[Loading Screen Subsystem](USER_GUIDE.md#8-loading-screen-subsystem)**: Zero-hitch Slate map transitions.

---

## 🚀 Key Modules & Capabilities

| Module | Core Functionality |
| :--- | :--- |
| **CommonFrontendUI** | Core UI subsystem, activatable widget stacks, choice dialogues, and settings registry. |
| **CommonFrontendUI_LoadingScreen** | Seamless background loading screens with custom Slate interface implementations. |

### Architectural Highlights
- **Settings Registry**: Data-driven options system integrating directly with `UGameUserSettings` for video, audio, and gameplay preferences.
- **Key Remapping Screen**: Native rebinding interface built for `EnhancedInput` with gamepad and keyboard support.
- **Activatable Widget Stack**: Async latent actions (`PushFrontendWidget`, `PushChoiceScreen`) managing layered menu hierarchies without hard references.
- **Loading Subsystem**: Independent loading state machine preventing frame hitching during map travel.

---

## 🏛️ System Overview

```mermaid
flowchart TD
    A[Game Instance / Player Controller] --> B[UCommonFrontendUI_Subsystem]
    B --> C{Primary Layout Stack}
    C --> D[Main Menu Screen]
    C --> E[Options & Settings Registry]
    C --> F[Key Remapping Screen]
    C --> G[Choice & Confirmation Modals]

    H[Map Travel / Level Stream] --> I[UCommonFrontendLoadingScreenSubsystem]
    I --> J[Slate Loading Screen Interface]
```

---

## 📋 Installation & Setup

1. Clone or copy `CommonFrontendUI` into your project's `Plugins/` directory:
   ```text
   YourProject/
   └── Plugins/
       └── CommonFrontendUI/
           ├── CommonFrontendUI.uplugin
           ├── Binaries/Win64/
           ├── Content/
           ├── Resources/
           ├── Source/
           ├── README.md
           └── USER_GUIDE.md
   ```
2. Enable the **CommonUI** plugin in **Edit > Plugins**.
3. Enable **CommonFrontendUI** and restart the editor.
4. Explore the included demo in `Content/Map/L_Demo.umap`.

---

## 📄 License & Attribution

Distributed under the **MIT License**.

Created by **[Shankharaj Datta](https://www.linkedin.com/in/shankharaj-datta-45921a28a/)**.
