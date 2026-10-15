"""Exercise the production CLI without a database, client or listening service."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def check(executable):
  build_directories = [ROOT / 'Artifacts/Bin/x64' / config for config in ('Debug', 'Release')]
  working = ROOT if executable.parent in build_directories else executable.parent
  logs = ROOT / 'Artifacts/smoke' / executable.parent.name
  logs.mkdir(parents=True, exist_ok=True)
  environment = os.environ.copy()
  environment.pop('BF_DATABASE_PASSWORD', None)
  environment.pop('BF_ACCOUNT_PASSWORD', None)
  if executable.parent not in build_directories:
    system = Path(os.environ['SystemRoot'])
    environment['PATH'] = os.pathsep.join([str(system / 'System32'), str(system)])
  def run(name, arguments, status, expected):
    result = subprocess.run([str(executable), *arguments], cwd=working, env=environment,
                            capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=30)
    output = result.stdout + result.stderr
    (logs / (name + '.log')).write_text(output, encoding='utf-8')
    if result.returncode != status or expected not in output:
      raise RuntimeError(f'{name}: exit {result.returncode}, expected {status}; see {logs}')
  run('help', ['--help'], 0, '--check-config')
  run('config', ['--check-config'], 0, 'Configuration is valid.')
  run('credentials', [], 1, 'BF_DATABASE_PASSWORD is required')
  run('invalid_option', ['--does-not-exist'], 1, 'Invalid command line')
  with tempfile.TemporaryDirectory(prefix='cn0261-smoke-') as directory:
    invalid = Path(directory) / 'invalid.json'
    invalid.write_text('{"network":42}', encoding='utf-8')
    run('invalid_config', ['--config', str(invalid), '--check-config'], 1, 'network must be an object')
  print(str(executable) + ': production CLI and configuration smoke checks passed.')


if __name__ == '__main__':
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument('--configuration', choices=['Debug', 'Release'])
  parser.add_argument('--executable', type=Path)
  args = parser.parse_args()
  if args.executable:
    check(args.executable.resolve(strict=True))
  else:
    for configuration in [args.configuration] if args.configuration else ['Debug', 'Release']:
      check(ROOT / 'Artifacts/Bin/x64' / configuration / 'BubbleFighter_CN0261_Server.exe')
