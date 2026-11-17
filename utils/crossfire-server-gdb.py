"""
crossfire-server-gdb.py -- Crossfire pretty-printers for GDB

To use these pretty-printers, source this file into your GDB session or add
this file to your GDB auto-load path.
"""

import re

class KeyValuePrinter(object):
    def __init__(self, val):
        self.val = val

    def children(self):
        val = self.val
        while val != 0:
            f = val.dereference()
            yield ("\"%s\"" % f['key'].string(), "\"%s\"" % f['value'].string())
            val = val['next']

class ObjectPrinter(object):
    def __init__(self, val):
        self.val = val

    def to_string(self):
        rval = "Obj %d" % self.val['count']
        if self.val['name'] != 0:
            rval = rval + " \"%s\"" % self.val['name'].string()
        return rval

    def children(self):
        def pfnn(f):
            if self.val[f] != 0:
                yield (f, self.val[f])

        yield ('type', "%d/%d" % (self.val['type'], self.val['subtype']))
        yield from pfnn('arch')
        yield from pfnn('nrof')
        yield from pfnn('inv')
        yield from pfnn('contr')
        yield from pfnn('env')
        yield from pfnn('above')
        yield from pfnn('below')
        yield from pfnn('container')
        yield from pfnn('more')
        yield from pfnn('head')
        yield from pfnn('map')
        yield from pfnn('artifact')
        yield from pfnn('race')
        yield from pfnn('slaying')
        yield from pfnn('skill')
        yield from pfnn('lore')
        yield from pfnn('speed')
        yield from pfnn('speed_left')
        yield from pfnn('materialname')
        yield from pfnn('material')
        yield from pfnn('value')
        yield from pfnn('level')
        yield from pfnn('invisible')
        yield from pfnn('other_arch')
        yield from pfnn('owner')
        yield from pfnn('enemy')
        yield from pfnn('attacked_by')
        yield from pfnn('randomitems')
        yield from pfnn('chosen_skill')
        yield from pfnn('spellitem')
        yield from pfnn('spell')
        yield from pfnn('spellarg')
        yield ('key_values', self.val['key_values'])

printers = [
    (re.compile('.*key_value \*$'), KeyValuePrinter),
    (re.compile('.*object$'), ObjectPrinter),
    (re.compile('.*struct obj$'), ObjectPrinter)
]

def cfpp_lookup(val):
    t = str(val.type)
    for regex, printer in printers:
        if regex.match(t):
            return printer(val)
    return None

gdb.pretty_printers.append(cfpp_lookup)
