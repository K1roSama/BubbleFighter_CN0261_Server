"""Refresh explicit source lists and Visual Studio filters; no compilation side effects."""
from pathlib import Path
import re
import uuid

ROOT = Path(__file__).resolve().parents[1]


def update(name, paths):
  project = ROOT / 'build' / name
  text = project.read_text(encoding='utf-8')
  text = re.sub(r'  <!-- BEGIN GENERATED SOURCES -->.*?  <!-- END GENERATED SOURCES -->\n?', '', text, flags=re.S)
  lines = ['  <!-- BEGIN GENERATED SOURCES -->', '  <ItemGroup>']
  filters = ['<?xml version="1.0" encoding="utf-8"?>',
             '<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">',
             '  <ItemGroup>']
  groups = sorted({parent.relative_to(ROOT).as_posix() for path in paths for parent in path.parents
                   if parent != ROOT and parent.is_relative_to(ROOT)})
  for group in groups:
    filters.extend(['    <Filter Include="' + group.replace('/', '\\') + '">',
                    '      <UniqueIdentifier>{' + str(uuid.uuid5(uuid.NAMESPACE_URL, group)).upper() + '}</UniqueIdentifier>',
                    '    </Filter>'])
  filters.extend(['  </ItemGroup>', '  <ItemGroup>'])
  for path in paths:
    relative = '..\\' + str(path.relative_to(ROOT)).replace('/', '\\')
    kind = 'ClCompile' if path.suffix == '.cpp' else 'ClInclude'
    lines.append('    <' + kind + ' Include="' + relative + '" />')
    filters.extend(['    <' + kind + ' Include="' + relative + '">',
                    '      <Filter>' + str(path.parent.relative_to(ROOT)).replace('/', '\\') + '</Filter>',
                    '    </' + kind + '>'])
  lines.extend(['  </ItemGroup>', '  <!-- END GENERATED SOURCES -->'])
  prefix, suffix = text.rsplit('</Project>', 1)
  text = prefix + '\n'.join(lines) + '\n</Project>' + suffix
  project.write_text(text, encoding='utf-8', newline='\r\n')
  filters.extend(['  </ItemGroup>', '</Project>', ''])
  Path(str(project) + '.filters').write_text('\n'.join(filters), encoding='utf-8', newline='\r\n')


if __name__ == '__main__':
  paths = sorted(path for path in (ROOT / 'src').rglob('*') if path.suffix in ('.cpp', '.h'))
  main = ROOT / 'src/application/main.cpp'
  update('BubbleFighter_CN0261_Server.vcxproj', [main])
  update('server_core.vcxproj', [path for path in paths if path != main])
  if (ROOT / 'build/regression.vcxproj').is_file():
    update('regression.vcxproj', sorted((ROOT / 'tests').glob('*.cpp')))
  print('Updated production projects and filters.')
