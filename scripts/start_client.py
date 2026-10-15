"""Start the verified CN0261 image with a distinct process-only instance mutex."""

import argparse
import ctypes
from ctypes import wintypes
import hashlib
import json
import os
import re
from pathlib import Path
import struct
import subprocess
import time


EXPECTED_HASH = "4c16c49e4dcf90b3fb8422918dce7698acbe7f1c1e318eccddd71c444284ff4b"
MUTEX_ADDRESS = 0xEA728C
ORIGINAL_MUTEX = b"Lambda\0"


class StartupInfo(ctypes.Structure):
  _fields_ = [
    ("cb", wintypes.DWORD), ("reserved", wintypes.LPWSTR), ("desktop", wintypes.LPWSTR),
    ("title", wintypes.LPWSTR), ("x", wintypes.DWORD), ("y", wintypes.DWORD),
    ("xSize", wintypes.DWORD), ("ySize", wintypes.DWORD), ("xCountChars", wintypes.DWORD),
    ("yCountChars", wintypes.DWORD), ("fillAttribute", wintypes.DWORD), ("flags", wintypes.DWORD),
    ("showWindow", wintypes.WORD), ("reservedSize", wintypes.WORD), ("reservedData", ctypes.c_void_p),
    ("stdInput", wintypes.HANDLE), ("stdOutput", wintypes.HANDLE), ("stdError", wintypes.HANDLE),
  ]


class ProcessInformation(ctypes.Structure):
  _fields_ = [
    ("process", wintypes.HANDLE), ("thread", wintypes.HANDLE),
    ("processId", wintypes.DWORD), ("threadId", wintypes.DWORD),
  ]


def transfer_identity(kernel, process, ticket):
  """Move the client's own parsed string buffer into its empty connection identity.

    CN0261 VA 0x9878AD assigns login.ini id to UserInfo+36, while VA 0x6DAA80
    reads GameConnection+2440. Moving the two 24-byte string representations
    preserves ownership of the allocation made by the client's own allocator.
    Only the process created by this invocation is accessed; no code is changed.
    """
  def read(address, length):
    buffer = ctypes.create_string_buffer(length)
    copied = ctypes.c_size_t()
    if not kernel.ReadProcessMemory(process.process, address, buffer, length, ctypes.byref(copied)):
      return None
    return buffer.raw if copied.value == length else None

  def pointer(address):
    value = read(address, 4)
    return struct.unpack("<I", value)[0] if value else 0

  def write(address, value):
    copied = ctypes.c_size_t()
    if not kernel.WriteProcessMemory(process.process, address, value, len(value), ctypes.byref(copied)):
      raise ctypes.WinError(ctypes.get_last_error())
    if copied.value != len(value):
      raise RuntimeError("Incomplete launcher identity transfer")

  def select_launcher_login():
    # VA 0xB1EDE0: std::map<int, bool>, head at config+4, key at node+12, value at node+16.
    # S960 only inserts absent keys (0xB1ED90). CN0261 already enables 1004 during startup.
    # Set this existing configuration value before LoginStage can open the operator SDK window.
    config = pointer(0xFF3620)
    head = pointer(config + 4) if config else 0
    node = pointer(head + 4) if head else 0
    visited = set()
    while node and node != head and len(visited) < 64:
      if node in visited:
        raise RuntimeError("Client content configuration tree contains a cycle")
      visited.add(node)
      data = read(node, 24)
      if not data or data[21] != 0:
        break
      key = struct.unpack_from("<i", data, 12)[0]
      if key == 1004:
        if data[16] not in (0, 1):
          raise RuntimeError("Operator login configuration value is invalid")
        write(node + 16, b"\0")
        if read(node + 16, 1) != b"\0":
          raise RuntimeError("Launcher login configuration verification failed")
        return
      node = struct.unpack_from("<I", data, 0 if 1004 < key else 8)[0]
    raise RuntimeError("Verified CN0261 operator login configuration was not found")

  encoded = ticket.encode("utf-16-le") + b"\0\0"
  deadline = time.monotonic() + 60
  while time.monotonic() < deadline:
    user = pointer(0xFFBAEC)
    connection = pointer(0x100DED4)
    source = read(user + 36, 24) if user else None
    destination = read(connection + 2440, 24) if connection else None
    if source and destination:
      length, capacity = struct.unpack_from("<II", source, 16)
      if length == len(ticket) and capacity >= length and capacity < 4096:
        if kernel.SuspendThread(process.thread) == 0xFFFFFFFF:
          raise ctypes.WinError(ctypes.get_last_error())
        try:
          if source != read(user + 36, 24) or destination != read(connection + 2440, 24):
            continue
          if struct.unpack_from("<II", destination, 16) != (0, 7):
            raise RuntimeError("Connection identity was not empty before launcher handoff")
          source_buffer = struct.unpack_from("<I", source)[0]
          if read(source_buffer, len(encoded)) != encoded:
            raise RuntimeError("Parsed client identity did not match the issued ticket")
          select_launcher_login()
          write(connection + 2440, source)
          write(user + 36, destination)
          if read(connection + 2440, 24) != source or read(user + 36, 24) != destination:
            raise RuntimeError("Launcher identity handoff verification failed")
          return
        finally:
          if kernel.ResumeThread(process.thread) == 0xFFFFFFFF:
            raise ctypes.WinError(ctypes.get_last_error())
    exit_code = wintypes.DWORD()
    if not kernel.GetExitCodeProcess(process.process, ctypes.byref(exit_code)) or exit_code.value != 259:
      raise RuntimeError("Client exited before launcher identity handoff")
    time.sleep(0.001)
  raise RuntimeError("Client identity handoff was not ready within 60 seconds")


def launch(image_path, instance, launcher_identity=False):
  image_path = image_path.resolve(strict=True)
  image_bytes = image_path.read_bytes()
  if hashlib.sha256(image_bytes).hexdigest() != EXPECTED_HASH:
    raise ValueError("Client SHA-256 does not match the verified original CN0261 image")
  pe_offset = struct.unpack_from("<I", image_bytes, 0x3C)[0]
  if struct.unpack_from("<H", image_bytes, pe_offset + 4)[0] != 0x14C:
    raise ValueError("Expected the verified x86 image")
  if struct.unpack_from("<I", image_bytes, pe_offset + 52)[0] != 0x400000:
    raise ValueError("Expected the verified image base")
  replacement = ("B261_" + instance).encode("ascii") + b"\0"
  if len(replacement) != len(ORIGINAL_MUTEX):
    raise ValueError("Instance must be one uppercase letter or digit")

  ticket = None
  if launcher_identity:
    values = dict(line.split("=", 1) for line in (image_path.parent / "login.ini").read_text(
      encoding="utf-8").splitlines() if "=" in line)
    ticket = values.get("id", "")
    if not re.fullmatch(r"[0-9a-f]{64}", ticket):
      raise ValueError("Launcher identity must be an issued one-time login ticket")

  kernel = ctypes.WinDLL("kernel32", use_last_error=True)
  kernel.CreateProcessW.argtypes = [
    wintypes.LPCWSTR, wintypes.LPWSTR, ctypes.c_void_p, ctypes.c_void_p, wintypes.BOOL,
    wintypes.DWORD, ctypes.c_void_p, wintypes.LPCWSTR, ctypes.POINTER(StartupInfo),
    ctypes.POINTER(ProcessInformation),
  ]
  kernel.CreateProcessW.restype = wintypes.BOOL
  for name in ("ResumeThread", "SuspendThread"):
    getattr(kernel, name).argtypes = [wintypes.HANDLE]
    getattr(kernel, name).restype = wintypes.DWORD
  for name in ("ReadProcessMemory", "WriteProcessMemory"):
    getattr(kernel, name).argtypes = [
      wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t,
      ctypes.POINTER(ctypes.c_size_t),
    ]
    getattr(kernel, name).restype = wintypes.BOOL
  kernel.VirtualProtectEx.argtypes = [
    wintypes.HANDLE, ctypes.c_void_p, ctypes.c_size_t, wintypes.DWORD, ctypes.POINTER(wintypes.DWORD),
  ]
  kernel.VirtualProtectEx.restype = wintypes.BOOL
  kernel.GetExitCodeProcess.argtypes = [wintypes.HANDLE, ctypes.POINTER(wintypes.DWORD)]
  kernel.GetExitCodeProcess.restype = wintypes.BOOL
  kernel.TerminateProcess.argtypes = [wintypes.HANDLE, wintypes.UINT]
  kernel.TerminateProcess.restype = wintypes.BOOL
  kernel.CloseHandle.argtypes = [wintypes.HANDLE]
  kernel.CloseHandle.restype = wintypes.BOOL

  startup = StartupInfo()
  startup.cb = ctypes.sizeof(startup)
  startup.flags = 1
  # The game window is interactive; launcher helpers remain hidden.
  startup.showWindow = 1
  process = ProcessInformation()
  command = ctypes.create_unicode_buffer(subprocess.list2cmdline([str(image_path)]))
  environment = {key: value for key, value in os.environ.items()
         if not key.startswith("BF_") and key != "MYSQL_PWD"}
  environment["__COMPAT_LAYER"] = "RunAsInvoker"
  environment_block = ctypes.create_unicode_buffer(
    "\0".join(key + "=" + value for key, value in sorted(environment.items())) + "\0\0")
  saved_compatibility = os.environ.get("__COMPAT_LAYER")
  os.environ["__COMPAT_LAYER"] = "RunAsInvoker"
  try:
    created = kernel.CreateProcessW(
      str(image_path), command, None, None, False, 0x4 | 0x400, environment_block,
      str(image_path.parent), ctypes.byref(startup), ctypes.byref(process))
    creation_error = ctypes.get_last_error()
  finally:
    if saved_compatibility is None:
      os.environ.pop("__COMPAT_LAYER", None)
    else:
      os.environ["__COMPAT_LAYER"] = saved_compatibility
  if not created:
    raise ctypes.WinError(creation_error)

  completed = False
  suspended = True
  observed = ctypes.create_string_buffer(len(ORIGINAL_MUTEX))
  copied = ctypes.c_size_t()
  try:
    if kernel.ResumeThread(process.thread) == 0xFFFFFFFF:
      raise ctypes.WinError(ctypes.get_last_error())
    suspended = False
    deadline = time.monotonic() + 20
    while time.monotonic() < deadline:
      exit_code = wintypes.DWORD()
      if not kernel.GetExitCodeProcess(process.process, ctypes.byref(exit_code)):
        raise ctypes.WinError(ctypes.get_last_error())
      if exit_code.value != 259:
        raise RuntimeError("Client exited before mutex initialization: 0x{:08X}".format(exit_code.value))
      valid = kernel.ReadProcessMemory(
        process.process, MUTEX_ADDRESS, observed, len(ORIGINAL_MUTEX), ctypes.byref(copied))
      if valid and copied.value == len(ORIGINAL_MUTEX) and observed.raw == ORIGINAL_MUTEX:
        break
      time.sleep(0)
    else:
      raise RuntimeError("Verified client mutex bytes did not appear within 20 seconds")

    if kernel.SuspendThread(process.thread) == 0xFFFFFFFF:
      raise ctypes.WinError(ctypes.get_last_error())
    suspended = True
    if not kernel.ReadProcessMemory(
        process.process, MUTEX_ADDRESS, observed, len(ORIGINAL_MUTEX), ctypes.byref(copied)):
      raise ctypes.WinError(ctypes.get_last_error())
    if copied.value != len(ORIGINAL_MUTEX) or observed.raw != ORIGINAL_MUTEX:
      raise RuntimeError("Mutex bytes changed before the suspended-thread verification")
    previous_protection = wintypes.DWORD()
    if not kernel.VirtualProtectEx(
        process.process, MUTEX_ADDRESS, len(ORIGINAL_MUTEX), 0x04, ctypes.byref(previous_protection)):
      raise ctypes.WinError(ctypes.get_last_error())
    try:
      if not kernel.WriteProcessMemory(
          process.process, MUTEX_ADDRESS, replacement, len(replacement), ctypes.byref(copied)):
        raise ctypes.WinError(ctypes.get_last_error())
      if copied.value != len(replacement):
        raise RuntimeError("Incomplete process mutex write")
    finally:
      ignored = wintypes.DWORD()
      if not kernel.VirtualProtectEx(
          process.process, MUTEX_ADDRESS, len(ORIGINAL_MUTEX), previous_protection, ctypes.byref(ignored)):
        raise ctypes.WinError(ctypes.get_last_error())
    if not kernel.ReadProcessMemory(
        process.process, MUTEX_ADDRESS, observed, len(replacement), ctypes.byref(copied)):
      raise ctypes.WinError(ctypes.get_last_error())
    if copied.value != len(replacement) or observed.raw != replacement:
      raise RuntimeError("Process mutex verification failed")
    if kernel.ResumeThread(process.thread) == 0xFFFFFFFF:
      raise ctypes.WinError(ctypes.get_last_error())
    suspended = False
    if launcher_identity:
      transfer_identity(kernel, process, ticket)
    completed = True
    return {"processId": process.processId, "sourceHash": EXPECTED_HASH,
        "runtimeAddress": "0xEA728C", "originalMutex": "Lambda",
        "instanceMutex": replacement[:-1].decode("ascii"), "fileChanged": False,
        "launcherIdentityTransferred": launcher_identity,
        "operatorLoginDisabled": launcher_identity}
  finally:
    if not completed:
      kernel.TerminateProcess(process.process, 1)
    elif suspended:
      kernel.ResumeThread(process.thread)
    kernel.CloseHandle(process.thread)
    kernel.CloseHandle(process.process)


if __name__ == "__main__":
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument("--image", required=True, type=Path)
  parser.add_argument("--instance", choices=list("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"), default="A")
  parser.add_argument("--launcher-identity", action="store_true")
  arguments = parser.parse_args()
  print(json.dumps(launch(arguments.image, arguments.instance, arguments.launcher_identity)))
