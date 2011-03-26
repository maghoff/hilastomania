#!/bin/env python
# -*- coding: utf-8 -*-

class gcc_configurator:
	@staticmethod
	def sane_default(env):
		env.append_unique('CCFLAGS', '-fPIC')
		env.append_unique('CXXFLAGS', '-fPIC')
		env.append_unique('LINKFLAGS', '-fPIC')

	@staticmethod
	def debug_mode(env):
		env.append_unique('CXXFLAGS', '-ggdb')
		env.append_unique('CXXDEFINES', 'DEBUG')
		env.append_unique('CCFLAGS', '-ggdb')
		env.append_unique('CCDEFINES', 'DEBUG')

	@staticmethod
	def release_mode(env):
		env.append_unique('CXXDEFINES', 'NDEBUG')
		env.append_unique('CCDEFINES', 'NDEBUG')
		env.append_unique('LINKFLAGS', '-s')

	@staticmethod
	def optimize(env):
		env.append_unique('CXXFLAGS', '-O3')
		env.append_unique('CCFLAGS', '-O3')

	@staticmethod
	def many_warnings(env):
		env.append_unique('CXXFLAGS', '-Wall')
		env.append_unique('CCFLAGS', '-Wall')
		env.append_unique('LINKFLAGS', '-Wall')

	@staticmethod
	def warnings_as_errors(env):
		env.append_unique('CXXFLAGS', '-Werror')
		env.append_unique('CCFLAGS', '-Werror')
		env.append_unique('LINKFLAGS', '-Werror')

	@staticmethod
	def link_time_code_generation(env):
		if env['CC_VERSION'] >= ('4', '5'):
			env.append_unique('CXXFLAGS', '-flto')
			env.append_unique('CCFLAGS', '-flto')
			env.append_unique('LINKFLAGS', '-flto')
			# WHOPR, which is also available in GCC 4.5, is a more scalable
			# alternative, but it optimizes less. From GCC 4.6, WHOPR is the
			# default when specifying LTO like this.


class msvc_configurator:
	@staticmethod
	def sane_default(env):
		env.append_unique('CXXFLAGS', '/GR') # Enable RTTI
		env.append_unique('CXXFLAGS', '/GS') # Buffer Security Check

	@staticmethod
	def debug_mode(env):
		env.append_unique('CXXFLAGS', '/MDd')
		env.append_unique('CXXFLAGS', '/Od')
		env.append_unique('CXXFLAGS', '/RTC1')
		env.append_unique('LINKFLAGS', '/DEBUG')
		env.append_unique('CXXDEFINES', 'DEBUG')
		env.append_unique('CCFLAGS', '/MDd')
		env.append_unique('CCDEFINES', 'DEBUG')

	@staticmethod
	def release_mode(env):
		env.append_unique('CXXFLAGS', '/MD')
		env.append_unique('CXXDEFINES', 'NDEBUG')
		env.append_unique('CCFLAGS', '/MD')
		env.append_unique('CCDEFINES', 'NDEBUG')

	@staticmethod
	def optimize(env):
		env.append_unique('CXXFLAGS', '/O2')
		env.append_unique('CCFLAGS', '/O2')

	@staticmethod
	def many_warnings(env):
		pass

	@staticmethod
	def warnings_as_errors(env):
		env.append_unique('CXXFLAGS', '/WX')
		env.append_unique('CCFLAGS', '/WX')
		env.append_unique('LINKFLAGS', '/WX')

	@staticmethod
	def link_time_code_generation(env):
		env.append_unique('CXXFLAGS', '/GL')
		env.append_unique('CCFLAGS', '/GL')
		env.append_unique('LINKFLAGS', '/LTCG')


def get_configurator(conf):
	compiler_configurators = {
		'gcc': gcc_configurator,
		'msvc': msvc_configurator,
	}

	return compiler_configurators[conf.env['CXX_NAME']]
