# TODO = Document
# TODO = Support Linux builds

#!/usr/bin/env python
import os
import sys

STEAM_HEADERS = "steamworks-api/public"
STEAM_BINARIES = "steamworks-api/redistributable_bin"

name = "libgdsteamhl"
env = SConscript("godot-cpp/SConstruct", variant_dir='src/build')

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/", STEAM_HEADERS])
sources = Glob("src/*.cpp")

steam_lib = "steam_api"
steam_libpath = STEAM_BINARIES
if env["platform"] == "macos":
    steam_lib += ".dylib"
    steam_libpath += "/osx"
    env.Append(LIBS=[steam_lib])
    env.Append(LIBPATH=[steam_libpath])
else:
    # Only support 64-bit
    steam_lib += "64.lib"
    steam_libpath += "/win64"
    # Thank you GodotSteam!
    env.Append(LIBPATH=[steam_libpath])
    if env["CC"] == "cl":
        env.Append(LINKFLAGS=[steam_lib])
    else:
        env.Append(LIBPATH=[steam_libpath])


if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/{}.{}.{}.framework/{}.{}.{}".format(
            name, env["platform"], env["target"], name, env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "demo/bin/{}{}{}".format(name, env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
