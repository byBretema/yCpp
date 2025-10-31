import os
import sys
import shutil
import subprocess
import argparse
from typing import NoReturn, Callable
import traceback

# import mimetypes
# from pathlib import Path

################################################################################
# Globals

BUILD_DIR = "build"
TESTS_DIR = "tests"


################################################################################
def print_deco(
    title: str | None,
    pre_ln: bool = False,
    post_ln: bool = False,
    align_right: bool = False,
    sep_offset: int = 0,  # len() could miss-count when the string contains emojis
):

    template = "-- ❱{t}{s}❰"
    if title:
        template = "-- ❱ {t} ❰{s}❰"
        if align_right:
            template = "-- ❱{s}❱ {t} ❰"
    else:
        title = ""

    template_with_title = template.format(t=title, s="")
    n_sep = max(80 - len(template_with_title) - sep_offset, 0)
    sep = "-" * n_sep

    if pre_ln:
        print("")

    print(template.format(t=title, s=sep))

    if post_ln:
        print("")


################################################################################
def _is_required(item: str, check_func: Callable[[str], bool | str | None], item_type: str, info: str = ""):
    info = f" {info}" if info else ""
    if not check_func(item):
        error_exit(f"{item_type.capitalize()} '{item}' is required.{info}")
    print(f"@ Found: {item}")


################################################################################
def command_required(cmd: str, info: str = ""):
    _is_required(cmd, shutil.which, "Command", info)


################################################################################
def file_required(path: str, info: str = ""):
    _is_required(path, os.path.isfile, "File", info)


################################################################################
def folder_required(path: str, info: str = ""):
    _is_required(path, os.path.isdir, "Folder", info)


################################################################################
def error_exit(message: str, exception=None, traceback_on_fail: bool = True) -> NoReturn:
    sep = "\n\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n"

    exception_err = f" | {type(exception)}\n{str(exception)}" if exception else ""
    err = f"{sep}@ ERROR | {message}{exception_err}"

    print(err, file=sys.stderr)

    if exception and traceback_on_fail:
        t_sep = "\n\nttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt\n\n"
        print(f"{t_sep}{traceback.format_exc()}", file=sys.stderr)

    print()
    sys.exit(1)


################################################################################
def is_binary_file(file_path, chunk_size=1024, null_byte_threshold=0.1):
    if not os.path.exists(file_path):
        return False

    try:
        with open(file_path, "rb") as f:
            chunk = f.read(chunk_size)
    except IOError:
        return False

    null_count = chunk.count(b"\x00")
    if null_count / len(chunk) > null_byte_threshold:
        return True


################################################################################
def main():

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

    build_type = "Release" if args.release else "Debug"
    sub_build_dir = f"{BUILD_DIR}/project"
    cmake_gen = args.cmakegen

    ############################################################################
    # Check required commands

    command_required("cmake")

    if cmake_gen == "Ninja":
        command_required("ninja")

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

    print(f"\n\n# CONFIG\n")
    print_deco(None)
    config_cmd = ["cmake", "-G", cmake_gen, "../..", f"-DCMAKE_BUILD_TYPE={build_type}", "-DCMAKE_POLICY_VERSION_MINIMUM=3.10"]
    subprocess.run(config_cmd, check=True, cwd=sub_build_dir)
    print_deco(None)

    print(f"\n\n# BUILD\n")
    build_cmd = ["cmake", "--build", ".", "-j", str(os.cpu_count()), "--config", build_type]
    subprocess.run(build_cmd, check=True, cwd=sub_build_dir)

    ############################################################################
    # Tests

    if args.tests:
        print(f"\n\n# TESTs")
        tests_dir = os.path.abspath(f"{BUILD_DIR}/tests")

        tests_total = 0
        tests_passed = 0

        if os.path.isdir(tests_dir):

            for root, _, files in os.walk(tests_dir):
                for filename in files:
                    filepath = os.path.join(root, filename)

                    if is_binary_file(filepath):
                        tests_total += 1

                        print_deco(filename, pre_ln=True)

                        p = subprocess.run([filepath], capture_output=True)
                        ok = not p.returncode

                        tests_passed += int(ok)

                        mark = "🟢" if ok else "🔴"
                        status = "PASS" if ok else "FAIL"
                        print_deco(f"{status}  {mark} ", align_right=True, sep_offset=1)

        ok = tests_passed == tests_total

        mark = "✅️" if ok else "⛔️"
        status = "PASS" if ok else "FAIL"
        print(f"\n-- Passed Tests ({tests_passed}/{tests_total}) ❱ {status} {mark}")

    ############################################################################


################################################################################
if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        error_exit("User interrupts execution")
    except PermissionError as e:
        error_exit("Files in use / Folder not found", e)
    except Exception as e:
        error_exit(f"Unexpected", e)
