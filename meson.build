project('nett', 'cpp',
  version: '0.0.1',
  default_options: ['default_library=shared',
                      'cpp_std=c++20',
                      'cpp_args=-Wall -Wextra -Wpedantic -Wshadow'],
    meson_version: '>=1.0.0')

vcpkg = find_program('vcpkg', required : true)
run_command(vcpkg, 'install', '--x-wait-for-lock',
                              '--x-manifest-root=' + meson.current_source_dir())

fmt_dep = dependency('fmt')

subdir('src')

executable('nett', sources,
  dependencies: [fmt_dep])
