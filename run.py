import os
import sys
import shutil
import subprocess
import argparse

import vendor.y as y


BUILD_DIR: str = "build"
TESTS_DIR: str = "tests"


def main():
    y.setup()

    ############################################################################
    # Args

    parser = argparse.ArgumentParser(description="Build script")

    parser.add_argument("-t", "--tests", action="store_true", help="Run project tests")
    parser.add_argument("-c", "--cleanup", action="store_true", help="Cleanup sub-build directory")
    parser.add_argument("-C", "--fullcleanup", action="store_true", help="Cleanup build directory")
    parser.add_argument("-r", "--release", action="store_true", help="Build in Release mode")
    parser.add_argument("-g", "--cmakegen", type=str, default="Ninja", help="CMake generator")

    args = parser.parse_args()

    ############################################################################
    # Variables

    build_type: str = "Release" if args.release else "Debug"
    sub_build_dir: str  = f"{BUILD_DIR}/project"
    cmake_gen: str = args.cmakegen

    ############################################################################
    # Check required commands

    y.required_command("cmake")

    if cmake_gen == "Ninja":
        y.required_command("ninja")

    ############################################################################
    # PreBuild Cleanup

    if args.fullcleanup and os.path.isdir(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)
    os.makedirs(BUILD_DIR, exist_ok=True)

    if args.cleanup and os.path.isdir(sub_build_dir):
        shutil.rmtree(sub_build_dir)
    os.makedirs(sub_build_dir, exist_ok=True)

    ############################################################################
    # Config + Build

    y.log_trace(f"CONFIG")
    y.log_fancy(None)
    config_cmd: list[str] = [
        "cmake",
        "-G",
        cmake_gen,
        "../..",
        f"-DCMAKE_BUILD_TYPE={build_type}",
        "-DCMAKE_POLICY_VERSION_MINIMUM=3.10",
    ]
    y.run_cmd(config_cmd, cwd=sub_build_dir)
    y.log_fancy(None)

    y.log_trace(f"BUILD")
    build_cmd: list[str] = ["cmake", "--build", ".", "-j", str(os.cpu_count()), "--config", build_type]
    y.run_cmd(build_cmd, cwd=sub_build_dir)

    ############################################################################
    # Tests

    if args.tests:
        y.log_trace(f"TESTs")
        tests_dir: str = os.path.abspath(f"{BUILD_DIR}/tests")

        tests_total: int = 0
        tests_passed: int = 0

        if os.path.isdir(tests_dir):

            for root, _, files in os.walk(tests_dir):
                for filename in files:
                    filepath: str = os.path.join(root, filename)

                    if y.file_is_binary(filepath):
                        tests_total += 1

                        y.log_fancy(filename, pre_ln=True)

                        p: y.RunCmdInfo = y.run_cmd([filepath], permissive=True, verbosity=0, is_external=True)

                        if p.stdout:
                            y.log_info(f"\n{p.stdout}", "")

                        ok: bool = not p.returncode

                        tests_passed += int(ok)

                        mark: str = "🟢" if ok else "🔴"
                        status: str = "PASS" if ok else "FAIL"
                        y.log_fancy(f"{status}  {mark} ", align_right=True, sep_offset=1)

        ok: bool = tests_passed == tests_total

        mark: str = "✅️" if ok else "⛔️"
        status: str = "PASS" if ok else "FAIL"
        y.log_info("", "")
        y.log_info(f"Passed Tests ({tests_passed}/{tests_total}) ❱ {status} {mark}")


################################################################################
y.entrypoint(main)
