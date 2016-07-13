#!/usr/bin/env python
# -*- coding: utf-8 -*-

from os import path
from waflib.Configure import conf

def load_tools(ctx):
    ctx.load('compiler_cxx')
    ctx.load(['run_command'], tooldir="buildtools");

def options(ctx):
    load_tools(ctx)

    ctx.add_option('--debug', action='store', default=True, help='Compile with debug symbols')

def configure(conf):
    load_tools(conf)

    conf.env['CXXFLAGS'] = ['-std=c++11']

    print "Debug: ", conf.options.debug

    if conf.options.debug:
        conf.env['CXXFLAGS'] += ['-g']
        conf.env['CFLAGS'] += ['-g']

def build(bld):
    bld.recurse('src')

@conf
def lib(bld, *k, **kw):
    if bld.options.shared:
        bld.shlib(*k, **kw)
    else:
        bld.stlib(*k, **kw)
