set_xmakever("2.8.3")
set_project("PandaEngine")

set_policy("build.ccache", false)

set_languages("c11","cxx17")

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
add_rules("mode.debug", "mode.release")

if (is_os("windows")) then 
    add_defines("UNICODE","_WINDOWS")
elseif (is_os("macosx") or is_os("linux")) then
    add_requires("libsdl")
else
    -- .....
end

includes("Source/xmake.lua")
includes("Samples/xmake.lua")