#!/bin/env python
# -*- coding: utf-8 -*-

import configurators


def options(opt):
	opt.load('compiler_cc')
	opt.load('compiler_cxx')


def msvc_initial_setup(conf):
	conf.env['MSVC_VERSIONS'] = ['msvc 9.0']
	conf.env['MSVC_TARGETS'] = ['x86']


def configure(conf):
	msvc_initial_setup(conf)

	conf.load('compiler_cc')
	conf.load('compiler_cxx')

	cc = configurators.get_configurator(conf)

	cc.sane_default(conf.env)
	cc.many_warnings(conf.env)
	cc.warnings_as_errors(conf.env)

	core_env = conf.env.derive()

	debug_env = core_env.derive()
	cc.debug_mode(debug_env)
	conf.setenv('debug', env = debug_env)

	release_env = core_env.derive()
	cc.release_mode(release_env)
	cc.optimize(release_env)
	cc.link_time_code_generation(release_env)
	conf.setenv('release', env = release_env)


# All of the following boiler plate is recommended (demanded) by the waf book for enabling build variants:

def build(bld):
	if not bld.variant:
		bld.fatal('call "waf build_debug" or "waf build_release", and try "waf --help"')

	bld.add_subdirs('src')

from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext

for x in 'debug release'.split():
	for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
		name = y.__name__.replace('Context','').lower()
		class tmp(y):
			cmd = name + '_' + x
			variant = x

