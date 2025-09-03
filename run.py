import os
import sys
import shutil
import subprocess
import argparse
import mimetypes

BUILD_DIR = "build"
TESTS_DIR = "tests"


def command_exists(cmd: str) -> bool:
    return shutil.which(cmd) is not None


def error_exit(message: str) -> None:
    print(f"[ERR] - {message}", file=sys.stderr)
    sys.exit(1)


def is_binary_by_mimetype(filepath):
    try:
        mime_type, _ = mimetypes.guess_type(filepath)
        if mime_type:
            return not mime_type.startswith('text/')
    except:
        pass
    return False


def main():
    parser = argparse.ArgumentParser(description="Build script")
    parser.add_argument(
        "-c", "--cleanup", action="store_true", help="Cleanup sub-build directory"
    )
    parser.add_argument(
        "-C", "--fullcleanup", action="store_true", help="Cleanup build directory"
    )
    parser.add_argument(
        "-r", "--release", action="store_true", help="Build in Release mode"
    )
    parser.add_argument(
        "-g", "--cmakegen", type=str, default="Ninja", help="CMake generator"
    )
    args = parser.parse_args()

    # Check required commands
    if not command_exists("cmake"):
        error_exit("Command 'cmake' is required")

    build_type = "Release" if args.release else "Debug"
    cmake_gen = args.cmakegen

    if cmake_gen == "Ninja" and not command_exists("ninja"):
        error_exit("Command 'ninja' is required")

    sub_build_dir = f"{BUILD_DIR}/project"

    # Cleanup
    if args.fullcleanup and os.path.isdir(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)
    os.makedirs(BUILD_DIR, exist_ok=True)

    if args.cleanup and os.path.isdir(sub_build_dir):
        shutil.rmtree(sub_build_dir)
    os.makedirs(sub_build_dir, exist_ok=True)

    # Change to build directory
    os.chdir(sub_build_dir)

    # Config
    config_cmd = ["cmake", "-G", cmake_gen, "../..", f"-DCMAKE_BUILD_TYPE={build_type}"]
    subprocess.run(config_cmd, check=True)

    # Build
    build_cmd = [
        "cmake",
        "--build",
        ".",
        "-j",
        str(os.cpu_count()),
        "--config",
        build_type,
    ]
    subprocess.run(build_cmd, check=True)

    # Tests
    tests_dir = f"{BUILD_DIR}/tests"

    if os.path.isdir(tests_dir):
        for root, _, files in os.walk(tests_dir):
            for filename in files:
                filepath = os.path.join(root, filename)
                if is_binary_by_mimetype(filepath):
                    print(f"-- Running TEST : {filename}")

    # Cleanup
    if os.path.isdir(tests_dir):


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        error_exit("User interrupts execution.")
    except PermissionError:
        error_exit("Some files are in use, cannot run the script.")
    except Exception as e:
        error_exit(f"Unexpected | {type(e)}\n\t    {str(e)}")
