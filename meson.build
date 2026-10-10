project(
	'bench-box',
	['c', 'cpp'],
	default_options: [
		'warning_level=3',
		'cpp_std=c++20',
		'cpp_rtti=false',
		'b_lto=false',
	],
	meson_version: '>=1.1.0'
)

cpp_compiler = meson.get_compiler('cpp')

add_project_arguments(cpp_compiler.get_supported_arguments([
	'-Wconversion',
	'-Wsign-conversion',
	'-Wdiv-by-zero',
	'-Wshorten-64-to-32',
	'-Wfloat-equal',
	'-Wcast-align',
	'-Wshadow',
	'-Wmissing-field-initializers',
	'-Wmissing-noreturn',
	'-Wmissing-declarations',
	'-Wmissing-variable-declarations',
	'-Wmissing-prototypes',
	'-Wstrict-prototypes',
	'-Woverloaded-virtual',
	'-Wvla',
	'-Wpointer-arith',
	'-Wempty-body',
	'-Wpragmas',
	'-Wformat-security',
	'-Wformat=2',
	'-Wformat-nonliteral',
	'-Wwrite-strings',
	'-Wnested-externs',
	'-Wendif-labels',
	'-Wunreachable-code',
	'-Wunreachable-code-break',
	'-Waddress',
	'-Wnull-dereference',
	'-Wattributes',
	'-Wextended-offsetof',
	'-Wlanguage-extension-token',
	'-Wswitch-enum',
	'-Wunused-macros',
	'-Wunused-parameter',
	'-Wredundant-decls',
	'-Wheader-guard',
	'-Wmisleading-indentation',
	'-Wlogical-op',
	'-Wduplicated-cond',
	'-Wduplicated-branches',
	'-Wlifetime',
	'-Weffc++',
	'-fexperimental-library',
	'-DBENCH_BOX_BENCH_VERSION="' + meson.project_version() + '"',
	'/D_WIN32_VERSION=0x0601', # better win 7 compat
	'/DNTDDI_VERSION=0x06010000', # better win 7 compat
	'/D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR', # fix abi issue with older redist
	'/DNOMINMAX' # don't use macro min / max
]), language: 'cpp')

lyra_proj = subproject('lyra')
lyra_dep = lyra_proj.get_variable('lyra_dep')

base_name = 'bench-box-'

sources_core = [
	'src/bench_box/memory/memory.cpp',
	'src/bench_box/threading/threading.cpp',
	'src/bench_box/threading/topology.cpp',
	'src/bench_box/timer.cpp',
	'src/bench_box/format.cpp',
	'src/bench_box/console.cpp',
	'src/bench_box/isa/x86/cpuid.cpp'
]

inc_dir = 'src'

lib = static_library(
	base_name,
	sources_core,
	include_directories: inc_dir,
	dependencies: [
		lyra_dep
	],
	install: false
)

bench_box_dep = declare_dependency(
	include_directories: inc_dir,
	link_with: lib,
)

gtest_dep = dependency('gtest_main', static: true)

test_exe = executable(
	base_name + '-utest',
	[
		'test/src/buffer_test.cpp',
		'test/src/mapping.cpp',
		'test/src/mapping_numa_test.cpp',
		'test/src/mapping_phte_test.cpp',
		'test/src/parse_test.cpp',
		'test/src/timer_test.cpp'
	],
	include_directories: inc_dir,
	dependencies: [
		gtest_dep
	],
	link_with: [
		lib
	]
)

test('gtest', test_exe, protocol: 'gtest', timeout: 30)

harness_test_exe = executable(
	base_name + '-harness-utest',
	[
		'test/src/harness_test.cpp',
		'test/src/harness_parse_test.cpp'
	],
	include_directories: inc_dir,
	dependencies: [
		gtest_dep,
		lyra_dep
	],
	link_with: [
		lib
	]
)

test('harness-gtest', harness_test_exe, protocol: 'gtest', timeout: 300)
