const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    // Standard target options allows the person running `zig build` to choose
    // what target to build for. Here we do not override the defaults, which
    // means any target is allowed, and the default is native. Other options
    // for restricting supported target set are available.
    const target = b.standardTargetOptions(.{});

    // Standard release options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const mode = b.standardReleaseOptions();
    
    const lib = b.addStaticLibrary("cactus", "src/core.c");
    lib.addIncludePath("src");
    lib.addCSourceFiles(&[_][]const u8{
        "src/evaluator/eval.c",
        "src/evaluator/cont.c",
        "src/pair.c",
        "src/number.c",
        "src/str.c",
        "src/val.c",
        "src/read.c",
        "src/storage/store.c",
        "src/storage/arena.c",
        "src/storage/arena_set.c",
        "src/storage/obj.c",
        "src/utils.c",
        "src/err.c",
        "src/env.c",
        "src/sym.c",
        "src/rec.c",
        "src/vec.c",
        "src/xmalloc.c",
        "src/proc.c",
        "src/builtin.c",
        "src/port.c",
        "src/bool.c",
        "src/syn.c",
        "src/write.c",
        "src/compiler/vm.c",
    }, &[_][]const u8{
        "-g",
        "-Isrc",
        "-Ilib",
        "-Wall",
    });
    lib.setTarget(target);
    lib.setBuildMode(mode);
    lib.install();

    const exe = b.addExecutable("cact", "src/main.zig");
    exe.addIncludePath("src");
    exe.addIncludePath("lib");
    lib.addCSourceFiles(&[_][]const u8{
        "lib/linenoise.c",
    }, &[_][]const u8{
        "-g",
        "-Isrc",
        "-Ilib",
        "-Wall",
    });
    exe.linkLibrary(lib);
    exe.linkLibC();
    exe.setTarget(target);
    exe.setBuildMode(mode);
    exe.install();

    const run_cmd = exe.run();
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    const exe_tests = b.addTest("src/main.zig");
    exe_tests.setTarget(target);
    exe_tests.setBuildMode(mode);

    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&exe_tests.step);
}
