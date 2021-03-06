#!/usr/bin/env python

import os


class Exists:
	def __init__(self, path):
		self.path = path

	def __call__(self, installpath):
		return os.path.exists(os.path.join(installpath, self.path))


BOX2D = {
	"name": "Box2D",
	"uri": "http://box2d.googlecode.com/files/Box2D_v2.1.2.zip",
	"target_path": "",
	"is_installed": Exists("Box2D_v2.1.2"),
	"includes": "Box2D_v2.1.2/Box2D"
}

PACKAGES = [ BOX2D ]


def extract_uri(uri, target_path, verbosity=1):
	import urllib2, zipfile, StringIO

	if verbosity >= 1: print "Downloading %s..." % uri
	f = urllib2.urlopen(uri)
	b = StringIO.StringIO(f.read())
	if verbosity >= 1: print "Extracting...",
	z = zipfile.ZipFile(b, 'r')
	z.extractall(target_path)
	if verbosity >= 1: print "done"


def do_import(verbosity=1):
	basepath = os.path.dirname(__file__)

	for p in PACKAGES:
		installpath = os.path.join(basepath, p["target_path"])
		is_installed = p["is_installed"](installpath)

		if is_installed:
			if verbosity >= 1: print "%s already installed" % p["name"]
		else:
			extract_uri(p["uri"], installpath, verbosity)


def configure(debug_env, release_env):
	basepath = os.path.abspath(os.path.dirname(__file__))

	release_env.INCLUDES_Box2D_inc = debug_env.INCLUDES_Box2D_inc = [
		os.path.join(basepath, BOX2D["includes"])
	]


if __name__ == '__main__':
	do_import()
