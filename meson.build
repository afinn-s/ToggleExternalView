project('ToggleExternalView', 'cpp', default_options: ['cpp_std=c++20', 'buildtype=release'])

add_project_arguments('-DXPLM200=1', '-DXPLM210=1', 
                      '-DXPLM300=1', '-DXPLM301=1',
                      '-DXPLM303=1', '-DXPLM400=1', 
                      '-DXPLM411=1', language: 'cpp')

if target_machine.system() == 'windows'
    add_project_arguments('-DIBM=1', '-D_CRT_SECURE_NO_WARNINGS', language: 'cpp')
elif target_machine.system() == 'darwin'
    add_project_arguments('-DAPL=1', '-fPIC', language: 'cpp')
else
    add_project_arguments('-DLIN=1', '-fPIC', language: 'cpp')
endif

xplm_dep = dependency('xplm')

subdir('src')
shared_library(
    'ToggleExternalView',
    sources,
    name_suffix: 'xpl',
    dependencies: xplm_dep,
    cpp_args: '-fuse-ld=mold')