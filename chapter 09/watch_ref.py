try:
    import gdb
except ImportError as e:
    raise ImportError("This script must be run in gdb: ", str(e))
'''
First define the global settings
'''
creation_function = 'after_this_function()'
creation_breakpoint = None
watch_point = None
symbol = 'referenceCount'

def print_stack_trace():
    gdb.execute('bt')

class CustomWatchPoint(gdb.Breakpoint):
    '''
    gdb watchpoint expression '..' doesn't work. It will complains 'You may have requested too many hardware breakpoints/watchpoints.'
    The workaround is set wp by address, like 'watch *(long *) 0xa0f74d8'
    '''

    def __init__(self, expr, cb):
        self.expr = expr
        self.val = gdb.parse_and_eval(self.expr)
        self.address = self.val.address
        self.ty = gdb.lookup_type('int')
        addr_expr = '*(int*)' + str(self.address)
        gdb.Breakpoint.__init__(self, addr_expr, gdb.BP_WATCHPOINT)
        self.silent = True
        self.callback = cb

    def stop(self):
        addr = int(str(self.address), 16)
        val_buf = gdb.selected_inferior().read_memory(addr, 4)
        val = gdb.Value(val_buf, self.ty)
        print('symbal value = ' + str(val))
        self.callback()
        return False

class CustomBreakPoint(gdb.Breakpoint):
    '''
    gdb breakpoint expression 
    '''

    def __init__(self, bp_expr, cb, temporary=False):
        # spec [, type ][, wp_class ][, internal ][, temporary ][, qualified ])
        gdb.Breakpoint.__init__(
            self, bp_expr, gdb.BP_BREAKPOINT, False, temporary)
        self.silent = True
        self.callback = cb

        def stop(self):
            self.callback()
        return False

class CustomFinishBreakpoint(gdb.FinishBreakpoint):
    def stop(self):
        print("normal finish")
        global watch_point
        try:
            if watch_point is None:
                watch_point = CustomWatchPoint(symbol, print_stack_trace)
            if not watch_point.is_valid():
                print("Cannot watch " + symbol)
            else:
                print("watching " + symbol)
        except RuntimeError as e:
            print(e)

        return False

def out_of_scope():
    print("abnormal finish")

class CreateWatchPointCommand(gdb.Command):
    '''
        A gdb command that traces memory usage
    '''
    _command = "watch_ref"

    def __init__(self):
        gdb.Command.__init__(self, self._command, gdb.COMMAND_STACK)

    def invoke(self, argument, from_tty):
        '''
        1. Try to create the watchpoint
        2. If fail, then create a breakpoint after the variable has been created.
        '''
        global symbol
        global watch_point
        global creation_breakpoint
        watch_point = None
        creation_breakpoint = None

def watch():
    customFinishPoint = CustomFinishBreakpoint()
    try:
        creation_breakpoint = CustomBreakPoint(creation_function, watch)
    except Exception as e:
        print(e)

gdb.execute("set pagination off")
gdb.execute("set print object on")
gdb.execute("handle SIGSEGV nostop noprint pass")
CreateWatchPointCommand()
gdb.execute('watch_ref')
gdb.execute('set logging file debug-logging.txt')
gdb.execute('set logging on')
gdb.execute("c")
