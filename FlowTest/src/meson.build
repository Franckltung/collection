project('FlowTest', 'cpp')
tbbdep = dependency('tbb')
SDL2dep = dependency('sdl2')
SDL2_ttfdep = dependency('SDL2_ttf')
SDL2_imagedep = dependency('SDL2_image')
imgui = include_directories('imgui')
imgui_sources = ['imgui/imgui_demo.cpp', 'imgui/imgui_draw.cpp', 'imgui/imgui_tables.cpp', 'imgui/imgui_widgets.cpp', 'imgui/imgui.cpp', 'imgui/imgui_impl_sdl.cpp', 'imgui/imgui_impl_sdlrenderer.cpp']

#add_project_arguments('-pg', language : 'cpp')
#add_project_link_arguments('-pg', language : 'cpp')
add_project_arguments('-Ofast', '-march=native', language : 'cpp')

e = executable('FlowTest', ['flowtest.cpp', 'boidProcessor.cpp', 'world.cpp', 'simpleRenderer.cpp', 'timer.cpp', 'imGuiSystem.cpp', imgui_sources], dependencies : [tbbdep, SDL2dep, SDL2_ttfdep, SDL2_imagedep], include_directories : imgui)
test('testRun', e)
