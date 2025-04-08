import os
import sys
import shutil
import subprocess
import argparse


def command_exists(cmd:str) -> bool:
    return shutil.which(cmd) is not None


def error_exit(message:str) -> None:
    print(f"[ERR] - {message}", file=sys.stderr)
    sys.exit(1)


def use_conan(enabled:bool, build_type:str) -> str:
    if not enabled:
        return ""

    conan_profile = os.path.expanduser("~/.conan2/profiles/default")

    if not os.path.exists(conan_profile):
        subprocess.run(["uvx", "conan", "profile", "detect"], check=True)

    subprocess.run([
        "uvx", "conan", "install", "..", f"-s=build_type={build_type}", "--build=missing", "-of=."]
    , check=True)

    preset = "conan-default" if os.name == "nt" else f"conan-{build_type.lower()}"
    return preset


def main():
    parser = argparse.ArgumentParser(description="Build script")
    parser.add_argument("-c", "--cleanup", action="store_true", help="Cleanup sub-build directory")
    parser.add_argument("-C", "--fullcleanup", action="store_true", help="Cleanup build directory")
    parser.add_argument("-r", "--release", action="store_true", help="Build in Release mode")
    parser.add_argument("-g", "--cmakegen", type=str, default="Ninja", help="CMake generator")
    parser.add_argument("-a", "--useconan", action="store_true", help="Prefer conan for dependencies")
    args = parser.parse_args()

    # Check required commands
    if not command_exists("cmake"):
        error_exit("Command 'cmake' is required")
    if args.useconan and not command_exists("uvx"):
        error_exit("Command 'uvx' is required (https://docs.astral.sh/uv/#installation)")

    build_type = "Release" if args.release else "Debug"
    cmake_gen = args.cmakegen

    if cmake_gen == "Ninja" and not command_exists("ninja"):
        error_exit("Command 'ninja' is required")

    build_parent_dir = "build"
    build_dir = f"{build_parent_dir}/sub-build"

    # Cleanup
    if args.fullcleanup and os.path.isdir(build_parent_dir):
        shutil.rmtree(build_parent_dir)
    if args.cleanup and os.path.isdir(build_dir):
        shutil.rmtree(build_dir)
    os.makedirs(build_parent_dir, exist_ok=True)
    os.makedirs(build_dir, exist_ok=True)

    # Change to build directory
    os.chdir(build_dir)

    # Deps management :: CMake or Conan
    preset = use_conan(args.useconan, build_type)

    # Config
    config_cmd = ["cmake", "-G", cmake_gen, "../..", f"-DCMAKE_BUILD_TYPE={build_type}"]
    if preset != "":
        config_cmd.append(f"--preset={preset}")
    subprocess.run(config_cmd, check=True)

    # Build
    subprocess.run(["cmake", "--build", ".", "-j", str(os.cpu_count()), "--config", build_type], check=True)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        error_exit("User interrupts execution.")
    except PermissionError:
        error_exit("Some files are in use, cannot run the script.")
    except Exception as e:
        error_exit(f"Unexpected | {type(e)}\n\t    {str(e)}")
