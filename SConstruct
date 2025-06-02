# TODO = Document
# TODO = Support Linux builds

#!/usr/bin/env python
import os
import sys

STEAM_HEADERS = "steamworks-api/public/steam"
STEAM_BINARIES = "steamworks-api/redistributable_bin"


env = SConscript("godot-cpp/SConstruct")
SConscript("src/SConstruct",
           variant_dir="src/build",
           duplicate=False,
           exports=["env", "STEAM_HEADERS", "STEAM_BINARIES"]
)