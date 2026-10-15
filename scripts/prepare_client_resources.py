"""Restore verified CN0261 HUD portrait mappings in an isolated runtime archive."""

import argparse
import copy
import hashlib
import io
import json
from pathlib import Path
import re
import struct
import xml.etree.ElementTree as etree
import zipfile


IMAGE_HASH = 'e743bb798175597ccde623bfe3d986395f7fe22ec86cc681804fec0a29406d49'
ARCHIVE_HASH = '3c1233d45678dff0a30c4f727efb5d58094eb5810aa113b57bb4136bcab6d7d2'
BASE_HASH = '97838c87ddb15e763eae644e38805c42c44833146cb81743ec5c22abd9537829'
DONOR_HASH = 'fff797604f64e1445f7926516a21820192db7e1e11fff1046dffbf321666ba4d'
ARCHIVE_PATH = 'Models/Characters/Characters.bfz'
BASE_PATH = 'Models/Characters/AniList.xml'
DONOR_PATH = 'Models/Characters/AniList_5R.xml'
MOTIONS = ('normal', 'boring', 'hpLow', 'dash', 'cover', 'respawn', 'attacked')
CHARACTERS = ('Dao', 'Dizni', 'Bazzi', 'Ethi', 'Kephi', 'Marid', 'Uni', 'Mos', 'Leo', 'Lavi', 'Twotop')
SPREADSHEET = 'urn:schemas-microsoft-com:office:spreadsheet'
SS = '{' + SPREADSHEET + '}'


def digest(data):
  return hashlib.sha256(data).hexdigest()


def resource_password(image):
  if digest(image) != IMAGE_HASH:
    raise ValueError('BubbleFighterU.exe does not match the verified CN0261 analysis image')
  pe_offset = struct.unpack_from('<I', image, 0x3C)[0]
  section_count = struct.unpack_from('<H', image, pe_offset + 6)[0]
  optional_size = struct.unpack_from('<H', image, pe_offset + 20)[0]
  image_base = struct.unpack_from('<I', image, pe_offset + 52)[0]

  def read(address, length):
    rva = address - image_base
    for index in range(section_count):
      section = pe_offset + 24 + optional_size + 40 * index
      _, start, size, offset = struct.unpack_from('<IIII', image, section + 8)
      if start <= rva and rva + length <= start + size:
        return image[offset + rva - start:offset + rva - start + length]
    raise ValueError('Resource key address is outside verified image sections')

  # Original VA 0xB06D80: six interleaved tables, indexed by the uppercase archive path.
  pointers = struct.unpack('<6I', read(0xFF2458, 24))
  keys = [read(pointer, 64) for pointer in pointers]
  archive_name = ARCHIVE_PATH.upper().encode('ascii')
  return bytes(keys[index % 6][archive_name[index % len(archive_name)] % 64] for index in range(64))


def cell_values(row):
  values = {}
  index = 1
  for cell in row.findall(SS + 'Cell'):
    index = int(cell.get(SS + 'Index', index))
    data = cell.find(SS + 'Data')
    values[index] = data.text if data is not None else ''
    index += 1
  return values


def common_table(data):
  root = etree.fromstring(data)
  matches = [sheet.find(SS + 'Table') for sheet in root.findall(SS + 'Worksheet')
       if sheet.get(SS + 'Name') == 'CommonAni']
  if len(matches) != 1 or matches[0] is None:
    raise ValueError('Expected exactly one CommonAni table')
  return matches[0]


def merge_portraits(base, donor):
  if digest(base) != BASE_HASH or digest(donor) != DONOR_HASH:
    raise ValueError('Animation tables do not match the verified CN0261 resources')
  table = common_table(base)
  rows = table.findall(SS + 'Row')
  if any(SS + 'Index' in row.attrib for row in rows):
    raise ValueError('Unexpected sparse animation rows')
  original_values = [cell_values(row) for row in rows]
  if any(values.get(3) == 'torso' and values.get(4) in MOTIONS for values in original_values):
    raise ValueError('A portrait mapping already exists; refusing to replace it')

  # Preserve the original XML byte representation, namespaces, existing rows and other sheets.
  sheet_pattern = rb'(<Worksheet ss:Name="CommonAni"[^>]*>)(.*?)(</Worksheet>)'
  base_sheet = re.search(sheet_pattern, base, re.DOTALL)
  donor_sheet = re.search(sheet_pattern, donor, re.DOTALL)
  if base_sheet is None or donor_sheet is None:
    raise ValueError('Unexpected CommonAni XML layout')
  selected = {}
  donor_rows = common_table(donor).findall(SS + 'Row')
  raw_rows = list(re.finditer(rb'<Row\b[^>]*>.*?</Row>', donor_sheet[2], re.DOTALL))
  if len(raw_rows) != len(donor_rows):
    raise ValueError('Unexpected donor row representation')
  for row, match in zip(donor_rows, raw_rows):
    values = cell_values(row)
    if values.get(2) == 'Common' and values.get(3) == 'torso' and values.get(4) in MOTIONS:
      name = values[4]
      if name in selected or values.get(6) != 'torso-' + name or values.get(7) != 'torso-' + name:
        raise ValueError('Unexpected portrait mapping in donor table')
      selected[name] = match[0]
  if set(selected) != set(MOTIONS):
    raise ValueError('Donor is missing required portrait mappings')
  body = base_sheet[2]
  end = body.index(b'</Table>')
  body = body[:end] + b'\r\n'.join(selected[name] for name in MOTIONS) + b'\r\n  ' + body[end:]
  body, replacements = re.subn(rb'ss:ExpandedRowCount="448"', b'ss:ExpandedRowCount="455"', body, count=1)
  if replacements != 1:
    raise ValueError('Unexpected original CommonAni row count')
  result = base[:base_sheet.start(2)] + body + base[base_sheet.end(2):]
  merged_values = [cell_values(row) for row in common_table(result).findall(SS + 'Row')]
  if len(merged_values) != 455 or merged_values[:len(rows)] != original_values:
    raise ValueError('Existing animation rows changed during the merge')
  return result


def replace_archive_entry(original, filename, replacement):
  """Retain all existing encrypted records; replace only the target directory entry."""
  with zipfile.ZipFile(io.BytesIO(original)) as source:
    entries = source.infolist()
    if len({item.filename for item in entries}) != len(entries):
      raise ValueError('Duplicate archive entry')
    info = copy.copy(source.getinfo(filename))
  # Both verified archives are standard ZIP32. The replacement is ordinary unencrypted DEFLATE;
  # the original reader supports both forms. Other entries keep their exact encrypted bytes.
  end = original.rfind(b'PK\x05\x06')
  if end < 0:
    raise ValueError('Missing ZIP end record')
  signature, disk, start_disk, disk_count, count, size, offset, comment_size = struct.unpack_from(
    '<4s4H2IH', original, end)
  if disk or start_disk or disk_count != count or count != len(entries) or offset + size != end:
    raise ValueError('Unsupported archive directory layout')
  if end + 22 + comment_size != len(original):
    raise ValueError('Unexpected archive trailing data')
  single = io.BytesIO()
  with zipfile.ZipFile(single, 'w') as target:
    target.writestr(info, replacement, compress_type=zipfile.ZIP_DEFLATED)
  encoded = single.getvalue()
  single_end = encoded.rfind(b'PK\x05\x06')
  single_size, single_offset = struct.unpack_from('<II', encoded, single_end + 12)
  payload_record = encoded[:single_offset]
  replacement_record = bytearray(encoded[single_offset:single_offset + single_size])
  struct.pack_into('<I', replacement_record, 42, offset)

  records = []
  cursor = offset
  for entry in entries:
    if original[cursor:cursor + 4] != b'PK\x01\x02':
      raise ValueError('Invalid archive central directory')
    name_size, extra_size, entry_comment_size = struct.unpack_from('<HHH', original, cursor + 28)
    length = 46 + name_size + extra_size + entry_comment_size
    record = original[cursor:cursor + length]
    records.append(bytes(replacement_record) if entry.filename == filename else record)
    cursor += length
  if cursor != end:
    raise ValueError('Unexpected central directory records')
  directory = b''.join(records)
  trailer = struct.pack('<4s4H2IH', signature, 0, 0, count, count, len(directory),
             offset + len(payload_record), comment_size)
  return original[:offset] + payload_record + directory + trailer + original[end + 22:]


def prepare(client_directory, runtime_directory):
  client_root = client_directory.resolve(strict=True)
  runtime_root = runtime_directory.resolve(strict=True)
  if client_root == runtime_root or runtime_root in client_root.parents:
    raise ValueError('An isolated runtime directory is required')
  output = runtime_root / ARCHIVE_PATH
  # In particular, reject a Models junction back into the original client tree.
  for parent in (runtime_root / 'Models', output.parent):
    if parent.resolve(strict=True) != parent:
      raise ValueError('The runtime Models and Characters directories must be independent directories')
  if output.exists():
    raise ValueError('The runtime character archive must not already exist')
  source_path = client_root / ARCHIVE_PATH
  original = source_path.read_bytes()
  if digest(original) != ARCHIVE_HASH:
    raise ValueError('Characters.bfz does not match the verified CN0261 resource archive')
  password = resource_password((client_root / 'BubbleFighterU.exe').read_bytes())
  with zipfile.ZipFile(io.BytesIO(original)) as source:
    base = source.read(BASE_PATH, pwd=password)
    donor = source.read(DONOR_PATH, pwd=password)
    merged = merge_portraits(base, donor)
    patched = replace_archive_entry(original, BASE_PATH, merged)
    with zipfile.ZipFile(io.BytesIO(patched)) as target:
      if target.namelist() != source.namelist():
        raise ValueError('Runtime archive entry names changed')
      for name in source.namelist():
        expected = merged if name == BASE_PATH else source.read(name, pwd=password)
        if target.read(name, pwd=password) != expected:
          raise ValueError('Runtime archive verification failed: ' + name)
  for character in CHARACTERS:
    with zipfile.ZipFile(client_root / 'Models' / 'Characters' / character / 'Common' / 'Common.bfz') as model:
      names = {name.rsplit('/', 1)[-1].casefold() for name in model.namelist()}
      if any('torso-' + name.casefold() + '.mtn' not in names for name in MOTIONS):
        raise ValueError('Missing portrait model animation: ' + character)
  evidence = dict(sourceArchive=ARCHIVE_PATH, sourceArchiveSha256=digest(original),
          runtimeArchiveSha256=digest(patched), modifiedEntry=BASE_PATH,
          sourceEntrySha256=digest(base), donorEntrySha256=digest(donor),
          runtimeEntrySha256=digest(merged), addedPortraitMotions=list(MOTIONS),
          verifiedCharacters=list(CHARACTERS), otherEntriesPreserved=True,
          originalClientFilesChanged=False)
  with output.open('xb') as stream:
    stream.write(patched)
  (runtime_root / 'ClientResourceEvidence.json').write_bytes(
    (json.dumps(evidence, indent=2) + '\n').encode('utf-8'))
  return evidence


def main():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument('--client-directory', type=Path, required=True)
  parser.add_argument('--runtime-directory', type=Path, required=True)
  arguments = parser.parse_args()
  evidence = prepare(arguments.client_directory, arguments.runtime_directory)
  print('Prepared {} verified HUD portrait mappings in the runtime archive.'.format(
    len(evidence['addedPortraitMotions'])))


if __name__ == '__main__':
  main()
