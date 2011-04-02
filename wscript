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

	wafutil.do_some_configuration(conf)

	dependencies.do_import()


def core_build(bld):
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
