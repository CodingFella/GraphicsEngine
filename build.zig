const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const graphi = b.addStaticLibrary(.{
        .name = "graphi",
        .target = target,
        .optimize = optimize,
    });
    graphi.addCSourceFile(.{
        .file = b.path("src/graphi.c"),
    });
    graphi.addIncludePath(b.path("include"));

    // b.addInstallArtifact(graphi, .{
    //     // .h_dir = .{ .override = .{ .lib =  } },
    // });
    b.installArtifact(graphi);

    // ----------------------------------------------------------------------

    inline for (examples) |example| {
        const exa = b.addExecutable(.{
            .optimize = optimize,
            .target = target,
            .name = example.name,
        });
        exa.addCSourceFile(.{
            .file = b.path("examples/" ++ example.file),
            .flags = &.{
                "-std=c11",
            },
        });
        exa.addIncludePath(b.path("include"));
        exa.linkLibC();
        exa.linkLibrary(graphi);

        const run = b.addRunArtifact(exa);
        const step = b.step("example-" ++ example.name, "run the " ++ example.name ++ " example");
        step.dependOn(&run.step);
    }

    // ----------------------------------------------------------------------

    const wasmtarget = b.resolveTargetQuery(.{
        .cpu_arch = .wasm32,
        // .os_tag = .wasi,
        .os_tag = .freestanding,
    });

    // const trans = b.addTranslateC(.{
    //     .root_source_file = b.path("wasm/wasm_2x2_cube.c"),
    //     .link_libc = true,
    //     .optimize = optimize,
    //     .target = wasmtarget,
    // });

    const wasm = b.step("wasm", "build the wasm example");
    const wexe = b.addExecutable(.{
        // .root_source_file = trans.getOutput(),
        .name = "abcd",
        .optimize = optimize,
        .target = wasmtarget,
    });

    wexe.addCSourceFile(.{
        .file = b.path("wasm/wasm_2x2_cube.c"),
        .flags = &.{
            // "--no-standard-libraries",
            "-Wl,--export-all",
            "-Wl,--no-entry",
            // "--no-entry",
        },
    });

    wexe.entry = .disabled;
    wexe.root_module.export_symbol_names = &.{"render"};

    // TODO: allow setting graphi target
    wexe.addCSourceFile(.{
        .file = b.path("src/graphi.c"),
    });
    wexe.addIncludePath(b.path("include"));

    const wasi = b.addInstallArtifact(wexe, .{});
    wasm.dependOn(&wasi.step);
}

const examples = .{.{
    .file = "timing.c",
    .name = "timing",
}};
