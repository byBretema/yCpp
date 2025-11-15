import os
import sys
import platform
import shutil
import subprocess
from pathlib import Path
import tempfile
from typing import NoReturn, Callable, Generator
import glob
import json
import traceback
from collections import namedtuple
import enum


# --- Globals ---


################################################################################
EXIT_CB: Callable | None = None
TRACE_CB: Callable | None = None
PROCESS_CB: Callable | None = None
PROCESS_TH: int = 10
PROCESS_FILEPATH: str | None = None


# --- Logger ---

################################################################################
__GLOBAL_TEE: Generator[None, str, None] | None = None
__GLOBAL_TEE_WITH_CALLBACK: Generator[None, str, None] | None = None


################################################################################
def tee(msg: str, run_cb: bool = False):
    tee_ = __GLOBAL_TEE_WITH_CALLBACK if run_cb else __GLOBAL_TEE
    if tee_:
        tee_.send(msg)


################################################################################
def __tee_generator(
    log_filepath: str | None = None,
    log_threshold: int = 1,
    closure: Callable | None = None,
) -> Generator[None, str, None]:

    count: int = 0
    partial: str = ""
    log_threshold = 1 if log_threshold < 1 else log_threshold

    if log_filepath and not os.path.isfile(log_filepath):
        raise Exception("LogFile not found")

    try:
        while True:
            # Blokcs till Generator.send("your-message")
            line = yield
            # Line has been received, do the 'tee'
            if line:
                sys.stdout.write(line)
                sys.stdout.flush()
                count += 1
                partial += line
                if log_filepath:
                    with open(log_filepath, "a") as file:
                        file.write(line)
            # Threshold reached, trigger closure
            if count >= log_threshold:
                if partial and closure:
                    closure(partial)
                count = 0
                partial = ""

    # This gets triggered calling Generator.close()
    finally:
        if partial and closure:
            closure(partial)


################################################################################
def tee_new(log_filepath: str | None = None, log_threshold: int = 1, closure: Callable | None = None):
    t = __tee_generator(log_filepath, log_threshold, closure)
    next(t)
    return t


################################################################################
def log_error(message: str, exception=None, traceback_on_fail: bool = True) -> None | NoReturn:

    e_sep = "\n\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n"
    exception_err = f" | {type(exception)}\n{str(exception)}" if exception else ""
    err = f"{e_sep}@ ERROR | {message}{exception_err}"

    tee(err)

    if EXIT_CB:
        EXIT_CB(err)

    if exception and traceback_on_fail:
        t_sep = "\n\nttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt\n\n"
        tee(f"{t_sep}{traceback.format_exc()}")

    tee("")
    if exit:
        sys.exit(1)
    else:
        tee("- - - - - Execution will continue below - - - - -\n")


################################################################################
def log_trace(msg: str, publish: bool = True):
    tee(f"\n@ {msg}\n\n")


################################################################################
def log_info(msg: str, prefix="-- "):
    tee(f"{prefix}{msg}\n")


################################################################################
def log_fancy(
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


# --- Run Command ---


################################################################################
def run_command(
    cmd: list[str],
    cwd: str | None = None,
    shell: bool = False,
    err_info: str = "",
    verbosity: int = 2,
    permissive: bool = False,
    is_external: bool = False,
):
    cmd_str = " ".join(cmd)
    cmd_str += "" if not cwd else f"  (at {cwd})"
    if verbosity > 0:
        log_trace(cmd_str)

    if is_external:
        cmd = ["stdbuf", "-oL"] + cmd

    process = subprocess.Popen(
        cmd,
        shell=shell,  # Process input as a shell (to support &&, |, <<, etc)
        stdout=subprocess.PIPE,  # Capture stdout
        stderr=subprocess.STDOUT,  # Merge stderr into stdout
        text=True,  # Decode output to strings
        bufsize=1,  # Line-buffered pipe reading
        cwd=cwd,  # Set folder where execute the command
        env=os.environ.copy(),
    )

    stdout = ""

    if process.stdout:
        _tee = tee_new(PROCESS_FILEPATH, PROCESS_TH, PROCESS_CB)
        for line in iter(process.stdout.readline, ""):
            stdout += line
            if verbosity > 1:
                _tee.send(line)
        _tee.close()
        process.stdout.close()

    returncode = process.wait()
    if returncode:
        output = f"\n{stdout}" if stdout and verbosity < 2 else ""
        err_info = f"{err_info}. " if err_info else ""
        err_info = f"{err_info}Command failed with return code: {returncode}{output}"
        if not permissive:
            log_error(err_info)
        elif verbosity > 1:
            log_info(err_info)

    RunCmdResult = namedtuple("RunCmdResult", "stdout returncode")
    return RunCmdResult(stdout, returncode)


# --- Check required helpers ---


################################################################################
def _required_base(item: str, cb: Callable[[str], bool | str | None], type: str, info: str = "", silent: bool = False):
    info = f" {info}" if info else ""
    if not cb(item):
        log_error(f"{type.capitalize()} '{item}' is required.{info}")
    if not silent:
        log_info(f"Found: {item}")


################################################################################
def required_command(cmd: str, info: str = "", silent: bool = False):
    _required_base(cmd, shutil.which, "Command", info, silent)


################################################################################
def required_file(path: str, info: str = "", silent: bool = False):
    _required_base(path, os.path.isfile, "File", info, silent)


################################################################################
def required_folder(path: str, info: str = "", silent: bool = False):
    _required_base(path, os.path.isdir, "Folder", info, silent)


# --- Conversions ---


################################################################################
def to_int(s: str, fallback: int):
    try:
        return int(s)
    except ValueError:
        return fallback


################################################################################
def to_bool(v):
    if isinstance(v, str):
        return v.lower() == "true"
    else:
        return bool(v)


# --- ENV Utilities ---


################################################################################
def env_path_add(to_add: list[str] | str):
    if isinstance(to_add, str):
        to_add_list = [to_add]
    else:
        to_add_list = to_add
    prev_path = [os.environ.get("PATH", "")]
    os.environ["PATH"] = os.pathsep.join(to_add_list + prev_path)


# --- File Utilities ---


################################################################################
def file_is_binary(file_path, chunk_size=1024, null_byte_threshold=0.1):
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


# --- OS Utilities ---


################################################################################
def os_tempfile(filename: str, check: bool = True):
    try:
        filepath = Path(os.path.join(tempfile.gettempdir(), filename))
        filepath.touch()
        return str(filepath)
    except Exception as e:
        if check:
            log_error(f"Failed to create temp file: {e}")
        return ""


################################################################################
def os_home():
    return str(Path.home()).replace("\\", "/")


################################################################################
def mac_bundle(start_path: str, app_name: str, is_bundle: bool = True):
    # Mac paths
    if is_bundle:
        bundle = f"{start_path}/{app_name}.app"
        contents = f"{bundle}/Contents"
        frameworks = f"{contents}/Frameworks"
        bin = f"{contents}/MacOS/{app_name}"
        # Ensure Frameworks path exists and, consequently, Contents.
        os.makedirs(frameworks, exist_ok=True)
    else:
        bundle = start_path
        contents = start_path
        frameworks = start_path
        bin = f"{start_path}/{app_name}"
    # Return them as class-like object with fields
    MacBundle = namedtuple("MacBundle", "bundle contents frameworks bin")
    return MacBundle(bundle, contents, frameworks, bin)


################################################################################
def os_binpath(start_path: str, bin_name: str, macos_get_bundle: bool = False) -> str:
    p = platform.system().lower()

    if p == "windows":
        return f"{start_path}/{bin_name}.exe"

    if p == "linux":
        return f"{start_path}/{bin_name}"

    if p == "darwin":
        mac = mac_bundle(start_path, bin_name)
        if macos_get_bundle:
            return mac.bundle
        else:
            return mac.bin

    return ""


################################################################################
def os_copy_to(src: str, dst: str, only_content: bool = True):

    if os.path.isdir(src):
        log_info(f"Copying dir: {src} -> {dst}")
        if only_content:
            shutil.copytree(src, dst, dirs_exist_ok=True)
        else:
            folder_name = os.path.basename(src)
            shutil.copytree(src, f"{dst}/{folder_name}", dirs_exist_ok=True)

    else:
        log_info(f"Copying file: {src} -> {dst}")
        shutil.copy(src, dst)


################################################################################
class GlobSort(enum.Enum):
    Nope = enum.auto()
    Alpha = enum.auto()
    VerNum = enum.auto()


################################################################################
def os_glob(
    pattern: str,
    sort: GlobSort = GlobSort.Alpha,
    rev: bool = False,
    err_info: str = "",
) -> str | NoReturn:
    glob_res = glob.glob(pattern)

    if not sort is GlobSort.Nope:
        sort_algo = None  # If None its default is Alphabectically

        if sort is GlobSort.VerNum:
            rev = True
            sort_algo = lambda s: [int(u) for u in s.split(os.sep)[-1].split(".")]

        glob_res.sort(key=sort_algo, reverse=rev)

    final_path: str = glob_res[0].replace("\\", "/") if glob_res else ""

    if not final_path:
        err_info = f" {err_info}" if err_info else ""
        log_error(f"Glob '{pattern}' has failed.{err_info}")

    return final_path


# --- Tools ---


################################################################################
def zip_it(dst: str, src: str):
    cmd = ["7z", "a", "-tzip", "-bso0", "-bsp0", "-bse1"]
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    run_command([*cmd, dst, src], verbosity=1)


# --- AWS ---


################################################################################
def aws_copy_file(filepath: str, aws_filepath: str, content_type: str | None = None):

    cmd = ["aws", "s3", "cp", "--cache-control", "max-age:no-cache", filepath, aws_filepath]

    if content_type:
        cmd.insert(5, content_type)
        cmd.insert(5, "--content-type")

    run_command(cmd, verbosity=0, err_info=f"Could not update {filepath} to AWS", permissive=True)


################################################################################
def aws_auth(pem_filepath: str, key_filepath: str, trust_arn: str, profile_arn: str, role_arn: str, info: str = ""):

    info = int(len(info) < 1) * " " + f" {info}"

    required_file(pem_filepath, f"Missing AWS certificate file.{info}")
    required_file(key_filepath, f"Missing AWS private-key file.{info}")

    required_command(
        "aws_signing_helper",
        f"Get from https://github.com/aws/rolesanywhere-credential-helper/releases "
        "or 'chmod +x aws_signing_helper' if it's already in the path'",
    )

    aws_auth_cmd: list[str] = [
        "aws_signing_helper",
        "credential-process",
        "--certificate",
        pem_filepath,
        "--private-key",
        key_filepath,
        "--trust-anchor-arn",
        trust_arn,
        "--profile-arn",
        profile_arn,
        "--role-arn",
        role_arn,
    ]
    aws_auth_output: str = run_command(aws_auth_cmd, verbosity=0).stdout
    aws_auth: dict = json.loads(aws_auth_output)

    for key in ["AccessKeyId", "SecretAccessKey", "SessionToken"]:
        if not key in aws_auth:
            log_error(f"Missing key '{key}' during AWS Auth")

    os.environ["AWS_ACCESS_KEY_ID"] = aws_auth["AccessKeyId"]
    os.environ["AWS_SECRET_ACCESS_KEY"] = aws_auth["SecretAccessKey"]
    os.environ["AWS_SESSION_TOKEN"] = aws_auth["SessionToken"]


# --- Qt Classes & Functions ---


################################################################################
class QtInfo:
    def __init__(self):
        self.path: str = ""
        self.tools_path: str = ""
        self.plugins_path: str = ""
        self.deployqt: str = ""  # As path in windows and a command in macos
        self.cmake_defs: list[str] = []

        p = platform.system().lower()

        if p == "windows":
            self.path = f"{Path.home().drive}/Qt"
            required_folder(self.path)

            self.tools_path = f"{self.path}/Tools"
            required_folder(self.tools_path)

            qt_version_path = os_glob(f"{self.path}/*.*.*", GlobSort.VerNum)
            qt_msvc_path = os_glob(f"{qt_version_path}/msvc*_64", rev=True)

            log_info(f"Using Qt Version {os.path.basename(qt_version_path)}")
            log_info(f"Using MSVC Version {os.path.basename(qt_msvc_path)}")

            self.plugins_path = f"{qt_msvc_path}/plugins"
            required_folder(self.plugins_path)
            self.deployqt = f"{qt_msvc_path}/bin/windeployqt.exe"
            required_file(self.deployqt)
            self.cmake_defs = [f"-DCMAKE_PREFIX_PATH={qt_msvc_path}/lib/cmake"]

            env_path_add([f"{self.tools_path}/CMake_64/bin", f"{self.tools_path}/Ninja"])

        if p == "darwin":
            self.deployqt = "macdeployqt"
            required_command(self.deployqt)


# --- EntryPoint ---


################################################################################
def entrypoint(main: Callable):  # TODO add kwargs to pass to the main function
    import inspect

    callee_name = inspect.stack()[1].frame.f_globals.get("__name__", "")

    if callee_name != "__main__":
        return

    try:
        main()
    except KeyboardInterrupt:
        log_error("User interrupts execution.")
    except PermissionError as e:
        log_error("Some files are in use, execution blocked", e)
    except Exception as e:
        log_error("Unexpected", e)


# --- Setup ---


################################################################################
def __initialize():
    global __GLOBAL_TEE, __GLOBAL_TEE_WITH_CALLBACK
    if not __GLOBAL_TEE:
        __GLOBAL_TEE = tee_new(None, 1, None)
    if not __GLOBAL_TEE_WITH_CALLBACK:
        __GLOBAL_TEE_WITH_CALLBACK = tee_new(None, 1, TRACE_CB)


################################################################################
def setup(
    exit_cb: Callable | None = None,
    trace_cb: Callable | None = None,
    process_cb: Callable | None = None,
    process_th: int = 10,
    process_filepath: str | None = None,
):

    global EXIT_CB, TRACE_CB, PROCESS_CB, PROCESS_TH, PROCESS_FILEPATH

    EXIT_CB = exit_cb
    TRACE_CB = trace_cb
    PROCESS_CB = process_cb
    PROCESS_TH = process_th
    PROCESS_FILEPATH = process_filepath

    __initialize()
