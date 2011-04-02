#!/bin/env python
# -*- coding: utf-8 -*-


def options(opt):
	opt.load('compiler_cxx')


def configure(conf):
	import os, sys, dependencies
	try:
		sys.path.append(os.environ['YMSE_PATH'])
		import pymse.wafutil as wafutil
	except Exception, e:
		print "Failed to load pymse.wafutil. Bailing out"
		sys.exit(1)


	wafutil.msvc_initial_setup(conf.env)

	conf.load('compiler_cxx')

	cc = wafutil.get_compiler_configurator(conf)

	cc.sane_default(conf.env)
	cc.many_warnings(conf.env)

	# Box2D gets some warnings.
	#cc.warnings_as_errors(conf.env)

	core_env = conf.env.derive()

	debug_env = core_env.derive()
	cc.debug_mode(debug_env)
	conf.setenv('debug', env = debug_env)

	release_env = core_env.derive()
	cc.release_mode(release_env)
	cc.optimize(release_env)
	cc.link_time_code_generation(release_env)
	conf.setenv('release', env = release_env)

	wafutil.configure_ymse(debug_env, release_env)

	dependencies.do_import()
	dependencies.configure(debug_env, release_env)


def core_build(bld):
	bld.add_subdirs('dependencies')
	bld.add_subdirs('src')


# All of the following boiler plate is recommended (demanded) by the waf book for enabling build variants:

def build(bld):
	if not bld.variant:
		bld.fatal('call "waf build_debug" or "waf build_release", and try "waf --help"')

	core_build(bld)

from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext

for x in ['debug', 'release']:
	for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
		name = y.__name__.replace('Context','').lower()
		class tmp(y):
			cmd = name + '_' + x
			variant = x
