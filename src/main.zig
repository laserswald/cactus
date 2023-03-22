const std = @import("std");
const c = @cImport({
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
    @cInclude("./core.h");
    @cInclude("evaluator/eval.h");
    @cInclude("write.h");
    @cInclude("err.h");
    @cInclude("builtins.h");
    @cInclude("linenoise.h");
    @cInclude("../config.h");
});

const cactus = struct {
    pub const Context = c.struct_cact_context;
    pub const initContext = c.cact_init;
    pub const evalString = c.cact_eval_string;
    pub const evalFile = c.cact_eval_file;
    pub const finishContext = c.cact_finish;

    pub const Value = c.cact_value_t;
    pub const display = c.cact_display;

    pub const Builtin = c.struct_cact_builtin;
    pub const defineBuiltinArray = c.cact_define_builtin_array;

    pub const isError = c.cact_is_error;
};

const PROMPT = ">()< | ";

var verbosity: c_int = 0;



pub export fn repl(arg_cact: [*c]c.cact_context) c_int {
    var cact = arg_cact;
    var line: [*c]u8 = undefined;
    var x: cactus.Value = undefined;
    while ((blk: {
        const tmp = c.linenoise(">()< ");
        line = tmp;
        break :blk tmp;
    }) != @ptrCast([*c]u8, @alignCast(@import("std").meta.alignment([*c]u8), @intToPtr(?*anyopaque, @as(c_int, 0))))) {
        x = cactus.evalString(cact, line);
        _ = cactus.display(x);
        _ = c.putchar(@as(c_int, '\n'));
        _ = c.fflush(c.stdout);
        _ = c.linenoiseHistoryAdd(line);
        c.linenoiseFree(@ptrCast(?*anyopaque, line));
    }
    return 0;
}

pub export fn load_file(arg_cact: [*c]cactus.Context, arg_filename: [*c]u8) cactus.Value {
    var cact = arg_cact;
    var filename = arg_filename;
    var v: cactus.Value = undefined;
    var f: [*c]c.FILE = null;
    f = c.fopen(filename, "r");
    if (!(f != null)) {
        c.perror("Could not open file");
        c.exit(@as(c_int, 1));
    }
    v = cactus.evalFile(cact, f);
    if (cactus.isError(v)) {
        _ = cactus.display(v);
        c.perror("Encountered errors");
        c.exit(@as(c_int, 1));
    }
    return v;
}

pub export fn main(arg_argc: c_int, arg_argv: [*c][*c]u8) c_int {
    var argc = arg_argc;
    var argv = arg_argv;
    var cact: cactus.Context = undefined;
    var result: c_int = 0;
    cactus.initContext(&cact);
    cactus.defineBuiltinArray(&cact, @ptrCast([*c]cactus.Builtin, @alignCast(@import("std").meta.alignment([*c]cactus.Builtin), &c.builtins)), @sizeOf([35]cactus.Builtin) / @sizeOf(cactus.Builtin));
    _ = load_file(&cact, @intToPtr([*c]u8, @ptrToInt("rt/init.scm")));
    if (argc == @as(c_int, 2)) {
        _ = load_file(&cact, argv[@intCast(c_uint, @as(c_int, 1))]);
    } else {
        result = repl(&cact);
    }
    cactus.finishContext(&cact);
    return result;
}

test "simple test" {
    var list = std.ArrayList(i32).init(std.testing.allocator);
    defer list.deinit(); // try commenting this out and see if zig detects the memory leak!
    try list.append(42);
    try std.testing.expectEqual(@as(i32, 42), list.pop());
}
