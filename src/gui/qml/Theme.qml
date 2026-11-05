pragma Singleton
import QtQuick
import CppForge

Item {
    id: theme

    // Synchronized with C++ ThemeController (persisted via QSettings)
    readonly property bool isDark: ThemeController.isDark

    function toggleTheme() {
        ThemeController.toggleTheme()
    }

    function setDark(val) {
        ThemeController.isDark = val
    }

    // Dynamic logo depending on theme (transparent in both light and dark)
    readonly property string logoPath: isDark ? "qrc:/icons/main_logo_dark.ico" : "qrc:/icons/main_logo_transparent.png"

    // Background surfaces (KDE Breeze design language)
    readonly property color bgApp: isDark ? "#232629" : "#eff0f1"
    readonly property color bgSidebar: isDark ? "#2a2e32" : "#fcfcfc"
    readonly property color bgCard: isDark ? "#31363b" : "#ffffff"
    readonly property color bgCardHover: isDark ? "#3a3f44" : "#f5f6f7"
    readonly property color bgInput: isDark ? "#1b1e20" : "#ffffff"
    readonly property color bgInputHover: isDark ? "#232629" : "#fcfcfc"
    readonly property color bgCode: isDark ? "#1b1e20" : "#ffffff"
    readonly property color bgOverlay: isDark ? "#b01b1e20" : "#60000000"

    // Borders & dividers (Crisp 1px Breeze borders)
    readonly property color borderSubtle: isDark ? "#31363b" : "#e0e2e4"
    readonly property color borderStrong: isDark ? "#474a4c" : "#bdc3c7"
    readonly property color borderFocus: isDark ? "#3daee9" : "#1d99f3"
    readonly property color divider: isDark ? "#31363b" : "#e0e2e4"

    // Interactive & Accents (KDE Breeze Blue palette)
    readonly property color accent: isDark ? "#3daee9" : "#1d99f3"
    readonly property color accentHover: isDark ? "#56baf2" : "#3daee9"
    readonly property color accentPressed: isDark ? "#1d99f3" : "#157cc4"
    readonly property color accentLight: isDark ? "#203a4b" : "#e3f2fd"
    readonly property color linkColor: isDark ? "#3daee9" : "#1d99f3"
    readonly property color accentText: "#ffffff"

    // Status colors (Breeze Status tokens)
    readonly property color success: "#27ae60"
    readonly property color successBg: isDark ? "#1e3d2b" : "#e8f8f0"
    readonly property color warning: "#f67400"
    readonly property color warningBg: isDark ? "#3f260b" : "#fef3e7"
    readonly property color danger: "#da4453"
    readonly property color dangerBg: isDark ? "#3d1e22" : "#fdecee"
    readonly property color dangerHover: "#c83342"

    // Text colors (Breeze typography contrast)
    readonly property color textPrimary: isDark ? "#fcfcfc" : "#232629"
    readonly property color textSecondary: isDark ? "#bdc3c7" : "#69717d"
    readonly property color textMuted: isDark ? "#7a8288" : "#8f99a3"
    readonly property color textOnAccent: "#ffffff"

    // Typography (KDE standard font hierarchy)
    readonly property string fontFamily: "'Noto Sans', 'Segoe UI', -apple-system, BlinkMacSystemFont, Roboto, sans-serif"
    readonly property string fontCode: "'Hack', 'Noto Sans Mono', 'Fira Code', 'DejaVu Sans Mono', monospace"

    readonly property int fontSizeXs: 11
    readonly property int fontSizeSm: 12
    readonly property int fontSizeMd: 13
    readonly property int fontSizeLg: 15
    readonly property int fontSizeXl: 17
    readonly property int fontSizeXxl: 20
    readonly property int fontSizeTitle: 24

    // Dimensions & Spacings (KDE Breeze: 4px corner radius standard)
    readonly property int radiusXs: 2
    readonly property int radiusSm: 3
    readonly property int radiusMd: 4
    readonly property int radiusLg: 5
    readonly property int radiusXl: 6
    readonly property int radiusFull: 9999

    readonly property int spacingXs: 4
    readonly property int spacingSm: 8
    readonly property int spacingMd: 12
    readonly property int spacingLg: 16
    readonly property int spacingXl: 24
    readonly property int spacingXxl: 32

    // Animation standard durations
    readonly property int animFast: 120
    readonly property int animNormal: 200
    readonly property int animSlow: 350
}
