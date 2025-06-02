#include "register_types.h"

#include <src/gdsteam_only/input/inputeventsteamhlanalogaction.h>
#include <src/gdsteam_only/input/inputeventsteamhldigitalaction.h>
#include <src/steamhl.h>
#include <src/steam_wrapper/data_structures/steamhlanalogactiondata.h>
#include <src/steam_wrapper/data_structures/steamhldigitalactiondata.h>
#include <src/steam_wrapper/api/steamhlfriends.h>
#include <src/steam_wrapper/data_structures/steamhlid.h>
#include <src/steam_wrapper/api/steamhlinput.h>
#include <src/steam_wrapper/api/steamhlnetworkingsockets.h>
#include <src/gdsteam_only/multiplayer/steamhlpeer.h>
#include <src/steam_wrapper/api/steamhluser.h>
#include <src/steam_wrapper/api/steamhlutils.h>
#include <src/steam_wrapper/api/steamHLnetworkingutils.h>
#include <src/steam_wrapper/api/steamhlapps.h>
#include <src/steam_wrapper/api/steamhluserstats.h>

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void initialize_gdsteamhl_module(ModuleInitializationLevel p_level) {
    // TODO=get_singleton() may cache the null forever, even if Engine is registered later. A test is needed to verify this.
    // Source: https://github.com/godotengine/godot-cpp/issues/1180
    if (p_level == MODULE_INITIALIZATION_LEVEL_CORE && Engine::get_singleton() != nullptr && !(Engine::get_singleton()->is_editor_hint())) {
        // We need to call this for Vulkan renderers (allegedly, some Vulkan implementations do not allow the Steam Overlay to hook into the correct Vulkan layer after the windows has been created)
        // NOTE: When launched from Steam, this is not an issue since Steam initialized the API (as far as I can tell)
        SteamAPI_Init();
    }
    else if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        ClassDB::register_class<SteamHLID>();
        ClassDB::register_class<SteamHLAnalogActionData>();
        ClassDB::register_class<SteamHLDigitalActionData>();
        ClassDB::register_class<InputEventSteamHLAnalogAction>();
        ClassDB::register_class<InputEventSteamHLDigitalAction>();
        ClassDB::register_class<SteamHL>();
        ClassDB::register_class<SteamHLInput>();
        ClassDB::register_class<SteamHLPeer>();
        ClassDB::register_class<SteamHLUtils>();
        ClassDB::register_class<SteamHLFriends>();
        ClassDB::register_class<SteamHLUser>();
        ClassDB::register_class<SteamHLNetworkingUtils>();
        ClassDB::register_class<SteamHLApps>();
        ClassDB::register_class<SteamHLUserStats>();

        Engine::get_singleton()->register_singleton("SteamHL", SteamHL::get_singleton());
        Engine::get_singleton()->register_singleton("SteamHLInput", SteamHLInput::get_singleton());
        Engine::get_singleton()->register_singleton("SteamHLUtils", SteamHLUtils::get_singleton());
        Engine::get_singleton()->register_singleton("SteamHLUserStats", SteamHLUserStats::get_singleton());
    }
}

void uninitialize_gdsteamhl_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        Engine::get_singleton()->unregister_singleton("SteamHL");
        Engine::get_singleton()->unregister_singleton("SteamHLInput");
        Engine::get_singleton()->unregister_singleton("SteamHLUtils");
        Engine::get_singleton()->unregister_singleton("SteamHLUserStats");

        // While the singleton could theoretically be reused, we're deleting these here to
        // prevent weird edge cases around signals connecting to freed objects and SteamHL
        // thinking it is initialized despite no longer being connected to MainLoop.

        // Note: If there is a lambda connected to a signal, then an Access Violation will occur
        // when the object is memdelete'd. Fun!
        // The crash path is slightly different depending on if the object was memdelete'd in SCENE
        // or CORE.
        memdelete(SteamHLUserStats::get_singleton());
        memdelete(SteamHLUtils::get_singleton());
        memdelete(SteamHLInput::get_singleton());
        memdelete(SteamHL::get_singleton()); // TODO=This leaves a pointer to invalid memory in the SteamHL class
    }
}

extern "C" {
    // Initialization.
    // GDScript for 4.0
    /*GDExtensionBool GDE_EXPORT gdsteamhl_library_init(const GDExtensionInterface* p_interface, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization) {
        godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

        init_obj.register_initializer(initialize_gdsteamhl_module);
        init_obj.register_terminator(uninitialize_gdsteamhl_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
        
        return init_obj.init();
    }*/
    // GDScript for 4.1 (and hopefully later)
    GDExtensionBool GDE_EXPORT gdsteamhl_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization) {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
        
        init_obj.register_initializer(initialize_gdsteamhl_module);
        init_obj.register_terminator(uninitialize_gdsteamhl_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE); // TODO=If this needs an editor or engine restart to take effect, then it should be CORE (it only does if reinitializing the Steam API doesn't work in Vulkan because the display has already been created)

        return init_obj.init();
    }
}