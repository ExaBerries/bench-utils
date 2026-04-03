project(
	'bench-util',
	['c', 'cpp'],
	default_options: [
		'warning_level=3',
		'cpp_std=c++20',
		'cpp_rtti=false',
		'b_lto=false',
	],
	meson_version: '>=1.1.0',
	version: '0.1.0'
)

imgdump_option = get_option('imgdump')

cpp_compiler = meson.get_compiler('cpp')

add_project_arguments(cpp_compiler.get_supported_arguments([
	'-Wconversion',
	'-Wsign-conversion',
	'-Wdiv-by-zero',
	'-Wshorten-64-to-32',
	'-Wfloat-equal',
	'-Wcast-align',
	'-Wuseless-cast',
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
	'-DBENCH_UTIL_BENCH_VERSION="' + meson.project_version() + '"',
	'/D_WIN32_VERSION=0x0601', # better win 7 compat
	'/DNTDDI_VERSION=0x06010000', # better win 7 compat
	'/D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR' # fix abi issue with older redist
]), language: 'cpp')

base_name = 'bench-utils-' + meson.project_version()

sources_core = [
	'src/bench_utils/memory/memory.cpp',
	'src/bench_utils/threading/threading.cpp',
	'src/bench_utils/threading/topology.cpp',
	'src/bench_utils/timer.cpp',
	'src/bench_utils/format.cpp',
	'src/bench_utils/isa/x86/cpuid.cpp'
]

inc_dir = 'src'

lib = static_library(
	base_name,
	sources_core,
	include_directories: inc_dir,
	install: false
)

bench_utils_dep = declare_dependency(
	include_directories: inc_dir,
	link_with: lib
)

if host_machine.system() != 'windows'
	# had issues with gtest in windows...
	gtest_dep = dependency('gtest_main')

	test_exe = executable(
		base_name + '-utest',
		[
			'test/src/buffer_test.cpp'
		],
		include_directories: inc_dir,
		dependencies: gtest_dep,
		link_with: [
			lib
		]
	)

	test('gtest', test_exe, protocol: 'gtest')
endif
