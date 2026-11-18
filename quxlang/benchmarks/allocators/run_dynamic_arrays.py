#!/usr/bin/env python3
"""The runner compares runtime-sized array allocation with a matched C++ workload and verifies payload checksums."""

import argparse
import json
import os
from pathlib import Path
import platform
import shutil
import statistics
import subprocess

from run import add_cpp_build_arguments, cpp_build_flags, ROOT, REPOSITORY, counter_description, digest, host_configuration, measure_case, minimum_seconds, prepare_executables, mimalloc_documented_interface_flags


PATTERNS = {'fixed': 0, 'random': 1, 'growth': 2}
SIZES = tuple(1 << bits for bits in range(4, 31))


def main():
    """The runner builds both implementations and records interleaved samples of identical work."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--work-dir', type=Path, default=REPOSITORY / 'tmp/allocator-comparison-out/dynamic-arrays')
    parser.add_argument('--qxc', type=Path, default=REPOSITORY / 'misc/build/buildspaces/system-clang-libcxx/quxlang/Release/qxc')
    parser.add_argument('--mimalloc-source', type=Path, required=True)
    parser.add_argument('--sizes', type=int, nargs='+', choices=SIZES, metavar='BYTES',
                        default=[64, 1024, 8192, 16384, 65536, 1048576],
                        help='Maximum array size in bytes; a power of two from 16 to 1073741824.')
    parser.add_argument('--windows', type=int, nargs='+', default=[64])
    parser.add_argument('--patterns', nargs='+', choices=PATTERNS, default=list(PATTERNS))
    parser.add_argument('--operations', type=int, default=1000000)
    parser.add_argument('--repetitions', type=int, default=3)
    parser.add_argument('--min-seconds', type=minimum_seconds, default=3.0)
    parser.add_argument('--build-only', action='store_true')
    add_cpp_build_arguments(parser)
    args = parser.parse_args()
    if args.operations < 1 or args.repetitions < 1 or any(w < 1 or w > 4096 for w in args.windows):
        parser.error('operations and repetitions must be positive; windows must be 1..4096')
    work = args.work_dir.resolve()
    work.mkdir(parents=True, exist_ok=True)
    bundle = work / 'bundle'
    standard = REPOSITORY / 'quxlang/tests/testdata/testbundle/modules'
    for name in ('std', 'runtime', 'syscall', 'posix', 'io'):
        shutil.copytree(standard / name, bundle / 'modules' / name, dirs_exist_ok=True)
    shutil.copytree(ROOT / 'modules/main', bundle / 'modules/main', dirs_exist_ok=True)
    config = (ROOT / 'qxcbuild.yml').read_text().replace('benchmark: "::allocations"', 'benchmark: "::dynamic_arrays"')
    (bundle / 'qxcbuild.yml').write_text(host_configuration(config))
    commands = []

    def execute(command, **kwargs):
        commands.append([str(value) for value in command])
        subprocess.run(command, check=True, **kwargs)

    qxc = args.qxc.resolve()
    mimalloc = args.mimalloc_source.resolve()
    cpp_flags, cpp_linker_flags = cpp_build_flags(args)
    with (work / 'qxc-build.log').open('w') as log:
        execute([qxc, 'build', bundle, work / 'quxlang', 'native'], stdout=log, stderr=subprocess.STDOUT)
    execute(['clang', '-std=c17', '-O3', *cpp_flags, '-DNDEBUG', '-DMI_STATIC_LIB', *mimalloc_documented_interface_flags(), '-I' + str(mimalloc / 'include'),
             '-c', mimalloc / 'src/static.c', '-o', work / 'mimalloc.o'])
    execute(['clang++', '-std=c++17', '-O3', *cpp_flags, *cpp_linker_flags, '-fsized-deallocation', '-DNDEBUG', '-DUSE_MIMALLOC', '-DMI_STATIC_LIB', '-pthread',
             '-fno-builtin-malloc', '-fno-builtin-free', '-I' + str(mimalloc / 'include'),
             ROOT / 'dynamic_arrays.cpp', ROOT / 'barriers.S', work / 'mimalloc.o', '-o', work / 'cpp_mimalloc'])
    binaries = {'quxlang_alloc': work / 'quxlang/output/allocations', 'cpp_mimalloc': work / 'cpp_mimalloc'}
    sdk = prepare_executables(binaries, execute)
    environment = {key: value for key, value in os.environ.items() if not key.startswith(('Malloc', '_Malloc', 'MIMALLOC_'))}
    report = {'platform': platform.platform(), 'logical_cpus': os.cpu_count(), 'commands': commands,
              'cpp_target': args.cpp_target, 'cpp_lto': args.cpp_lto,
              'clang': subprocess.check_output(['clang', '--version'], text=True),
              'workload': 'Every array has a runtime element count of 64-bit words. Fixed retains a rolling window of arrays of the maximum size. Random retains a rolling window of arrays whose element counts have uniformly distributed binary magnitude up to the maximum size. Growth repeats the doubling capacity sequence of an appended array from one element to the maximum size without copying elements. Every array receives its sequence number in its first and last elements through opaque word writes, and both elements contribute to the release-time checksum.',
              'timing': 'Window storage is created outside timing. One thread measures the complete allocation loop. The work count is allocations for fixed and random and complete growth sequences for growth.',
              'counter': counter_description(), 'sdk_version': sdk, 'minimum_sample_seconds': args.min_seconds,
              'repetitions': args.repetitions, 'qxc_sha256': digest(qxc), 'measurement_sha256': digest(ROOT.parent / 'measurement.py'),
              'mimalloc_revision': subprocess.check_output(['git', '-C', mimalloc, 'rev-parse', 'HEAD'], text=True).strip(),
              'binary_sha256': {name: digest(path) for name, path in binaries.items()},
              'source_sha256': {str(path.relative_to(bundle)): digest(path) for path in sorted(bundle.rglob('*.qxs'))},
              'cpp_source_sha256': digest(ROOT / 'dynamic_arrays.cpp'), 'results': []}
    destination = work / 'results.json'
    destination.write_text(json.dumps(report, indent=2) + '\n')
    if args.build_only:
        return
    for size in args.sizes:
        for pattern in args.patterns:
            for window in (args.windows if pattern != 'growth' else args.windows[:1]):
                checksums = {}

                def sample(name, count):
                    options = [f'--n={count}', f'--size={size}', f'--batch={window}', f'--pattern={PATTERNS[pattern]}'] if name == 'quxlang_alloc' else [str(count), str(size), str(window), str(PATTERNS[pattern])]
                    output = subprocess.check_output([binaries[name], *options], text=True, env=environment, timeout=3600)
                    ticks, frequency, checksum = map(int, output.split())
                    if checksums.setdefault(count, checksum) != checksum or ticks <= 0 or frequency <= 0:
                        raise RuntimeError(f'{name}: checksum differs between implementations or timer is invalid: {output!r}')
                    seconds = ticks / frequency
                    print(f'{pattern} bytes={size} window={window} {name} n={count} seconds={seconds:.6f}', flush=True)
                    return {'seconds': seconds, 'frequency_hz': frequency, 'checksum': checksum}

                result = measure_case(binaries, sample, args.operations, args.repetitions, args.min_seconds)
                samples = {name: [row['seconds'] for row in rows] for name, rows in result['samples'].items()}
                medians = {name: statistics.median(rows) for name, rows in samples.items()}
                report['results'].append({'bytes': size, 'window': window, 'pattern': pattern,
                                          'work': result['work'], 'sample_seconds': samples, 'median_seconds': medians,
                                          'median_ns_per_work': {name: value * 1e9 / result['work'] for name, value in medians.items()},
                                          'calibration': result['calibration'], 'run_order': result['run_order'],
                                          'checksum': checksums[result['work']]})
                destination.write_text(json.dumps(report, indent=2) + '\n')
                print(f'{pattern} bytes={size} window={window}: ' + ', '.join(f'{name}={value * 1e9 / result["work"]:.1f} ns' for name, value in medians.items()), flush=True)


if __name__ == '__main__':
    main()
