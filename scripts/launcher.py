"""CN0261 account launcher: verified TLS credentials exchange and owned client startup."""

import argparse
import json
import os
import queue
import re
import socket
import ssl
import struct
import subprocess
import threading
import uuid
from pathlib import Path
import tkinter as tk
from tkinter import ttk

from start_client import launch


def receive_exact(connection, length):
  result = bytearray()
  while len(result) < length:
    data = connection.recv(length - len(result))
    if not data:
      raise ConnectionError("认证连接在完整响应前关闭")
    result.extend(data)
  return bytes(result)


def authenticate(host, port, certificate, account_name, password):
  context = ssl.create_default_context(cafile=str(certificate))
  context.minimum_version = ssl.TLSVersion.TLSv1_2
  body = json.dumps({"version": 1, "action": "login", "accountName": account_name,
           "password": password}, ensure_ascii=False).encode("utf-8")
  if len(body) > 4096:
    raise ValueError("认证请求过长")
  with socket.create_connection((host, port), timeout=10) as transport:
    with context.wrap_socket(transport, server_hostname=host) as connection:
      connection.sendall(struct.pack("<I", len(body)) + body)
      length = struct.unpack("<I", receive_exact(connection, 4))[0]
      if not 0 < length <= 4096:
        raise ValueError("认证响应长度不正确")
      response = json.loads(receive_exact(connection, length).decode("utf-8"))
  if response.get("success") is not True:
    errors = {"credentialsRejected": "账号或密码不正确，或账号已停用", "serviceBusy": "认证服务繁忙，请稍后重试",
         "invalidRequest": "账号须为 3 至 32 位小写字母、数字或下划线；密码须为 8 至 128 个 UTF-8 字节"}
    raise ValueError(errors.get(response.get("error"), "认证服务暂时不可用"))
  if response.get("protocolVersion") != 717 or not re.fullmatch(r"[0-9a-f]{64}", response.get("loginTicket", "")):
    raise ValueError("服务器返回的客户端版本或凭据格式不正确")
  if not isinstance(response.get("gamePort"), int) or not 1 <= response["gamePort"] <= 65535:
    raise ValueError("服务器端口不正确")
  if not isinstance(response.get("gameHost"), str) or not re.fullmatch(r"[A-Za-z0-9.:-]{1,253}", response["gameHost"]):
    raise ValueError("服务器地址不正确")
  return response


def start_game(root, config, response):
  client_directory = (root / config["clientDirectory"]).resolve(strict=True)
  runtime = root / "Artifacts" / "ClientRuns" / uuid.uuid4().hex / "Client"
  subprocess.run(["powershell.exe", "-NoProfile", "-NonInteractive", "-File",
          str(root / "scripts" / "create_client_runtime.ps1"), "-ClientDirectory", str(client_directory),
          "-RuntimeDirectory", str(runtime)], check=True, capture_output=True,
         creationflags=subprocess.CREATE_NO_WINDOW, timeout=90)
  login_path = runtime / "login.ini"
  try:
    login_path.write_text("serverAddr={}\nserverPort={}\nid={}\n".format(
      response["gameHost"], response["gamePort"], response["loginTicket"]), encoding="utf-8")
    return launch(runtime / "BubbleFighter.exe", config["instance"], launcher_identity=True)
  finally:
    if login_path.exists():
      login_path.unlink()


def run(config_path):
  root = Path(__file__).resolve().parents[1]
  config = json.loads(config_path.read_text(encoding="utf-8"))
  if not isinstance(config.get("instance"), str) or not re.fullmatch(r"[A-Z0-9]", config["instance"]):
    raise ValueError("instance 必须是单个大写字母或数字")
  certificate = (root / config["certificateFile"]).resolve(strict=True)
  window = tk.Tk()
  window.title("BubbleFighter CN0261 启动器")
  window.geometry("500x300")
  window.resizable(False, False)
  frame = ttk.Frame(window, padding=24)
  frame.pack(fill="both", expand=True)
  ttk.Label(frame, text="泡泡战士 · CN0261", font=("Microsoft YaHei UI", 18)).grid(row=0, column=0, columnspan=2, sticky="w")
  ttk.Label(frame, text="认证服务器：{}:{}（TLS）".format(config["authenticationHost"], config["authenticationPort"])).grid(
    row=1, column=0, columnspan=2, sticky="w", pady=(8, 18))
  account = tk.StringVar()
  password = tk.StringVar()
  status = tk.StringVar(value="请输入自建服务端账号。首次账号由服务器管理员创建。")
  ttk.Label(frame, text="账号").grid(row=2, column=0, sticky="w", padx=(0, 12), pady=6)
  account_entry = ttk.Entry(frame, textvariable=account, width=38)
  account_entry.grid(row=2, column=1, sticky="ew", pady=6)
  ttk.Label(frame, text="密码").grid(row=3, column=0, sticky="w", pady=6)
  ttk.Entry(frame, textvariable=password, show="●", width=38).grid(row=3, column=1, sticky="ew", pady=6)
  events = queue.Queue()
  closing = threading.Event()

  def worker(name, secret):
    try:
      response = authenticate(config["authenticationHost"], config["authenticationPort"], certificate, name, secret)
      if closing.is_set():
        return
      events.put((False, "认证成功，正在启动 CN0261 客户端……"))
      start_game(root, config, response)
      events.put((True, "客户端已启动。游戏登录将消费本次一次性凭据。"))
    except ssl.SSLCertVerificationError:
      events.put((True, "服务器证书验证失败，请核对地址和启动器证书配置。"))
    except (ValueError, ConnectionError) as error:
      events.put((True, str(error)))
    except Exception:
      events.put((True, "连接或启动失败，请检查服务器、客户端路径及 CN0261 版本。"))

  def submit():
    name, secret = account.get(), password.get()
    password.set("")
    button.configure(state="disabled")
    status.set("正在通过 TLS 验证账号……")
    threading.Thread(target=worker, args=(name, secret), daemon=True).start()

  button = ttk.Button(frame, text="登录并启动游戏", command=submit)
  button.grid(row=4, column=0, columnspan=2, sticky="ew", pady=(15, 12))
  ttk.Label(frame, textvariable=status, wraplength=448).grid(row=5, column=0, columnspan=2, sticky="w")

  def poll():
    while not events.empty():
      complete, message = events.get_nowait()
      status.set(message)
      if complete:
        button.configure(state="normal")
    if not closing.is_set():
      window.after(100, poll)

  def close():
    closing.set()
    window.destroy()

  window.protocol("WM_DELETE_WINDOW", close)
  account_entry.focus_set()
  poll()
  window.mainloop()


if __name__ == "__main__":
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument("--config", type=Path, default=Path(__file__).resolve().parents[1] / "config" / "launcher.json")
  parser.add_argument("--authenticate-only", action="store_true", help="Return a ticket as JSON for integration tools")
  parser.add_argument("--account")
  arguments = parser.parse_args()
  if arguments.authenticate_only:
    settings = json.loads(arguments.config.read_text(encoding="utf-8"))
    project = Path(__file__).resolve().parents[1]
    print(json.dumps(authenticate(settings["authenticationHost"], settings["authenticationPort"],
                   project / settings["certificateFile"], arguments.account,
                   os.environ.get("BF_ACCOUNT_PASSWORD", ""))))
  else:
    run(arguments.config)
