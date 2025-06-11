const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const graphi = b.addStaticLibrary(.{
        .name = "graphi",
        .target = target,
        .optimize = optimize,
    });
    graphi.addCSourceFile(.{ .file = b.path("src/graphi.c") });

    graphi.addIncludePath(b.path("include"));
    graphi.linkLibC();

    const headerdir = b.path("include");
    b.addNamedLazyPath("include", headerdir);

    const header = b.addInstallHeaderFile(headerdir.path(b, "graphi.h"), "graphi.h");
    b.getInstallStep().dependOn(&header.step);
    b.installArtifact(graphi);

    // ----------------------------------------------------------------------

    const wasmtarget = b.resolveTargetQuery(.{ .cpu_arch = .wasm32, .os_tag = .freestanding });
    const wasmoptimize = .ReleaseSmall;

    inline for (examples) |example| {
        if (example.wasm) {
            const name = example.name;

            const wasmdir = "examples/wasm" ++ name ++ "/";
            const wasmout = "public/" ++ name ++ "/";

            const wexe = b.addExecutable(.{
                .name = "abcd",
                .optimize = wasmoptimize,
                .target = wasmtarget,
            });
            wexe.addCSourceFile(.{ .file = b.path(wasmdir ++ "main.c") });
            wexe.addCSourceFile(.{ .file = b.path("src/graphi.c"), .flags = &.{"-DGRAPHI_NO_LIBC=1"} });

            wexe.entry = .disabled;
            wexe.root_module.export_symbol_names = &.{"render"};

            wexe.addIncludePath(headerdir);

            const wasi = b.addInstallArtifact(wexe, .{ .dest_dir = .{ .override = .{ .custom = wasmout } } });

            const wasm = b.step("wasm-" ++ name, "build this example for wasm");
            wasm.dependOn(&wasi.step);

            const script = b.addInstallFile(b.path(wasmdir ++ "script.js"), wasmout ++ "script.js");
            const index = b.addInstallFile(b.path(wasmdir ++ "index.html"), wasmout ++ "index.html");

            wasm.dependOn(&script.step);
            wasm.dependOn(&index.step);

            // -----------

            const server = b.addSystemCommand(&.{
                "python", "-m",                  "http.server",
                "-d",     "zig-out/" ++ wasmout, "2348",
            });
            server.step.dependOn(wasm);
            const wasmserve = b.step("serve-" ++ name, "run a local server with this wasm example");
            wasmserve.dependOn(&server.step);
        } else {
            const exa = b.addExecutable(.{
                .name = example.name,
                .optimize = optimize,
                .target = target,
            });
            exa.addCSourceFile(.{ .file = b.path("examples/" ++ example.name ++ ".c") });
            exa.addIncludePath(headerdir);
            exa.linkLibC();
            exa.linkLibrary(graphi);

            const run = b.addRunArtifact(exa);
            const step = b.step("example-" ++ example.name, "run the " ++ example.name ++ " example");
            step.dependOn(&run.step);
        }
    }

    // ----------------------------------------------------------------------

    // bo-ho the tests are not working yet

    const teststep = b.step("test", "run the tests");
    const tests: []const []const u8 = &.{ "001", "002" };
    inline for (tests) |name| {
        const testExec = b.addExecutable(.{
            .name = name,
            .target = target,
            .optimize = optimize,
        });
        testExec.addCSourceFile(.{ .file = b.path("tests/test_" ++ name ++ ".c") });
        testExec.linkLibrary(graphi);
        testExec.addIncludePath(headerdir);
        // testExec.linkLibC();

        const testRun = b.addRunArtifact(testExec);
        testRun.expectExitCode(0);

        teststep.dependOn(&testRun.step);
    }
}

const Example = struct {
    name: []const u8,
    wasm: bool,
};

const examples: []const Example = &.{
    .{ .name = "timing", .wasm = false },
    .{ .name = "rubix", .wasm = true },
    .{ .name = "tri", .wasm = true },
    .{ .name = "cube", .wasm = true },
};
