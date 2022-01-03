project(
  'pcg-test',
  'cpp',
  version: '0.0.0',
  default_options: [
    'cpp_std=c++20'
  ],
)

exe_deps = []
exe_deps += dependency(
  'sfml-all',
  fallback: [
    'sfml',
    'sfml_dep',
  ],
)
exe_deps += dependency(
  'eigen3',
)

sources = []
subdir('src')

executable(
  meson.project_name(),
  sources: sources,
  dependencies: exe_deps,
)
