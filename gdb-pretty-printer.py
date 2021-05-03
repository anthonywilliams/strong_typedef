import gdb.printing

class StrongTypedefPrinter:
    def __init__(self,val):
        self.val=val

    def to_string(self):
        return "{value=" + str(self.val["value"])+"}"

def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter(
        "strong_typedef")
    pp.add_printer('strong_typedef', '^jss::strong_typedef<.*>$', StrongTypedefPrinter)
    return pp

gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    build_pretty_printer())
