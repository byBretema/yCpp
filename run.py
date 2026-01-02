import os
import sys
import shutil
import subprocess
import argparse

import vendor.y as y


# - - - - - - - - - - - - - - - - Globals  - - - - - - - - - - - - - - - - - - #


BUILD_DIR: str = "build"
TESTS_DIR: str = "tests"


# - - - - - - - - - - - - - - - - - Main - - - - - - - - - - - - - - - - - - - #


def title(msg, post_ln=1):
    nl = "\n" * post_ln if post_ln > 0 else ""
    y.println_fill("\n\n· {s} {t} {s} ·" f"{nl}", "-", t=msg)
    # y.log_fancy(msg, align=y.FancyAlign.Center, pre_ln=2, post_ln=post_ln, sep="#")


def main():
    y.setup(show_traceback=True)

    ############################################################################
    ##                               Args                                     ##
    ############################################################################

    parser = argparse.ArgumentParser(description="Build script")

    parser.add_argument("-t", "--tests", action="store_true", help="Run project tests")
    parser.add_argument("-b", "--build", action="store_true", help="Run config/build step")
    parser.add_argument("-c", "--cleanup", action="store_true", help="Cleanup sub-build directory")
    parser.add_argument("-C", "--fullcleanup", action="store_true", help="Cleanup build directory")
    parser.add_argument("-r", "--release", action="store_true", help="Build in Release mode")
    parser.add_argument("-g", "--cmakegen", type=str, default="Ninja", help="CMake generator")

    args = parser.parse_args()

    ############################################################################
    ##                               Vars                                     ##
    ############################################################################

    build_type: str = "Release" if args.release else "Debug"
    sub_build_dir: str = f"{BUILD_DIR}/project"
    cmake_gen: str = args.cmakegen

    ############################################################################
    ##                             Required                                   ##
    ############################################################################

    y.required_command("cmake")

    if cmake_gen == "Ninja":
        y.required_command("ninja")

    ############################################################################
    ##                             Cleanup                                    ##
    ############################################################################

    if args.fullcleanup and os.path.isdir(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)
    os.makedirs(BUILD_DIR, exist_ok=True)

    if args.cleanup and os.path.isdir(sub_build_dir):
        shutil.rmtree(sub_build_dir)
    os.makedirs(sub_build_dir, exist_ok=True)

    ############################################################################
    ##                          Config + Build                                ##
    ############################################################################

    if args.build:

        title("CONFIG")

        config_cmd: list[str] = [
            "cmake",
            "-G",
            cmake_gen,
            "../..",
            f"-DCMAKE_BUILD_TYPE={build_type}",
            "-DCMAKE_POLICY_VERSION_MINIMUM=3.10",
        ]
        y.run_cmd(config_cmd, cwd=sub_build_dir)

        title("BUILD")

        build_cmd: list[str] = [
            "cmake",
            "--build",
            ".",
            "-j",
            str(os.cpu_count()),
            "--config",
            build_type,
        ]

        y.run_cmd(build_cmd, cwd=sub_build_dir)

    ############################################################################
    ##                              Tests                                     ##
    ############################################################################

    if args.tests:

        title("TESTs", 0)
        tests_dir: str = os.path.abspath(f"{BUILD_DIR}/tests")

        tests_total: int = 0
        tests_passed: int = 0

        if os.path.isdir(tests_dir):

            for root, _, files in os.walk(tests_dir):
                for filename in files:
                    filepath: str = os.path.join(root, filename)

                    if y.file_is_binary(filepath):
                        tests_total += 1

                        p: y.RunCmdInfo = y.run_cmd([filepath], permissive=True, verbosity=0, external=True)

                        if p.stdout:
                            y.println_fill("\n· >> {m1} {s} ·", "·", m1=filename)
                        else:
                            y.println()

                        if p.stdout:
                            y.println(f"\n{p.stdout}")

                        ok: bool = not p.returncode

                        tests_passed += int(ok)

                        mark: str = "🟢" if ok else "🔴"
                        status: str = "PASS" if ok else "FAIL"

                        y.println_fill("· >> {m1} {s} {m2} ··· ·", "·", m1=filename, m2=f"{status} {mark}")

        ok: bool = tests_passed == tests_total

        mark: str = "✅️" if ok else "⛔️"
        status: str = "PASS" if ok else "FAIL"
        y.println()
        y.log_info(f"Passed Tests ({tests_passed}/{tests_total}) ❱ {status} {mark}")


# - - - - - - - - - - - - - - - Entrypoint - - - - - - - - - - - - - - - - - - #
y.entrypoint(main)
