project('pcg-test',
        'cpp',
        version : '0.0.0',
        default_options: [
          'cpp_std=c++2a'
        ])

lsfml_window = dependency('sfml-window')
lsfml_system = dependency('sfml-system')
lsfml_graphics = dependency('sfml-graphics')
deps = [lsfml_window, lsfml_system, lsfml_graphics]

sources = []
subdir('src')

executable('pcg-test', sources, dependencies:deps)
