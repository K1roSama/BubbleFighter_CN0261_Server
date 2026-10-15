"""Validate structure, compatibility and Google C++ conventions (LLVM 22.1.8)."""
import argparse
import concurrent.futures
import functools
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import xml.etree.ElementTree as xml

ROOT = Path(__file__).resolve().parents[1]
LLVM_VERSION = '22.1.8'
TOKEN = re.compile(
    r'//[^\n]*|/\*.*?\*/|(?:u8|u|U|L)?"(?:\\.|[^"\\])*"|'
    r"(?:u8|u|U|L)?'(?:\\.|[^'\\])*'|"
    r"(?:0[xX][0-9A-Fa-f']+|[0-9]+(?:'[0-9]+)*(?:\.[0-9']*)?(?:[eEpP][+-]?[0-9']+)?)[uUlLfF]*|"
    r'\b[A-Za-z_]\w*\b|\s+|.', re.S)


def canonical_digest(text):
  """Ignore layout, include paths, guards and identifier case, retaining literals."""
  text = re.sub(r'^\s*#(?:include[^\n]*|pragma once|ifndef BUBBLEFIGHTER_SERVER_\w+|'
                r'define BUBBLEFIGHTER_SERVER_\w+|endif\s*//\s*BUBBLEFIGHTER_SERVER_\w+)\s*$',
                '', text, flags=re.M)
  def convert(match):
    value = match[0]
    if value.startswith(('//', '/*')) or value.isspace():
      return ''
    if re.fullmatch(r'[A-Za-z_]\w*', value):
      if re.match(r'k[A-Z]', value):
        value = value[1:]
      value = re.sub(r'([A-Z]+)([A-Z][a-z])', r'\1_\2', value)
      value = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', value).lower()
    return value + '\n'
  return hashlib.sha256(TOKEN.sub(convert, text).encode()).hexdigest()


def sources():
  return sorted(path for folder in ('src', 'tests') for path in (ROOT / folder).rglob('*')
                if path.suffix in ('.cpp', '.h'))


def structure():
  ns = {'m': 'http://schemas.microsoft.com/developer/msbuild/2003'}
  production = {path.resolve() for path in (ROOT / 'src').rglob('*') if path.suffix in ('.cpp', '.h')}
  include_paths = {path.relative_to(ROOT / 'src').as_posix() for path in (ROOT / 'src').rglob('*') if path.is_file()}
  items = []
  for project in sorted((ROOT / 'build').glob('*.vcxproj')):
    document = xml.parse(project)
    own_items = [item for kind in ('ClCompile', 'ClInclude')
                 for item in document.findall('.//m:' + kind, ns) if 'Include' in item.attrib]
    if project.name != 'regression.vcxproj':
      items.extend((project.parent / item.attrib['Include'].replace('\\', '/')).resolve() for item in own_items)
    for item in own_items + document.findall('.//m:ProjectReference', ns):
      target = (project.parent / item.attrib['Include'].replace('\\', '/')).resolve()
      if not target.is_file():
        raise RuntimeError(f'Missing project reference: {project.name} -> {target}')
    filters = xml.parse(Path(str(project) + '.filters'))
    filtered = [item.attrib['Include'] for kind in ('ClCompile', 'ClInclude')
                for item in filters.findall('.//m:' + kind, ns)]
    if sorted(filtered) != sorted(item.attrib['Include'] for item in own_items):
      raise RuntimeError('Project filters differ: ' + project.name)
  if set(items) != production or len(items) != len(production):
    raise RuntimeError('Production project items differ from src; run build/update_project_files.py')
  for path in sources():
    if not re.fullmatch(r'[a-z][a-z0-9_]*\.(cpp|h)', path.name):
      raise RuntimeError('Invalid source filename: ' + str(path))
    text = path.read_text(encoding='utf-8')
    for include in re.findall(r'^#include "([^"]+)"', text, re.M):
      target = ROOT / 'src' / include
      if not target.is_file() or include not in include_paths:
        raise RuntimeError(f'Invalid internal include: {path} -> {include}')
    if path.suffix == '.h':
      guard = 'BUBBLEFIGHTER_SERVER_' + re.sub('[^A-Z0-9]', '_', path.relative_to(ROOT / 'src').as_posix().upper()) + '_'
      if not all(part in text for part in ('#ifndef ' + guard, '#define ' + guard, '#endif  // ' + guard)):
        raise RuntimeError('Invalid header guard: ' + str(path))
  for document in [ROOT / 'README.md', *(ROOT / 'docs').glob('*.md')]:
    text = document.read_text(encoding='utf-8')
    links = re.findall(r'\]\(([^)]+)\)|\b(?:src|href)="([^"]+)"', text)
    for markdown, html in links:
      link = markdown or html
      if '://' not in link and not link.startswith(('#', 'mailto:')):
        if not (document.parent / link.split('#')[0]).exists():
          raise RuntimeError(f'Broken documentation link: {document.name} -> {link}')
  print('Project items, filters, include paths, guards and documentation links passed.')


def text_conventions():
  extensions = {'.cpp', '.h', '.py', '.ps1', '.cmake', '.json', '.sql', '.md', '.yml', '.yaml',
                '.vcxproj', '.props', '.filters', '.sln', '.txt'}
  paths = [path for path in ROOT.iterdir() if path.is_file() and path.suffix in extensions]
  for folder in ('src', 'build', 'scripts', 'tests', 'config', 'database', 'docs', '.github'):
    paths.extend(path for path in (ROOT / folder).rglob('*') if path.is_file() and path.suffix in extensions)
  for path in paths:
    raw = path.read_bytes()
    if raw.startswith(b'\xef\xbb\xbf'):
      raise RuntimeError('UTF-8 BOM is not allowed: ' + str(path))
    text = raw.decode('utf-8')
    if raw and not raw.endswith(b'\n'):
      raise RuntimeError('Missing final newline: ' + str(path))
    if path.suffix not in ('.sln', '.vcxproj', '.props', '.filters', '.ps1') and b'\r\n' in raw:
      raise RuntimeError('Expected LF line endings: ' + str(path))
    for number, line in enumerate(text.splitlines(), 1):
      if path.suffix != '.md' and line != line.rstrip():
        raise RuntimeError(f'Trailing whitespace: {path}:{number}')
      if path.suffix != '.sln' and '\t' in line:
        raise RuntimeError(f'Tab indentation: {path}:{number}')
  print('UTF-8, line endings and whitespace checks passed.')


def baselines():
  data = json.loads((ROOT / 'tests/refactor_baseline.json').read_text(encoding='utf-8'))
  for entry in data['source_files']:
    text = (ROOT / entry['path']).read_text(encoding='utf-8')
    for new, old in data['path_aliases'].items():
      text = text.replace('"' + new + '"', '"' + old + '"')
    if canonical_digest(text) != entry['sha256']:
      raise RuntimeError('Source compatibility baseline changed: ' + entry['path'])
  for entry in data['migrations']:
    if hashlib.sha256((ROOT / entry['path']).read_bytes()).hexdigest() != entry['sha256']:
      raise RuntimeError('Historical SQL migration changed: ' + entry['path'])
  for entry in data['config_files']:
    text = (ROOT / entry['path']).read_text(encoding='utf-8')
    for new, old in data['path_aliases'].items():
      text = text.replace('"' + new + '"', '"' + old + '"')
    canonical = json.dumps(json.loads(text), sort_keys=True, separators=(',', ':'), ensure_ascii=False)
    if hashlib.sha256(canonical.encode()).hexdigest() != entry['sha256']:
      raise RuntimeError('Configuration compatibility baseline changed: ' + entry['path'])
  print(f"Frozen compatibility: {len(data['source_files'])} source files and {len(data['migrations'])} migrations passed.")


def tool(name):
  executable = os.environ.get('BF_' + name.upper().replace('-', '_')) or shutil.which(name)
  if not executable:
    candidate = Path(os.environ.get('ProgramFiles', 'C:/Program Files')) / 'LLVM/bin' / (name + '.exe')
    if candidate.is_file():
      executable = str(candidate)
  if not executable:
    raise RuntimeError('Install LLVM ' + LLVM_VERSION + ': ' + name + ' was not found')
  version = subprocess.check_output([executable, '--version'], text=True, encoding='utf-8', errors='replace')
  if not re.search(r'\b' + re.escape(LLVM_VERSION) + r'\b', version):
    raise RuntimeError('Expected ' + name + ' ' + LLVM_VERSION)
  return executable


@functools.cache
def compiler_flags():
  flags = ['-std=c++20', '-fno-delayed-template-parsing', '-Wno-pragma-once-outside-header',
           '-DWIN32_LEAN_AND_MEAN', '-DNOMINMAX', '-D_WIN32_WINNT=0x0A00',
           '-DBOOST_ALL_DYN_LINK', '-DSPDLOG_FMT_EXTERNAL', '-DSPDLOG_WCHAR_FILENAMES', '-DFMT_SHARED',
           '-I' + str(ROOT / 'src'), '-isystem', str(ROOT / 'Artifacts/Dependencies/x64-windows/include')]
  if sys.platform == 'win32':
    import winreg
    vswhere = Path(os.environ['ProgramFiles(x86)']) / 'Microsoft Visual Studio/Installer/vswhere.exe'
    installation = subprocess.check_output(
        [str(vswhere), '-latest', '-products', '*', '-requires',
         'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', '-property', 'installationPath'], text=True).strip()
    toolsets = sorted((Path(installation) / 'VC/Tools/MSVC').glob('14.[34]*'))
    if not toolsets:
      raise RuntimeError('Install MSVC v143 for header and naming checks.')
    flags += ['-fms-compatibility-version=19.44', '-isystem', str(toolsets[-1] / 'include')]
    with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r'SOFTWARE\Microsoft\Windows Kits\Installed Roots',
                        0, winreg.KEY_READ | winreg.KEY_WOW64_32KEY) as key:
      kits = Path(winreg.QueryValueEx(key, 'KitsRoot10')[0]) / 'Include/10.0.26100.0'
    for folder in ('ucrt', 'shared', 'um', 'winrt'):
      if not (kits / folder).is_dir():
        raise RuntimeError('Install Windows SDK 10.0.26100.0 for header and naming checks.')
      flags += ['-isystem', str(kits / folder)]
  return flags


def parallel_check(paths, command, label):
  directory = ROOT / 'Artifacts/checks' / label
  directory.mkdir(parents=True, exist_ok=True)
  def run(path):
    result = subprocess.run(command(path), cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    (directory / (path.stem + '.log')).write_bytes(result.stdout)
    return (path, result.stdout.decode('utf-8', 'replace')[:1200]) if result.returncode else None
  with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
    failures = [result for result in pool.map(run, paths) if result]
  for path, output in failures[:6]:
    print(str(path.relative_to(ROOT)) + '\n' + output, file=sys.stderr)
  if failures:
    raise RuntimeError(f'{len(failures)} {label} check(s) failed; full logs: {directory}')
  print(f'{label}: {len(paths)} files passed.')


def main():
  parser = argparse.ArgumentParser(description=__doc__)
  for option in ('format', 'fix-format', 'headers', 'naming'):
    parser.add_argument('--' + option, action='store_true')
  args = parser.parse_args()
  structure()
  text_conventions()
  baselines()
  paths = sources()
  if args.format or args.fix_format:
    executable = tool('clang-format')
    flags = ['-i'] if args.fix_format else ['--dry-run', '--Werror']
    parallel_check(paths, lambda path: [executable, *flags, str(path)], 'format')
  if args.headers:
    executable = tool('clang++')
    parallel_check([p for p in paths if p.suffix == '.h'],
                   lambda path: [executable, *compiler_flags(), '-x', 'c++', '-fsyntax-only', str(path)], 'headers')
  if args.naming:
    executable = tool('clang-tidy')
    parallel_check([p for p in paths if p.suffix == '.cpp'],
                   lambda path: [executable, str(path), '--warnings-as-errors=readability-identifier-naming',
                                 '--', *compiler_flags()], 'naming')


if __name__ == '__main__':
  try:
    main()
  except Exception as error:
    print(error, file=sys.stderr)
    sys.exit(1)
