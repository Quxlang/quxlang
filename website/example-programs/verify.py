#!/usr/bin/env python3
"""Builds every example program with qxc and compares its standard output with the recorded output.

Usage: verify.py <path-to-qxc> [program-name ...]
                 [--programs-directory=<directory>] [--work-directory=<directory>]

The script assembles a source bundle in the work directory, builds it for the host platform, runs
each selected program, and reports every program whose output differs from
<programs-directory>/<name>/expected-output.txt. The exit status is zero only when every program
matches. The programs directory defaults to the "programs" directory beside this script, which
holds the programs of the Example Programs section. The "reference-programs" directory holds the
examples of the Standard Modules Reference pages. The work directory defaults to
<repository>/tmp/example-programs and is deleted before each run.
"""

import argparse
import pathlib
import platform
import shutil
import subprocess
import sys

STANDARD_MODULES = ["runtime", "syscall", "posix", "std", "io", "net"]
LOGICAL_MODULE_NAMES = {"runtime": "RUNTIME"}

HOST_TARGETS = {
    ("Darwin", "arm64"): ["platform: macos", "cpu: ARM64", "macho_config:", "  minimum_os_version: \"14.4\""],
    ("Linux", "x86_64"): ["platform: linux", "cpu: x64"],
    ("Linux", "aarch64"): ["platform: linux", "cpu: ARM64"],
}


def main() -> int:
    example_root = pathlib.Path(__file__).resolve().parent
    repository_root = example_root.parent.parent
    parser = argparse.ArgumentParser(description="Builds and runs the documentation example programs.")
    parser.add_argument("qxc", type=pathlib.Path)
    parser.add_argument("programs", nargs="*")
    parser.add_argument("--programs-directory", type=pathlib.Path, default=example_root / "programs")
    parser.add_argument("--work-directory", type=pathlib.Path, default=repository_root / "tmp" / "example-programs")
    arguments = parser.parse_args()
    qxc = arguments.qxc.resolve()
    programs_root = arguments.programs_directory.resolve()
    standard_modules_root = repository_root / "quxlang" / "tests" / "testdata" / "testbundle" / "modules"

    all_programs = sorted(entry.name for entry in programs_root.iterdir() if entry.is_dir())
    selected_programs = arguments.programs or all_programs
    unknown_programs = sorted(set(selected_programs) - set(all_programs))
    if unknown_programs:
        print("unknown programs: " + ", ".join(unknown_programs))
        return 2

    host = (platform.system(), platform.machine())
    if host not in HOST_TARGETS:
        print(f"unsupported host: {host}")
        return 2

    work_root = arguments.work_directory.resolve()
    bundle_root = work_root / "bundle"
    output_root = work_root / "out"
    for stale_directory in (bundle_root, output_root):
        if stale_directory.exists():
            shutil.rmtree(stale_directory)
    (bundle_root / "modules").mkdir(parents=True)

    for module in STANDARD_MODULES:
        shutil.copytree(standard_modules_root / module, bundle_root / "modules" / module)
    for program in selected_programs:
        shutil.copytree(programs_root / program, bundle_root / "modules" / program / "sources",
                        ignore=shutil.ignore_patterns("expected-output.txt"))

    lines = ["targets:", "  native:"]
    lines += ["    " + line for line in HOST_TARGETS[host]]
    lines += ["    backend: llvm", "    modules:"]
    for module in STANDARD_MODULES:
        lines.append(f"      {LOGICAL_MODULE_NAMES.get(module, module)}: {{source: {module}}}")
    for program in selected_programs:
        lines.append(f"      {program}: {{source: {program}}}")
    lines.append("outputs:")
    for program in selected_programs:
        lines += [f"  {program}:", "    target: native", "    type: executable", f"    main_module: {program}",
                  "    backend_llvm_options:", "      build_type: Release"]
    (bundle_root / "qxcbuild.yml").write_text("\n".join(lines) + "\n")

    build = subprocess.run([str(qxc), "build", str(bundle_root), str(output_root)])
    if build.returncode != 0:
        print("qxc build failed")
        return 1

    failures = []
    for program in selected_programs:
        expected = (programs_root / program / "expected-output.txt").read_text()
        try:
            run = subprocess.run([str(output_root / "output" / program)], capture_output=True, text=True, timeout=60)
        except subprocess.TimeoutExpired:
            failures.append(program)
            print(f"FAIL {program}: timed out")
            continue
        if run.returncode != 0 or run.stdout != expected:
            failures.append(program)
            print(f"FAIL {program}: exit status {run.returncode}")
            print("--- expected")
            print(expected, end="")
            print("--- actual")
            print(run.stdout, end="")
            if run.stderr:
                print("--- standard error")
                print(run.stderr, end="")
        else:
            print(f"PASS {program}")

    print(f"{len(selected_programs) - len(failures)} of {len(selected_programs)} programs passed")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
