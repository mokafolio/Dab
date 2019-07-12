project('Dab', ['cpp', 'c'], default_options : ['cpp_std=c++14'])

if meson.is_subproject() == false
    # disable this bogus warning (should be deprecated in newer clang versions)
    add_global_arguments('-Wno-missing-braces', '-fno-exceptions', language: 'cpp')
else
    message('Building Dab as subproject.')
endif

# to silence gl deprecation warnings on osx
if host_machine.system() == 'darwin'
    add_project_arguments('-DGL_SILENCE_DEPRECATION', language: ['cpp', 'c'])
endif

if meson.is_subproject() == false or get_option('forceInstallHeaders')
    install_headers('Dab/Dab.hpp', subdir: 'Dab')
    install_headers('Dab/OpenGL/GLDab.hpp', subdir: 'Dab/OpenGL')
    install_headers('Dab/Libs/GL/gl3w.h', subdir: 'Dab/Libs/GL')
endif

dabSrc = [
    'Dab/Dab.cpp',
    'Dab/OpenGL/GLDab.cpp',
    'Dab/Libs/GL/gl3w.c'
]

cc = meson.get_compiler('cpp')

incDirs = include_directories('.', 'Dab/Libs')

cc = meson.get_compiler('cpp')
dabDeps = []
dabExampleDeps = []
if get_option('buildSubmodules') == false
    dabDeps += [cc.find_library('Stick'), dependency('threads')]
    if get_option('buildExamples') == true
        dabExampleDeps += [cc.find_library('Luke', required: true), dependency('sdl2')]
    endif
else
    stickProj = subproject('Stick')
    dabDeps += stickProj.get_variable('stickDep')
    if get_option('buildExamples') == true
        lukeProj = subproject('Luke')
        dabExampleDeps += lukeProj.get_variable('lukeDep')
    endif
endif

dabDeps += dependency('GL')

if host_machine.system() == 'linux'
    dabDeps += cc.find_library('dl', required : true)
endif

dab = library('Dab', dabSrc, dependencies: dabDeps, include_directories : incDirs, install: meson.is_subproject() == false)

dabDep = declare_dependency(link_with : dab, 
    dependencies: dabDeps, 
    include_directories: incDirs)

if get_option('buildExamples') == true
    subdir('Examples')
endif
